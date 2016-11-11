/**
 * Arduino - Radio Frequency Identification MFRC522
 *
 * @author Dalmir da Silva <dalmirdasilva@gmail.com>
 */

#ifndef __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_MF0ICU1_H__
#define __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_MF0ICU1_H__ 1

#include <Reader.h>
#include <Tag.h>

/**
 * MIFARE Classic 1K
 */
class TagMF0ICU1: public Tag {

public:

    enum Permission {
        LEVEL_0 = 0x00,
        LEVEL_1 = 0x01,
        LEVEL_2 = 0x02,
        LEVEL_3 = 0x03,
        LEVEL_4 = 0x04,
        LEVEL_5 = 0x05,
        LEVEL_6 = 0x06,
        LEVEL_7 = 0x07
    };

    TagMF0ICU1(Reader *reader);
};

#endif // __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_MF1S503X_H__
