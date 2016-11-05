/**
 * Arduino - Radio Frequency Identification
 *
 * @author Dalmir da Silva <dalmirdasilva@gmail.com>
 */

#ifndef __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_H__
#define __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_H__ 1

#include <Reader.h>

class Tag {

public:

    enum TagType {
        UNDEFINED = 0X00,
        MIFARE_MINI = 0X01,
        MIFARE_1K = 0X02,
        MIFARE_4K = 0X03,
        MIFARE_UL = 0X04,
        MIFARE_ULC = 0X05,
        MIFARE_PLUS_2K = 0x06,
        MIFARE_PLUS_4K = 0x07
    };

    struct Uid {

        // Number of bytes in the UID. 4, 7 or 10.
        unsigned char size;

        unsigned char uid[10];

        // The SAK (Select acknowledge) byte returned from the tag after successful selection.
        unsigned char sak;
    };

    Tag(Reader *reader);

    virtual ~Tag();

    enum BlockType {
        DATA_BLOCK = 0x00,
        VALUE_BLOCK = 0x01,
    };

    virtual bool request() = 0;

    virtual bool wakeUp() = 0;
    
    virtual bool anticollision() = 0;

    virtual bool select() = 0;

    virtual bool halt() = 0;

    virtual bool authenticate(unsigned char keyType, unsigned char blockAddress, unsigned char *key, Uid *uid) = 0;

    virtual bool readBlock(unsigned char blockAddress, unsigned char *buf) = 0;

    virtual bool writeBlock(unsigned char blockAddress, unsigned char *buf) = 0;

    virtual bool decrement() = 0;

    virtual bool increment() = 0;

    virtual bool restore() = 0;

    virtual bool transfer() = 0;

    virtual bool setBlockType(unsigned char blockAddress, BlockType type) = 0;

    virtual bool readAccessBits(unsigned char blockAddress, unsigned char *buf) = 0;

    virtual bool writeAccessBits(unsigned char blockAddress, unsigned char *buf) = 0;

    virtual bool setBlockPermission(unsigned char blockAddress, unsigned char permission) = 0;

    virtual bool writeKey(unsigned char blockAddress, unsigned char blockType, unsigned char *key) = 0;

    Uid getUid();

    bool hasAnticollisionSupport();

    TagType getTagType();

protected:

    Reader *reader;

    TagType tagType;

    Uid uid;

    bool supportsAnticollision;
};

#endif // __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_H__
