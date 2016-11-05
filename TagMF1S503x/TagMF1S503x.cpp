#include "TagMF1S503x.h"
#include <Reader.h>

TagMF1S503x::TagMF1S503x(Reader *reader)
        : Tag(reader) {
}

bool TagMF1S503x::detect(unsigned char command) {
    unsigned char buf[2] = { command, 0x00 };
    reader->setBitFraming(0, 0x07);
    bool ok = reader->tranceive(buf, buf, 1) >= 0;
    if (ok) {
        supportsAnticollision = buf[0] & MF1S503X_ATQA_ANTICOLLISION_BIT;
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
    return wakeUp() && hasAnticollisionSupport() && anticollision() && select();
}

bool TagMF1S503x::anticollision() {
    return reader->anticollision() >= 0;
}

bool TagMF1S503x::select() {
    return reader->select() >= 0;
}

bool TagMF1S503x::halt() {

    unsigned char buf[4] = { HLT_A, 0, 0, 0 };

    // Calculate CRC_A
    reader->calculateCrc(buf, 2, &buf[2]);

    // If the PICC responds with any modulation during a period of 1 ms after the end of the frame containing the
    // HLTA command, this response shall be interpreted as 'not acknowledge'.
    return reader->tranceive(buf, NULL, 4) >= 0;
}

bool TagMF1S503x::authenticate(unsigned char keyType, unsigned char blockAddress, unsigned char *key, Uid *uid) {

    unsigned char buf[12];
    buf[0] = (keyType == KEY_A) ? AUTH_KEY_A : AUTH_KEY_B;
    buf[1] = blockAddress;
    for (unsigned char i = 0; i < TAG_MF1S503X_KEY_SIZE; i++) {
        buf[2 + i] = key[i];
    }
    for (unsigned char i = 0; i < 4; i++) {
        buf[8 + i] = uid->uid[i];
    }

    // Start the authentication.
    return reader->authenticate(buf, 12) >= 0;
}

bool TagMF1S503x::readBlock(unsigned char blockAddress, unsigned char *buf) {

    // Build command buffer
    buf[0] = READ;
    buf[1] = blockAddress;

    // Calculate CRC_A
    reader->calculateCrc(buf, 2, &buf[2]);

    // Transmit the buffer and receive the response, validate CRC_A.
    return reader->tranceive(buf, buf, 4) >= 0;
}

bool TagMF1S503x::writeBlock(unsigned char blockAddress, unsigned char *buf) {
    return true;
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
