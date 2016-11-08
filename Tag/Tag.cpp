/**
 * Arduino - Radio Frequency Identification
 *
 * @author Dalmir da Silva <dalmirdasilva@gmail.com>
 */

#include "Tag.h"

Tag::Tag(Reader *reader)
        : reader(reader), tagType(UNDEFINED), uid( { 0 }), supportsAnticollision(false), state(POWER_OFF) {
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
