/**
 * Arduino - Radio Frequency Identification
 *
 * @author Dalmir da Silva <dalmirdasilva@gmail.com>
 */

#include <Arduino.h>
#include "Tag.h"

Tag::Tag(Reader *reader)
        : reader(reader), tagType(MIFARE_UNKNOWN), uid( { 0 }), supportsAnticollision(false), state(POWER_OFF), keyType(KEY_A), key(NULL), sectorTrailerProtected(
        true) {
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

bool Tag::activate() {
    return request() && hasAnticollisionSupport() && select();
}

bool Tag::activateWakeUp() {
    return wakeUp() && hasAnticollisionSupport() && select();
}

bool Tag::detect(unsigned char command) {
    unsigned char buf[2] = { command, 0x00 };
    reader->stopCrypto();
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
    reader->stopCrypto();
    reader->calculateCrc(buf, 2, &buf[2]);
    reader->tranceive(buf, buf, 4);
    setState(HALT);

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
    if (isAddressSectorTrailer(address) && sectorTrailerProtected) {
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
    if (len == 0 || from + len > TAG_BLOCK_SIZE) {
        return false;
    }
    if (from == 0 && len == TAG_BLOCK_SIZE && !readBlock(address, receive)) {
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

bool Tag::increment(unsigned char address, uint32_t delta) {
    return arithmeticOperation(INCREMENT, address, delta);
}

bool Tag::decrement(unsigned char address, uint32_t delta) {
    return arithmeticOperation(DECREMENT, address, delta);
}

bool Tag::restore(unsigned char address) {
    return arithmeticOperation(RESTORE, address, 0);
}

bool Tag::arithmeticOperation(unsigned char operation, unsigned char address, uint32_t delta) {
    unsigned char cmd[6];
    if (key != NULL && !authenticate(address, keyType, key)) {
        return false;
    }
    cmd[0] = operation;
    cmd[1] = address;
    reader->calculateCrc(cmd, 2, &cmd[2]);
    reader->tranceive(cmd, cmd, 4);
    if (reader->getLastError() == Reader::NACK) {
        return false;
    }
    memcpy(&cmd[0], &delta, 4);
    reader->calculateCrc(cmd, 4, &cmd[4]);
    reader->tranceive(cmd, cmd, 6);
    return reader->getLastError() != Reader::NACK;
}

bool Tag::transfer(unsigned char address) {
    unsigned char cmd[4];
    if (key != NULL && !authenticate(address, keyType, key)) {
        return false;
    }
    cmd[0] = TRANSFER;
    cmd[1] = address;
    reader->calculateCrc(cmd, 2, &cmd[2]);
    reader->tranceive(cmd, cmd, 4);
    return reader->getLastError() == Reader::NO_ERROR;
}

bool Tag::createValueBlock(unsigned char address, uint32_t value, uint8_t addr) {
    Access access;
    unsigned char buf[18];
    if (!getAccessCondition(address, &access) || (access != CONDITION_1 && access != CONDITION_6)) {
        return false;
    }
    fillValueBlock(buf, value, addr);
    return writeBlock(address, buf);
}

bool Tag::readAccessBits(unsigned char sector, unsigned char *accessBits) {
    return readBlockSlice(getSectorTrailerAddress(sector), TAG_ACCESS_POSITION, TAG_ACCESS_BITS_SIZE, accessBits);
}

bool Tag::writeAccessBits(unsigned char sector, unsigned char *accessBits, unsigned char *keyA, unsigned char *keyB) {
    unsigned char buf[18];
    if (!isAccessBitsCorrect(accessBits)) {
        return false;
    }
    memcpy(&buf[TAG_KEY_TYPE_TO_POS(KEY_A)], keyA, TAG_KEY_SIZE);
    memcpy(&buf[TAG_ACCESS_POSITION], accessBits, TAG_ACCESS_BITS_SIZE);
    memcpy(&buf[TAG_KEY_TYPE_TO_POS(KEY_B)], keyB, TAG_KEY_SIZE);
    return writeBlockSlice(getSectorTrailerAddress(sector), 0, TAG_BLOCK_SIZE, buf);
}

bool Tag::setAccessCondition(unsigned char sector, unsigned char block, Access access, unsigned char *keyA, unsigned char *keyB) {
    unsigned char mask, c1, c2, c3, accessBits[4];
    if (!readAccessBits(sector, accessBits)) {
        return false;
    }
    mask = 0x01 << (block & 0x03);
    unpackAccessBits(accessBits, &c1, &c2, &c3);
    c1 = (c1 & ~mask) | ((access & 0x04) ? mask : 0);
    c2 = (c2 & ~mask) | ((access & 0x02) ? mask : 0);
    c3 = (c3 & ~mask) | ((access & 0x01) ? mask : 0);
    packAccessBits(accessBits, c1, c2, c3);
    return writeAccessBits(sector, accessBits, keyA, keyB);
}

bool Tag::getAccessCondition(unsigned char address, Access *access) {
    unsigned char block, mask, c1, c2, c3, accessBits[4];
    if (!readAccessBits(addressToSector(address), accessBits)) {
        return false;
    }
    block = addressToBlock(address) & 0x03;
    mask = 0x01 << block;
    unpackAccessBits(accessBits, &c1, &c2, &c3);
    *access = (Access) ((((c1 & mask) ? 0x04 : 0) | ((c2 & mask) ? 0x02 : 0) | ((c3 & mask) ? 0x01 : 0)) & 0x07);
    return true;
}

bool Tag::writeKey(unsigned char sector, KeyType type, unsigned char *keyA, unsigned char *keyB) {
    unsigned char buf[18];
    unsigned address = getSectorTrailerAddress(sector);
    if (!readBlock(address, buf)) {
        return false;
    }
    memcpy(&buf[TAG_KEY_TYPE_TO_POS(KEY_A)], keyA, TAG_KEY_SIZE);
    memcpy(&buf[TAG_KEY_TYPE_TO_POS(KEY_B)], keyB, TAG_KEY_SIZE);
    return writeBlock(address, buf);
}

bool Tag::readKey(unsigned char sector, KeyType type, unsigned char *key) {
    unsigned from = TAG_KEY_TYPE_TO_POS(type);
    return readBlockSlice(getSectorTrailerAddress(sector), from, TAG_KEY_SIZE, key);
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

bool Tag::isAccessBitsCorrect(unsigned char *accessBits) {
    unsigned char *c = accessBits;
    return (~(c[1] >> 4) & 0x0f) == (c[0] & 0x0f) && (~(c[2]) & 0x0f) == ((c[0] >> 4) & 0x0f) && (~(c[2] >> 4) & 0x0f) == (c[1] & 0x0f);
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

void Tag::packAccessBits(unsigned char *accessBits, unsigned char c1, unsigned char c2, unsigned char c3) {
    accessBits[0] = (~(c2 << 4) & 0xf0) | ((~c1) & 0x0f);
    accessBits[1] = ((c1 << 4) & 0xf0) | ((~c3) & 0x0f);
    accessBits[2] = ((c3 << 4) & 0xf0) | (c2 & 0x0f);
}

void Tag::unpackAccessBits(unsigned char *accessBits, unsigned char *c1, unsigned char *c2, unsigned char *c3) {
    *c1 = (accessBits[1] >> 4) & 0x0f;
    *c2 = accessBits[2] & 0x0f;
    *c3 = (accessBits[2] >> 4) & 0x0f;
}

void Tag::fillValueBlock(unsigned char *buf, uint32_t value, uint8_t addr) {
    memcpy(&buf[0], &value, 4);
    memcpy(&buf[8], &value, 4);
    value = ~value;
    memcpy(&buf[4], &value, 4);
    buf[12] = addr;
    buf[13] = ~addr;
    buf[14] = addr;
    buf[15] = ~addr;
}

void Tag::setSectorTrailerProtected(bool protect) {
    sectorTrailerProtected = protect;
}
