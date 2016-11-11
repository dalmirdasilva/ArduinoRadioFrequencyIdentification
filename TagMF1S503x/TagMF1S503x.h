/**
 * Arduino - Radio Frequency Identification MFRC522
 *
 * @author Dalmir da Silva <dalmirdasilva@gmail.com>
 */

#ifndef __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_MF1S503X_H__
#define __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_MF1S503X_H__ 1

#include <Arduino.h>
#include <Reader.h>
#include <Tag.h>

#define TAG_MF1S503X_SECTOR_SIZE    4

/**
 * MIFARE Classic 1K
 */
class TagMF1S503x: public Tag {

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

    TagMF1S503x(Reader *reader);

    unsigned char getSectorSize(unsigned char sector);

    unsigned char isAddressSectorTrailer(unsigned char address);

    unsigned char addressToSector(unsigned char address);

    unsigned char getSectorTrailerAddress(unsigned char sector);
};

#endif // __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_MF1S503X_H__
