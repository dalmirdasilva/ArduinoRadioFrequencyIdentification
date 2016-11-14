/**
 * Arduino - Radio Frequency Identification MFRC522
 *
 * @author Dalmir da Silva <dalmirdasilva@gmail.com>
 */

#ifndef __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_MF1ICS70_H__
#define __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_MF1ICS70_H__ 1

#include <Reader.h>
#include <TagMifareClassic.h>

// The 4 kByte EEPROM memory is organized in 32 sectors with 4 blocks
// and in 8 sectors with 16 blocks. One block consists of 16 bytes.

#define TAG_MF1ICS70_BLOCK_SIZE                     16
#define TAG_MF1ICS70_LOW_SECTOR_COUNT               32
#define TAG_MF1ICS70_LOW_BLOCK_COUNT_IN_SECTOR      4
#define TAG_MF1ICS70_HIGH_SECTOR_COUNT              8
#define TAG_MF1ICS70_HIGH_BLOCK_COUNT_IN_SECTOR     16
#define TAG_MF1ICS70_LOW_BLOCK_COUNT                TAG_MF1ICS70_LOW_SECTOR_COUNT * TAG_MF1ICS70_LOW_BLOCK_COUNT_IN_SECTOR
#define TAG_MF1ICS70_LOW_SECTOR_SIZE                TAG_MF1ICS70_BLOCK_SIZE * TAG_MF1ICS70_LOW_BLOCK_COUNT_IN_SECTOR
#define TAG_MF1ICS70_LOW_SIZE                       TAG_MF1ICS70_LOW_SECTOR_COUNT * TAG_MF1ICS70_LOW_SECTOR_SIZE
#define TAG_MF1ICS70_HIGH_BLOCK_COUNT               TAG_MF1ICS70_HIGH_SECTOR_COUNT * TAG_MF1ICS70_HIGH_BLOCK_COUNT_IN_SECTOR
#define TAG_MF1ICS70_HIGH_SECTOR_SIZE               TAG_MF1ICS70_BLOCK_SIZE * TAG_MF1ICS70_HIGH_BLOCK_COUNT_IN_SECTOR
#define TAG_MF1ICS70_HIGH_SIZE                      TAG_MF1ICS70_HIGH_SECTOR_COUNT * TAG_MF1ICS70_HIGH_SECTOR_SIZE
#define TAG_MF1ICS70_SIZE                           TAG_MF1ICS70_LOW_SIZE + TAG_MF1ICS70_HIGH_SIZE

class TagMF1ICS70: public TagMifareClassic {

public:

    TagMF1ICS70(Reader *reader);

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

#endif // __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_MF1ICS70_H__
