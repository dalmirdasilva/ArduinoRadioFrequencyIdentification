/**
 * Arduino - Radio Frequency Identification
 *
 * @author Dalmir da Silva <dalmirdasilva@gmail.com>
 */

#include <Arduino.h>
#include "Tag.h"

Tag::Tag(Reader *reader)
        : reader(reader) {
}

Tag::~Tag() {
}


