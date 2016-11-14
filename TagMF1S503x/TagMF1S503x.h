/**
 * Arduino - Radio Frequency Identification MFRC522
 *
 * @author Dalmir da Silva <dalmirdasilva@gmail.com>
 */

#ifndef __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_MF1S503X_H__
#define __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_MF1S503X_H__ 1

#include <Arduino.h>
#include <Reader.h>
#include <MifareClassic.h>

#define TAG_MF1S503X_SECTOR_COUNT               16
#define TAG_MF1S503X_BLOCK_SIZE                 16
#define TAG_MF1S503X_BLOCK_COUNT_IN_SECTOR      4

#define TAG_MF1S503X_BLOCK_COUNT                TAG_MF1S503X_SECTOR_COUNT * TAG_MF1S503X_BLOCK_COUNT_IN_SECTOR
#define TAG_MF1S503X_SECTOR_SIZE                TAG_MF1S503X_BLOCK_SIZE * TAG_MF1S503X_BLOCK_COUNT_IN_SECTOR
#define TAG_MF1S503X_SIZE                       TAG_MF1S503X_SECTOR_COUNT * TAG_MF1S503X_SECTOR_SIZE

class TagMF1S503x: public MifareClassic {

public:

    TagMF1S503x(Reader *reader);

    bool writeBlock(unsigned char address, unsigned char *buf);

    unsigned int getSize();

    unsigned char getSectorCount();

    unsigned int getBlockCount();

    unsigned char getBlockCountInSector(unsigned char sector);

    unsigned int getSectorSize(unsigned char sector);

    unsigned char isAddressSectorTrailer(unsigned char address);

    unsigned char addressToSector(unsigned char address);

    unsigned char addressToBlock(unsigned char address);

    unsigned char getSectorTrailerAddress(unsigned char sector);
};

#endif // __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_MF1S503X_H__
