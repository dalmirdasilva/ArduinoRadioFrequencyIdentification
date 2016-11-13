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

class TagMF1S503x: public Tag {

public:

    TagMF1S503x(Reader *reader);

    bool writeBlock(unsigned char address, unsigned char *buf);

    unsigned char getSectorSize(unsigned char sector);

    unsigned char isAddressSectorTrailer(unsigned char address);

    unsigned char addressToSector(unsigned char address);

    unsigned char addressToBlock(unsigned char address);

    unsigned char getSectorTrailerAddress(unsigned char sector);
};

#endif // __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_MF1S503X_H__
