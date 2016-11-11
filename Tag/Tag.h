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

    enum State {
        POWER_OFF = 0x00,
        IDLE = 0x01,
        READY = 0x02,
        ACTIVE = 0x03,
        HALT = 0x04
    };

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

    enum KeyType {
        KEY_A = 0x00,
        KEY_B = 0x01
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

    /**
     * This function performs a 'Request-Idle', 'Anticollision', 'Select' sequence to activate the PICC and
     * change its state from IDLE to ACTIVE state. Cascaded serial numbers are handled correctly.
     */
    virtual bool activateIdle() = 0;

    /**
     * This function performs a 'Request-All', 'Anticollision', 'Select' sequence to activate the PICC and
     * change its state from IDLE to ACTIVE state. Cascaded serial numbers are handled correctly.
     */
    virtual bool activateWakeUp() = 0;

    virtual bool request() = 0;

    virtual bool wakeUp() = 0;
    
    virtual bool select() = 0;

    virtual bool halt() = 0;

    /**
     * This function authenticates one card's sector (according to the block address) using the specified
     * master key A or B, addressed with auth_mode. Having send the command to the card the function
     * waits for the card's answer. This function is calling compatible with authentication functions former
     * reader IC's. The keys are stored by the microcontroller, which should be capable for the key management.
     */
    virtual bool authenticate(unsigned char address, KeyType type, unsigned char *key) = 0;

    virtual bool readBlock(unsigned char address, unsigned char *buf) = 0;

    virtual bool writeBlock(unsigned char address, unsigned char *buf) = 0;

    virtual bool readBlockSlice(unsigned char address, unsigned char from, unsigned char to, unsigned char *buf) = 0;

    virtual bool writeBlockSlice(unsigned char address, unsigned char from, unsigned char to, unsigned char *buf) = 0;

    virtual int readByte(unsigned char address, unsigned char pos) = 0;

    virtual bool writeByte(unsigned char address, unsigned char pos, unsigned char value) = 0;

    virtual bool decrement() = 0;

    virtual bool increment() = 0;

    virtual bool restore() = 0;

    virtual bool transfer() = 0;

    virtual bool setBlockType(unsigned char address, BlockType type) = 0;

    virtual bool readAccessBits(unsigned char sector, unsigned char *buf) = 0;

    virtual bool writeAccessBits(unsigned char sector, unsigned char *buf) = 0;

    virtual bool setBlockPermission(unsigned char address, unsigned char permission) = 0;

    virtual bool writeKey(unsigned char sector, KeyType type, unsigned char *key) = 0;

    virtual bool readKey(unsigned char sector, KeyType type, unsigned char *key) = 0;

    Uid getUid();

    bool hasAnticollisionSupport();

    TagType getTagType();

    void setState(State state);

    State getState();

protected:

    Reader *reader;

    TagType tagType;

    Uid uid;

    bool supportsAnticollision;

    State state;
};

#endif // __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_H__
