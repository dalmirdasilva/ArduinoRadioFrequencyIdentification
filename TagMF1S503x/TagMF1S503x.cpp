#include "TagMF1S503x.h"
#include <Reader.h>

TagMF1S503x::TagMF1S503x(Reader *reader)
        : Tag(reader), keyType(KEY_A), key(NULL), allowSectorTrailerWrite(false) {
}

bool TagMF1S503x::activateIdle() {
    return request() && hasAnticollisionSupport() && select();
}

bool TagMF1S503x::activateWakeUp() {
    return wakeUp() && hasAnticollisionSupport() && select();
}

bool TagMF1S503x::detect(unsigned char command) {
    unsigned char buf[2] = { command, 0x00 };
    reader->turnOffEncryption();
    reader->setBitFraming(0, 0x07);
    bool ok = reader->tranceive(buf, buf, 1) >= 0;
    if (ok) {
        setState(READY);
        supportsAnticollision = buf[0] & ATQA_ANTICOLLISION_BIT;
    }
    return ok;
}

bool TagMF1S503x::request() {
    return detect(REQUEST);
}

bool TagMF1S503x::wakeUp() {
    return detect(WAKE_UP);
}

bool TagMF1S503x::select() {

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
    // Each cascade level we receive 4 bytes corresponding to the CT + ID at that cascade level, followed by the BCC byte (xor of the id).
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
                needNextCascadeLevel = (uid.sak & SAK_BIT) > 0;
                unsigned char size = 4 - needNextCascadeLevel;
                memcpy(p, &send[2 + needNextCascadeLevel], size);
                p += size;
                uid.size += size;
            }
        } while (error == Reader::COLLISION_ERROR);
    }
    setState(ACTIVE);
    return true;
}

bool TagMF1S503x::halt() {

    unsigned char buf[4] = { HLT_A, 0, 0, 0 };
    reader->turnOffEncryption();
    setState(HALT);

    // Calculate CRC_A
    reader->calculateCrc(buf, 2, &buf[2]);

    reader->tranceive(buf, buf, 4);

    // If the PICC responds with any modulation during a period of 1 ms after the end of the frame containing the
    // HLTA command, this response shall be interpreted as 'not acknowledge'.
    return reader->getLastError() == Reader::TIMEOUT_ERROR;
}

bool TagMF1S503x::authenticate(unsigned char address, KeyType type, unsigned char *key) {

    unsigned char buf[12];
    if (getState() != ACTIVE) {
        return false;
    }
    buf[0] = (type == KEY_A) ? AUTH_KEY_A : AUTH_KEY_B;
    buf[1] = address;
    for (unsigned char i = 0; i < KEY_SIZE; i++) {
        buf[2 + i] = key[i];
    }
    for (unsigned char i = 0; i < 4; i++) {
        buf[8 + i] = uid.uid[i];
    }

    // Perform the authentication.
    return reader->authenticate(buf) >= 0;
}

bool TagMF1S503x::readBlock(unsigned char address, unsigned char *buf) {

    if (key != NULL && !authenticate(address, keyType, key)) {
        return false;
    }

    // Build command buffer
    buf[0] = READ;
    buf[1] = address;

    // Calculate CRC_A
    reader->calculateCrc(buf, 2, &buf[2]);

    // Transmit the buffer and receive the response, validate CRC_A.
    return reader->tranceive(buf, buf, 4, true) == 18;
}

bool TagMF1S503x::writeBlock(unsigned char address, unsigned char *buf) {

    unsigned char cmd[4];

    if (TAG_MF1S503X_ADDR_IS_SEC_TRAILER(address) && !allowSectorTrailerWrite) {
        return false;
    }

    if (key != NULL && !authenticate(address, keyType, key)) {
        return false;
    }

    // Build command buffer
    cmd[0] = WRITE;
    cmd[1] = address;

    // Calculate CRC_A
    reader->calculateCrc(cmd, 2, &cmd[2]);

    // Transmit the buffer and receive the response.
    reader->tranceive(cmd, cmd, 4);

    // Check if nack was received.
    if (reader->getLastError() == Reader::NACK) {
        return false;
    }

    // Calculate CRC_A
    reader->calculateCrc(buf, 16, &buf[16]);

    // Transmit the buffer and receive the response, validate CRC_A.
    reader->tranceive(buf, buf, 18);

    return reader->getLastError() != Reader::NACK;
}

bool TagMF1S503x::readBlockSlice(unsigned char address, unsigned char from, unsigned char to, unsigned char *buf) {
    unsigned char receive[18];
    if (to <= from || to > 16) {
        return false;
    }
    if (!readBlock(address, receive)) {
        return false;
    }
    memcpy(buf, &receive[from], to - from);
    return true;
}

bool TagMF1S503x::writeBlockSlice(unsigned char address, unsigned char from, unsigned char to, unsigned char *buf) {
    unsigned char receive[18];
    if (to <= from || to > 16) {
        return false;
    }
    if (!readBlock(address, receive)) {
        return false;
    }
    memcpy(&receive[from], buf, to - from);
    return writeBlock(address, receive);
}

int TagMF1S503x::readByte(unsigned char address, unsigned char pos) {

    unsigned char buf[18];
    if (!readBlock(address, buf)) {
        return -1;
    }
    return buf[pos];
}

bool TagMF1S503x::writeByte(unsigned char address, unsigned char pos, unsigned char value) {

    unsigned char buf[18];
    if (!readBlock(address, buf)) {
        return false;
    }
    buf[pos] = value;
    return writeBlock(address, buf);
}

bool TagMF1S503x::decrement() {
    return true;
}

bool TagMF1S503x::increment() {
    return true;
}

bool TagMF1S503x::restore() {
    return true;
}

bool TagMF1S503x::transfer() {
    return true;
}

bool TagMF1S503x::setBlockType(unsigned char address, BlockType type) {

    return true;
}

bool TagMF1S503x::readAccessBits(unsigned char sector, unsigned char *buf) {
    return readBlockSlice((sector * SECTOR_SIZE) + (SECTOR_SIZE - 1), 6, 10, buf);
}

bool TagMF1S503x::writeAccessBits(unsigned char sector, unsigned char *buf) {
    return writeBlockSlice((sector * SECTOR_SIZE) + (SECTOR_SIZE - 1), 6, 10, buf);
}

bool TagMF1S503x::setBlockPermission(unsigned char address, unsigned char permission) {
    return true;
}

bool TagMF1S503x::writeKey(unsigned char sector, KeyType type, unsigned char *key) {
    unsigned from = TAG_MF1S503X_KEY_TO_POS(key);
    return writeBlockSlice((sector * SECTOR_SIZE) + (SECTOR_SIZE - 1), from, from + KEY_SIZE, key);
}

bool TagMF1S503x::readKey(unsigned char sector, KeyType type, unsigned char *key) {
    unsigned from = TAG_MF1S503X_KEY_TO_POS(key);
    return readBlockSlice((sector * SECTOR_SIZE) + (SECTOR_SIZE - 1), from, from + KEY_SIZE, key);
}

void TagMF1S503x::setupAuthenticationKey(KeyType keyType, unsigned char *key) {
    this->keyType = keyType;
    this->key = key;
}

void TagMF1S503x::setAllowSectorTrailerWrite(bool allow) {
    allowSectorTrailerWrite = allow;
}

unsigned char TagMF1S503x::computeNvb(unsigned char collisionPos) {
    unsigned char bytes = collisionPos / 8;
    unsigned char bits = collisionPos % 8;
    return (((bytes << 4) & 0xf0) | (bits & 0x0f)) + 0x20;
}
