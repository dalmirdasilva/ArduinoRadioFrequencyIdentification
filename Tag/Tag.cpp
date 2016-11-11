/**
 * Arduino - Radio Frequency Identification
 *
 * @author Dalmir da Silva <dalmirdasilva@gmail.com>
 */

#include <Arduino.h>
#include "Tag.h"

Tag::Tag(Reader *reader)
        : reader(reader), tagType(MIFARE_UNKNOWN), uid( { 0 }), supportsAnticollision(false), state(POWER_OFF), keyType(KEY_A), key(NULL), sectorTrailerProtected(
        false) {
}

Tag::~Tag() {
}

Tag::Uid Tag::getUid() {
    return uid;
}

bool Tag::hasAnticollisionSupport() {
    return supportsAnticollision;
}

Tag::TagType Tag::getTagType() {
    return tagType;
}

void Tag::setState(Tag::State state) {
    this->state = state;
}

Tag::State Tag::getState() {
    return state;
}

bool Tag::activateIdle() {
    return request() && hasAnticollisionSupport() && select();
}

bool Tag::activateWakeUp() {
    return wakeUp() && hasAnticollisionSupport() && select();
}

bool Tag::detect(unsigned char command) {
    unsigned char buf[2] = { command, 0x00 };
    reader->turnOffEncryption();
    reader->setBitFraming(0, 0x07);
    bool ok = reader->tranceive(buf, buf, 1) >= 0;
    if (ok) {
        setState(READY);
        supportsAnticollision = buf[0] & TAG_ATQA_ANTICOLLISION_BIT;
    }
    return ok;
}

bool Tag::request() {
    return detect(REQUEST);
}

bool Tag::wakeUp() {
    return detect(WAKE_UP);
}

bool Tag::select() {
    if (getState() != READY) {
        return false;
    }
    Command cascadeLevels[3] = { SEL_CL1, SEL_CL2, SEL_CL3 };
    unsigned char collisionPosistion = 0, knownBytes, lastBits, send[9] = { 0 }, receive[9] = { 0 }, *p = &uid.uid[0];
    Reader::Error error;
    bool needNextCascadeLevel = true;
    uid.size = 0;
    reader->setuptForAnticollision();

    // Loop for each cascade levels.
    // Each cascade level we receive 4 bytes corresponding to the CT + ID at
    // that cascade level, followed by the BCC byte (xor of the id).
    for (unsigned char k = 0; needNextCascadeLevel && k < sizeof(cascadeLevels); k++) {

        do {
            lastBits = collisionPosistion % 8;
            knownBytes = (collisionPosistion / 8) + (lastBits ? 1 : 0);
            send[0] = cascadeLevels[k];
            send[1] = computeNvb(collisionPosistion);
            memcpy(&send[2], receive, knownBytes);
            reader->setBitFraming(lastBits, lastBits);
            reader->tranceive(send, receive, knownBytes + 2);
            collisionPosistion = 0;
            error = (Reader::Error) reader->getLastError();
            if (error != Reader::NO_ERROR && error != Reader::COLLISION_ERROR) {
                setState(IDLE);
                return false;
            }
            if (error == Reader::COLLISION_ERROR) {
                collisionPosistion = reader->getCollisionPosition();
            } else {

                // End of i_nth iteration.
                send[1] = 0x70;
                memcpy(&send[2], receive, 0x05);
                reader->calculateCrc(send, 7, &send[7]);
                reader->tranceive(send, receive, 0x09);
                if (reader->getLastError() != Reader::NO_ERROR) {
                    setState(IDLE);
                    return false;
                }
                uid.sak = receive[0];

                // TODO: Need more tests
                needNextCascadeLevel = (uid.sak & TAG_SAK_BIT) > 0;
                unsigned char size = 4 - needNextCascadeLevel;
                memcpy(p, &send[2 + needNextCascadeLevel], size);
                p += size;
                uid.size += size;
            }
        } while (error == Reader::COLLISION_ERROR);
    }
    computeTagType();
    setState(ACTIVE);
    return true;
}

bool Tag::halt() {
    unsigned char buf[4] = { HLT_A, 0, 0, 0 };
    reader->turnOffEncryption();
    setState(HALT);
    reader->calculateCrc(buf, 2, &buf[2]);
    reader->tranceive(buf, buf, 4);

    // If the PICC responds with any modulation during a period of 1 ms after the end of the frame containing the
    // HLTA command, this response shall be interpreted as 'not acknowledge'.
    return reader->getLastError() == Reader::TIMEOUT_ERROR;
}

bool Tag::authenticate(unsigned char address, KeyType type, unsigned char *key) {
    unsigned char buf[12];
    if (getState() != ACTIVE) {
        return false;
    }
    buf[0] = (type == KEY_A) ? AUTH_KEY_A : AUTH_KEY_B;
    buf[1] = address;
    for (unsigned char i = 0; i < TAG_KEY_SIZE; i++) {
        buf[2 + i] = key[i];
    }
    for (unsigned char i = 0; i < 4; i++) {
        buf[8 + i] = uid.uid[i];
    }
    return reader->authenticate(buf) >= 0;
}

bool Tag::readBlock(unsigned char address, unsigned char *buf) {
    if (key != NULL && !authenticate(address, keyType, key)) {
        return false;
    }
    buf[0] = READ;
    buf[1] = address;
    reader->calculateCrc(buf, 2, &buf[2]);
    return reader->tranceive(buf, buf, 4, true) == 18;
}

bool Tag::writeBlock(unsigned char address, unsigned char *buf) {
    unsigned char cmd[4];
    if (isAddressSectorTrailer(address) && !sectorTrailerProtected) {
        return false;
    }
    if (key != NULL && !authenticate(address, keyType, key)) {
        return false;
    }
    cmd[0] = WRITE;
    cmd[1] = address;
    reader->calculateCrc(cmd, 2, &cmd[2]);
    reader->tranceive(cmd, cmd, 4);
    if (reader->getLastError() == Reader::NACK) {
        return false;
    }
    reader->calculateCrc(buf, 16, &buf[16]);
    reader->tranceive(buf, buf, 18);
    return reader->getLastError() != Reader::NACK;
}

bool Tag::readBlockSlice(unsigned char address, unsigned char from, unsigned char len, unsigned char *buf) {
    unsigned char receive[18];
    if (len == 0 || from + len > 16) {
        return false;
    }
    if (!readBlock(address, receive)) {
        return false;
    }
    memcpy(buf, &receive[from], len);
    return true;
}

bool Tag::writeBlockSlice(unsigned char address, unsigned char from, unsigned char len, unsigned char *buf) {
    unsigned char receive[18];
    if (len == 0 || from + len > 16) {
        return false;
    }
    if (!readBlock(address, receive)) {
        return false;
    }
    memcpy(&receive[from], buf, len);
    return writeBlock(address, receive);
}

int Tag::readByte(unsigned char address, unsigned char pos) {

    unsigned char buf[18];
    if (!readBlock(address, buf)) {
        return -1;
    }
    return buf[pos];
}

bool Tag::writeByte(unsigned char address, unsigned char pos, unsigned char value) {

    unsigned char buf[18];
    if (!readBlock(address, buf)) {
        return false;
    }
    buf[pos] = value;
    return writeBlock(address, buf);
}

bool Tag::decrement() {
    return true;
}

bool Tag::increment() {
    return true;
}

bool Tag::restore() {
    return true;
}

bool Tag::transfer() {
    return true;
}

bool Tag::setBlockType(unsigned char address, BlockType type) {

    return true;
}

bool Tag::readAccessBits(unsigned char sector, unsigned char *buf) {
    return readBlockSlice(getSectorTrailerAddress(sector), 6, 10, buf);
}

bool Tag::writeAccessBits(unsigned char sector, unsigned char *buf) {
    return writeBlockSlice(getSectorTrailerAddress(sector), 6, 10, buf);
}

bool Tag::setBlockPermission(unsigned char address, unsigned char permission) {
    return true;
}

bool Tag::writeKey(unsigned char sector, KeyType type, unsigned char *key) {
    unsigned from = TAG_KEY_TO_POS(key);
    return writeBlockSlice(getSectorTrailerAddress(sector), from, from + TAG_KEY_SIZE, key);
}

bool Tag::readKey(unsigned char sector, KeyType type, unsigned char *key) {
    unsigned from = TAG_KEY_TO_POS(key);
    return readBlockSlice(getSectorTrailerAddress(sector), from, from + TAG_KEY_SIZE, key);
}

void Tag::setupAuthenticationKey(KeyType keyType, unsigned char *key) {
    this->keyType = keyType;
    this->key = key;
}

unsigned char Tag::computeNvb(unsigned char collisionPos) {
    unsigned char bytes = collisionPos / 8;
    unsigned char bits = collisionPos % 8;
    return (((bytes << 4) & 0xf0) | (bits & 0x0f)) + 0x20;
}

void Tag::computeTagType() {
    switch (uid.sak & 0x7f) {
    case 0x04:
        tagType = MIFARE_NOT_COMPLETE;
        break;
    case 0x09:
        tagType = MIFARE_MINI;
        break;
    case 0x08:
        tagType = MIFARE_1K;
        break;
    case 0x18:
        tagType = MIFARE_4K;
        break;
    case 0x00:
        tagType = MIFARE_UL;
        break;
    case 0x10:
    case 0x11:
        tagType = MIFARE_PLUS;
        break;
    default:
        tagType = MIFARE_UNKNOWN;
    }
}

void Tag::setSectorTrailerProtected(bool protect) {
    sectorTrailerProtected = protect;
}
