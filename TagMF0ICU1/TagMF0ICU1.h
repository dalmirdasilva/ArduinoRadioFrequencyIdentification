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

    TagMF0ICU1(Reader *reader);
};

#endif // __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_MF1S503X_H__
