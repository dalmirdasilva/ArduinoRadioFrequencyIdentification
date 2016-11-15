/**
 * Arduino - Radio Frequency Identification MFRC522
 *
 * @author Dalmir da Silva <dalmirdasilva@gmail.com>
 */

#ifndef __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_MIFARE_CLASSIC_H__
#define __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_MIFARE_CLASSIC_H__ 1

#include <Arduino.h>
#include <Reader.h>
#include <Tag.h>

#define MIFARE_CLASSIC_KEY_SIZE                         6
#define MIFARE_CLASSIC_ACCESS_BITS_SIZE                 4
#define MIFARE_CLASSIC_ACCESS_BITS_POSITION             6

#define MIFARE_CLASSIC_BLOCK_SIZE                       16

#define MIFARE_CLASSIC_LOW_SECTOR_COUNT                 32
#define MIFARE_CLASSIC_LOW_BLOCK_COUNT_IN_SECTOR        4
#define MIFARE_CLASSIC_HIGH_SECTOR_COUNT                8
#define MIFARE_CLASSIC_HIGH_BLOCK_COUNT_IN_SECTOR       16

#define MIFARE_CLASSIC_LOW_BLOCK_COUNT                  MIFARE_CLASSIC_LOW_SECTOR_COUNT * MIFARE_CLASSIC_LOW_BLOCK_COUNT_IN_SECTOR

#define MIFARE_CLASSIC_LOW_SECTOR_SIZE                  MIFARE_CLASSIC_BLOCK_SIZE * MIFARE_CLASSIC_LOW_BLOCK_COUNT_IN_SECTOR
#define MIFARE_CLASSIC_LOW_SIZE                         MIFARE_CLASSIC_LOW_SECTOR_COUNT * MIFARE_CLASSIC_LOW_SECTOR_SIZE
#define MIFARE_CLASSIC_HIGH_BLOCK_COUNT                 MIFARE_CLASSIC_HIGH_SECTOR_COUNT * MIFARE_CLASSIC_HIGH_BLOCK_COUNT_IN_SECTOR
#define MIFARE_CLASSIC_HIGH_SECTOR_SIZE                 MIFARE_CLASSIC_BLOCK_SIZE * MIFARE_CLASSIC_HIGH_BLOCK_COUNT_IN_SECTOR
#define MIFARE_CLASSIC_HIGH_SIZE                        MIFARE_CLASSIC_HIGH_SECTOR_COUNT * MIFARE_CLASSIC_HIGH_SECTOR_SIZE

#define MIFARE_CLASSIC_MINI_SECTOR_COUT                 5
#define MIFARE_CLASSIC_MINI_BLOCK_COUNT_IN_SECTOR       4
#define MIFARE_CLASSIC_MINI_BLOCK_COUNT                 MIFARE_CLASSIC_MINI_SECTOR_COUT * MIFARE_CLASSIC_MINI_BLOCK_COUNT_IN_SECTOR

#define MIFARE_CLASSIC_KEY_TYPE_TO_POS(type)            (((type) == KEY_A) ? 0 : 10)

class MifareClassic: public Tag {

public:

    enum KeyType {
        KEY_A = 0x00,
        KEY_B = 0x01
    };

    enum BlockType {
        DATA_BLOCK = 0x00,
        VALUE_BLOCK = 0x01,
    };

    enum Access {
        CONDITION_0 = 0x00,
        CONDITION_1 = 0x01,
        CONDITION_2 = 0x02,
        CONDITION_3 = 0x03,
        CONDITION_4 = 0x04,
        CONDITION_5 = 0x05,
        CONDITION_6 = 0x06,
        CONDITION_7 = 0x07
    };

    MifareClassic(Reader *reader);

    MifareClassic(Reader *reader, TagType type, TagSize size);

    /**
     * This function authenticates one card's sector (according to the block address) using the specified
     * master key A or B, addressed with type (KEY_A or KEY_B). Having send the command to the card the function
     * waits for the card's answer.
     *
     * @param   address             The block address.
     * @param   type                One of possible keys type (KEY_A or KEY_B).
     * @param   key                 Array of 6 bytes corresponding to the key.
     * @return  bool                The successfulness of the operation.
     */
    bool authenticate(unsigned char address, KeyType type, unsigned char *key);

    /**
     * After successfully authenticated, a block can be read. This operations tries to read
     * 16 bytes of the EEPROM.
     *
     * NOTE: It also depends on the configuration of the access bits of the corresponding sector.
     *
     * @param   address             The block address.
     * @param   buf                 The buffer where read data will be stored.
     * @return  bool                The successfulness of the operation.
     */
    bool readBlock(unsigned char address, unsigned char *buf);

    /**
     * After successfully authenticated, a block can be write. This operations tries to read
     * 16 bytes of the EEPROM.
     *
     * NOTE: It also depends on the configuration of the access bits of the corresponding sector.
     *
     * @param   address             The block address.
     * @param   buf                 The buffer containing the data to be used.
     * @return  bool                The successfulness of the operation.
     */
    bool writeBlock(unsigned char address, unsigned char *buf);

    /**
     * It uses @see{readBlock} method to read a block, then slice the block returning
     * only the desired portion of the block.
     *
     * @param   address             The block address.
     * @param   from                Index of the first byte of the slice into the block.
     * @param   len                 How many bytes the slice will contain.
     * @param   buf                 The buffer where slice will be stored.
     * @return  bool                The successfulness of the operation.
     */
    bool readBlockSlice(unsigned char address, unsigned char from, unsigned char len, unsigned char *buf);

    /**
     * It uses a combination of @see{readBlock} and @see{writeBlock} methods to read a block, then apply the
     * slice into it and finally write the block again into the device.
     *
     * @param   address             The block address.
     * @param   from                Index of the first byte of the slice into the block.
     * @param   len                 How many bytes the slice contains.
     * @param   buf                 The buffer containing the data to be used for the slice.
     * @return  bool                The successfulness of the operation.
     */
    bool writeBlockSlice(unsigned char address, unsigned char from, unsigned char len, unsigned char *buf);

    /**
     * It uses @see{readBlock} to read a block, then it returns only the byte at the pos position.
     *
     * @param   address             The block address.
     * @param   pos                 The position of the byte inside the block.
     * @return  int                 The read byte or -1 if error.
     */
    int readByte(unsigned char address, unsigned char pos);

    bool writeByte(unsigned char address, unsigned char pos, unsigned char value);

    bool increment(unsigned char address, uint32_t delta);

    bool decrement(unsigned char address, uint32_t delta);

    bool restore(unsigned char address);

    bool arithmeticOperation(unsigned char operation, unsigned char address, uint32_t delta);

    bool transfer(unsigned char address);

    bool createValueBlock(unsigned char address, uint32_t value, uint8_t addr);

    bool readAccessBits(unsigned char sector, unsigned char *accessBits);

    bool writeAccessBits(unsigned char sector, unsigned char *accessBits, unsigned char *keyA, unsigned char *keyB);

    /**
     * For the first 32 sectors (first 2K bytes of NV-memory) the access conditions can be set individually for a data area sized one block.
     * For the last 8 sectors (upper 2K bytes of NV-memory) access conditions can be set individually for a data area sized 5 blocks.
     *
     * BE CAREFUL WHEN CHANGING ACCESS BITS, EXPECIALLY THE PERMISSIONS TO THE SECTOR 3 (trailer sector), SINCE THE ACCESS BITS THEMSELVES CAN ALSO BE BLOCKED.
     * THERE ONLY 3 PERMISSIONS THAT ALLOW FURTHER CHANGES ON ACCESS BITS: CONDITION_1, CONDITION_3 AND CONDITION_4; EVERY OTHER, BLOCKS THEM FOREVER.
     */
    bool setAccessCondition(unsigned char sector, unsigned char block, Access access, unsigned char *keyA, unsigned char *keyB);

    bool getAccessCondition(unsigned char address, Access *access);

    /**
     * Remark: With each memory access the internal logic verifies the format of the access conditions.
     * If it detects a format violation the whole sector is irreversible blocked.
     */
    bool writeKey(unsigned char sector, KeyType type, unsigned char *keyA, unsigned char *keyB);

    bool readKey(unsigned char sector, KeyType type, unsigned char *key);

    void setupAuthenticationKey(KeyType keyType, unsigned char *key);

    bool isAccessBitsCorrect(unsigned char *accessBits);

    void packAccessBits(unsigned char *accessBits, unsigned char c1, unsigned char c2, unsigned char c3);

    void unpackAccessBits(unsigned char *accessBits, unsigned char *c1, unsigned char *c2, unsigned char *c3);

    void setSectorTrailerProtected(bool protect);

    unsigned char getSectorCount();

    unsigned int getBlockCount();

    unsigned char getBlockCountInSector(unsigned char sector);

    unsigned int getSectorSize(unsigned char sector);

    unsigned char isAddressSectorTrailer(unsigned char address);

    unsigned char addressToSector(unsigned char address);

    unsigned char addressToBlock(unsigned char address);

    unsigned char getSectorTrailerAddress(unsigned char sector);

protected:

    KeyType keyType;

    unsigned char *key;

    bool sectorTrailerProtected;

    void fillValueBlock(unsigned char *buf, uint32_t value, uint8_t addr);
};

#endif // __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_MIFARE_CLASSIC_H__
