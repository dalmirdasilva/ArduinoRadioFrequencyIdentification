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

    /**
     * This function authenticates one card's sector (according to the block address) using the specified
     * master key A or B, addressed with auth_mode. Having send the command to the card the function
     * waits for the card's answer. This function is calling compatible with authentication functions former
     * reader IC's. The keys are stored by the microcontroller, which should be capable for the key management.
     */
    virtual bool authenticate(unsigned char address, KeyType type, unsigned char *key);

    virtual bool readBlock(unsigned char address, unsigned char *buf);

    virtual bool writeBlock(unsigned char address, unsigned char *buf);

    virtual bool readBlockSlice(unsigned char address, unsigned char from, unsigned char len, unsigned char *buf);

    virtual bool writeBlockSlice(unsigned char address, unsigned char from, unsigned char len, unsigned char *buf);

    virtual int readByte(unsigned char address, unsigned char pos);

    virtual bool writeByte(unsigned char address, unsigned char pos, unsigned char value);

    /**
     * Remark: The MIFARE Increment, Decrement, and Restore command part 2 does not
     * provide an acknowledgement, so the regular time-out has to be used instead.
     */
    virtual bool increment(unsigned char address, uint32_t delta);

    virtual bool decrement(unsigned char address, uint32_t delta);

    virtual bool restore(unsigned char address);

    virtual bool arithmeticOperation(unsigned char operation, unsigned char address, uint32_t delta);

    virtual bool transfer(unsigned char address);

    virtual bool createValueBlock(unsigned char address, uint32_t value, uint8_t addr);

    virtual bool readAccessBits(unsigned char sector, unsigned char *accessBits);

    virtual bool writeAccessBits(unsigned char sector, unsigned char *accessBits, unsigned char *keyA, unsigned char *keyB);

    /**
     * For the first 32 sectors (first 2K bytes of NV-memory) the access conditions can be set individually for a data area sized one block.
     * For the last 8 sectors (upper 2K bytes of NV-memory) access conditions can be set individually for a data area sized 5 blocks.
     *
     * BE CAREFUL WHEN CHANGING ACCESS BITS, EXPECIALLY THE PERMISSIONS TO THE SECTOR 3 (trailer sector), SINCE THE ACCESS BITS THEMSELVES CAN ALSO BE BLOCKED.
     * THERE ONLY 3 PERMISSIONS THAT ALLOW FURTHER CHANGES ON ACCESS BITS: CONDITION_1, CONDITION_3 AND CONDITION_4; EVERY OTHER BLOCKS THEM FOREVER.
     */
    virtual bool setAccessCondition(unsigned char sector, unsigned char block, Access access, unsigned char *keyA, unsigned char *keyB);

    virtual bool getAccessCondition(unsigned char address, Access *access);

    /**
     * Remark: With each memory access the internal logic verifies the format of the access conditions.
     * If it detects a format violation the whole sector is irreversible blocked.
     */
    virtual bool writeKey(unsigned char sector, KeyType type, unsigned char *keyA, unsigned char *keyB);

    virtual bool readKey(unsigned char sector, KeyType type, unsigned char *key);

    virtual void setupAuthenticationKey(KeyType keyType, unsigned char *key);

    virtual void setSectorTrailerProtected(bool protect);

    virtual bool isAccessBitsCorrect(unsigned char *accessBits);

    virtual void packAccessBits(unsigned char *accessBits, unsigned char c1, unsigned char c2, unsigned char c3);

    virtual void unpackAccessBits(unsigned char *accessBits, unsigned char *c1, unsigned char *c2, unsigned char *c3);



    virtual unsigned int getSize() = 0;

    virtual unsigned char getSectorCount() = 0;

    virtual unsigned int getBlockCount() = 0;

    virtual unsigned char getBlockCountInSector(unsigned char sector) = 0;

    virtual unsigned int getSectorSize(unsigned char sector) = 0;

    virtual unsigned char isAddressSectorTrailer(unsigned char address) = 0;

    virtual unsigned char addressToSector(unsigned char address) = 0;

    virtual unsigned char addressToBlock(unsigned char address) = 0;

    virtual unsigned char getSectorTrailerAddress(unsigned char sector) = 0;

protected:

    KeyType keyType;

    unsigned char *key;

    bool sectorTrailerProtected;

    void fillValueBlock(unsigned char *buf, uint32_t value, uint8_t addr);
};

#endif // __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_MIFARE_CLASSIC_H__
