/**
 * Arduino - Radio Frequency Identification
 *
 * @author Dalmir da Silva <dalmirdasilva@gmail.com>
 */

#include <Arduino.h>
#include "Tag.h"

Tag::Tag(Reader *reader)
        : reader(reader), type(TYPE_UNKNOWN), size(SIZE_0), uid( { 0 }), supportsAnticollision(false), state(POWER_OFF_STATE) {
}

Tag::~Tag() {
}

Tag::Uid Tag::getUid() {
    return uid;
}

bool Tag::hasAnticollisionSupport() {
    return supportsAnticollision;
}

Tag::TagType Tag::getType() {
    return type;
}

void Tag::setType(TagType type) {
    this->type = type;
}

Tag::TagSize Tag::getSize() {
    return size;
}

void Tag::setSize(TagSize size) {
    this->size = size;
}

Tag::State Tag::getState() {
    return state;
}

void Tag::setState(Tag::State state) {
    this->state = state;
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

    // No need for CRC, transceive directly with reader. Do not use helper.
    bool ok = reader->transceive(buf, buf, 1) >= 0;
    if (ok) {
        setState(READY_STATE);
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
    if (getState() != READY_STATE) {
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

            // No need for CRC, transceive directly with reader. Do not use helper.
            reader->transceive(send, receive, knownBytes + 2);
            collisionPosistion = 0;
            error = (Reader::Error) reader->getLastError();
            if (error != Reader::NO_ERROR && error != Reader::COLLISION_ERROR) {
                setState(IDLE_STATE);
                return false;
            }
            if (error == Reader::COLLISION_ERROR) {
                collisionPosistion = reader->getCollisionPosition();
            } else {

                // End of i_nth iteration. No collision. 5 bytes received. Need to send 9 bytes including CRC.
                send[1] = 0x70;
                memcpy(&send[2], receive, 5);
                transceive(send, receive, 7);
                if (reader->getLastError() != Reader::NO_ERROR) {
                    setState(IDLE_STATE);
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
    setState(ACTIVE_STATE);
    return true;
}

int Tag::transceive(unsigned char *send, unsigned char *receive, unsigned char sendLen, bool checkCrc) {
    reader->calculateCrc(send, sendLen, &send[sendLen]);
    return reader->transceive(send, receive, sendLen + 2, checkCrc);
}

void Tag::computeTagType() {
    switch (uid.sak & 0x7f) {
    case 0x00:
        type = TYPE_ULTRALIGHT;
        break;
    case 0x04:
        type = TYPE_NOT_COMPLETE;
        break;
    case 0x08:
        type = TYPE_CLASSIC;
        size = SIZE_1K;
        break;
    case 0x09:
        type = TYPE_CLASSIC;
        size = SIZE_MINI;
        break;
    case 0x10:
        type = TYPE_PLUS;
        size = SIZE_2K;
        break;
    case 0x11:
        type = TYPE_PLUS;
        size = SIZE_4K;
        break;
    case 0x18:
        type = TYPE_CLASSIC;
        size = SIZE_4K;
        break;
    default:
        type = TYPE_UNKNOWN;
    }
}

unsigned char Tag::computeNvb(unsigned char collisionPos) {
    unsigned char bytes = collisionPos / 8;
    unsigned char bits = collisionPos % 8;
    return (((bytes << 4) & 0xf0) | (bits & 0x0f)) + 0x20;
}

