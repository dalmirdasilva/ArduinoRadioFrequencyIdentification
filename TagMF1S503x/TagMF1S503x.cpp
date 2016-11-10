#include "TagMF1S503x.h"
#include <Reader.h>

TagMF1S503x::TagMF1S503x(Reader *reader)
        : Tag(reader), keyType(KEY_A), key(NULL) {
}

bool TagMF1S503x::detect(unsigned char command) {
    unsigned char buf[2] = { command, 0x00 };
    reader->turnOffEncryption();
    reader->setBitFraming(0, 0x07);
    bool ok = reader->tranceive(buf, buf, 1) >= 0;
    if (ok) {
        setState(READY);
        supportsAnticollision = buf[0] & TAG_MF1S503X_ATQA_ANTICOLLISION_BIT;
    }
    return ok;
}

bool TagMF1S503x::request() {
    return detect(REQUEST);
}

bool TagMF1S503x::wakeUp() {
    return detect(WAKE_UP);
}

bool TagMF1S503x::activate() {
    return request() && hasAnticollisionSupport() && select();
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
                needNextCascadeLevel = (uid.sak & TAG_MF1S503X_SAK_BIT) > 0;
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

bool TagMF1S503x::authenticate(unsigned char keyType, unsigned char blockAddress, unsigned char *key) {

    unsigned char buf[12];
    if (getState() != ACTIVE) {
        return false;
    }
    buf[0] = (keyType == KEY_A) ? AUTH_KEY_A : AUTH_KEY_B;
    buf[1] = blockAddress;
    for (unsigned char i = 0; i < TAG_MF1S503X_KEY_SIZE; i++) {
        buf[2 + i] = key[i];
    }
    for (unsigned char i = 0; i < 4; i++) {
        buf[8 + i] = uid.uid[i];
    }

    // Perform the authentication.
    return reader->authenticate(buf) >= 0;
}

bool TagMF1S503x::readBlock(unsigned char blockAddress, unsigned char *buf) {

    if (key != NULL && !authenticate(keyType, blockAddress, key)) {
        return false;
    }

    // Build command buffer
    buf[0] = READ;
    buf[1] = blockAddress;

    // Calculate CRC_A
    reader->calculateCrc(buf, 2, &buf[2]);

    // Transmit the buffer and receive the response, validate CRC_A.
    return reader->tranceive(buf, buf, 4, true) == 18;
}

bool TagMF1S503x::writeBlock(unsigned char blockAddress, unsigned char *buf) {

    unsigned char cmd[4];
    if (key != NULL && !authenticate(keyType, blockAddress, key)) {
        return false;
    }

    // Build command buffer
    cmd[0] = WRITE;
    cmd[1] = blockAddress;

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

int TagMF1S503x::readByte(unsigned char blockAddress, unsigned char pos) {

    unsigned char buf[18];
    if (!readBlock(blockAddress, buf)) {
        return -1;
    }
    return buf[pos];
}

bool TagMF1S503x::writeByte(unsigned char blockAddress, unsigned char pos, unsigned char value) {

    unsigned char buf[18];
    if (!readBlock(blockAddress, buf)) {
        return false;
    }
    buf[pos] = value;
    return writeBlock(blockAddress, buf);
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

bool TagMF1S503x::setBlockType(unsigned char blockAddress, BlockType type) {
    return true;
}

bool TagMF1S503x::readAccessBits(unsigned char blockAddress, unsigned char *buf) {

    return true;
}

bool TagMF1S503x::writeAccessBits(unsigned char blockAddress, unsigned char *buf) {
    return true;
}

bool TagMF1S503x::setBlockPermission(unsigned char blockAddress, unsigned char permission) {
    return true;
}

bool TagMF1S503x::writeKey(unsigned char blockAddress, unsigned char blockType, unsigned char *key) {
    return true;
}

void TagMF1S503x::setupAuthenticationKey(KeyType keyType, unsigned char *key) {
    this->keyType = keyType;
    this->key = key;
}

unsigned char TagMF1S503x::computeNvb(unsigned char collisionPos) {
    unsigned char bytes = collisionPos / 8;
    unsigned char bits = collisionPos % 8;
    return (((bytes << 4) & 0xf0) | (bits & 0x0f)) + 0x20;
}
