/**
 * Arduino - Radio Frequency Identification MFRC522
 *
 * @author Dalmir da Silva <dalmirdasilva@gmail.com>
 */

#ifndef __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_MF1ICS70_H__
#define __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_MF1ICS70_H__ 1

#include <Reader.h>
#include <Tag.h>

// The 4 kByte EEPROM memory is organized in 32 sectors with 4 blocks
// and in 8 sectors with 16 blocks. One block consists of 16 bytes.

#define TAG_MF1ICS70_LOW_SECTOR_COUNT       32
#define TAG_MF1ICS70_HIGH_SECTOR_COUNT      8

#define TAG_MF1ICS70_LOW_SECTOR_SIZE        4
#define TAG_MF1ICS70_HIGH_SECTOR_SIZE       16

#define TAG_MF1ICS70_LOW_MEMORY_SIZE        TAG_MF1ICS70_LOW_SECTOR_COUNT * TAG_MF1ICS70_LOW_SECTOR_SIZE

/**
 * MIFARE Classic 4K
 */
class TagMF1ICS70: public Tag {

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

    TagMF1ICS70(Reader *reader);

    unsigned char getSectorSize(unsigned char sector);

    unsigned char isAddressSectorTrailer(unsigned char address);

    unsigned char addressToSector(unsigned char address);

    unsigned char getSectorTrailerAddress(unsigned char sector);
};

#endif // __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_MF1S503X_H__
