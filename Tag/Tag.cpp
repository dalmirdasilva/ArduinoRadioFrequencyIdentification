/**
 * Arduino - Radio Frequency Identification
 *
 * @author Dalmir da Silva <dalmirdasilva@gmail.com>
 */

#include "Tag.h"

Tag::Tag(Reader *reader)
        : reader(reader), tagType(UNDEFINED), uid( { 0 }), supportsAnticollision(false) {
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
