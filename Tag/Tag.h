/**
 * Arduino - Radio Frequency Identification
 *
 * @author Dalmir da Silva <dalmirdasilva@gmail.com>
 */

#ifndef __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_H__
#define __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_H__ 1

#include <Reader.h>

#define TAG_SAK_BIT                             0x20
#define TAG_ATQA_ANTICOLLISION_BIT              0x04

#define TAG_KEY_SIZE                            0x06
#define TAG_DEFAULT_SECTOR_SIZE                 0x04

#define TAG_KEY_TO_POS(key)                     ((type == KEY_A) ? 0 : 10)

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
        MIFARE_UNKNOWN = 0X00,
        MIFARE_MINI = 0X01,
        MIFARE_1K = 0X02,
        MIFARE_4K = 0X03,
        MIFARE_UL = 0X04,
        MIFARE_PLUS = 0x06,
        MIFARE_NOT_COMPLETE = 0xff
    };

    enum KeyType {
        KEY_A = 0x00,
        KEY_B = 0x01
    };

    enum BlockType {
        DATA_BLOCK = 0x00,
        VALUE_BLOCK = 0x01,
    };

    // All MIFARE Classic commands use the MIFARE Crypto1 and require an authentication.
    enum Command {

        // REQuest command, Type A. Invites PICCs in state IDLE to go to READY and prepare for anticollision or selection. 7 bit frame.
        REQUEST = 0x26,

        // Wake-UP command, Type A. Invites PICCs in state IDLE and HALT to go to READY(*) and prepare for anticollision or selection. 7 bit frame.
        WAKE_UP = 0x52,

        // Anti collision/Select, Cascade Level 1
        SEL_CL1 = 0x93,

        // Anti collision/Select, Cascade Level 2
        SEL_CL2 = 0x95,

        // Anti collision/Select, Cascade Level 3
        SEL_CL3 = 0x97,

        // HaLT command, Type A. Instructs an ACTIVE PICC to go to state HALT.
        HLT_A = 0x50,

        // Perform authentication with Key A.
        AUTH_KEY_A = 0x60,

        // Perform authentication with Key B.
        AUTH_KEY_B = 0x61,

        // Reads one 16 byte block from the authenticated sector of the PICC. Also used for MIFARE Ultralight.
        READ = 0x30,

        // Writes one 16 byte block to the authenticated sector of the PICC. Called "COMPATIBILITY WRITE" for MIFARE Ultralight.
        WRITE = 0xa0,

        // Decrements the contents of a block and stores the result in the internal data register.
        DECREMENT = 0xc0,

        // Increments the contents of a block and stores the result in the internal data register.
        INCREMENT = 0xc1,

        // Reads the contents of a block into the internal data register.
        RESTORE = 0xc2,

        // Writes the contents of the internal data register to a block.
        TRANSFER = 0xb0,
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

    Uid getUid();

    bool hasAnticollisionSupport();

    TagType getTagType();

    void setState(State state);

    State getState();

    virtual bool detect(unsigned char command);

    /**
     * This function performs a 'Request-Idle', 'Anticollision', 'Select' sequence to activate the PICC and
     * change its state from IDLE to ACTIVE state. Cascaded serial numbers are handled correctly.
     */
    virtual bool activateIdle();

    /**
     * This function performs a 'Request-All', 'Anticollision', 'Select' sequence to activate the PICC and
     * change its state from IDLE to ACTIVE state. Cascaded serial numbers are handled correctly.
     */
    virtual bool activateWakeUp();

    virtual bool request();

    virtual bool wakeUp();
    
    virtual bool select();

    virtual bool halt();

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

    virtual bool decrement();

    virtual bool increment();

    virtual bool restore();

    virtual bool transfer();

    virtual bool setBlockType(unsigned char address, BlockType type);

    virtual bool readAccessBits(unsigned char sector, unsigned char *buf);

    virtual bool writeAccessBits(unsigned char sector, unsigned char *buf);

    virtual bool setBlockPermission(unsigned char address, unsigned char permission);

    virtual bool writeKey(unsigned char sector, KeyType type, unsigned char *key);

    virtual bool readKey(unsigned char sector, KeyType type, unsigned char *key);

    virtual void setupAuthenticationKey(KeyType keyType, unsigned char *key);

    virtual void setSectorTrailerProtected(bool protect);

protected:

    Reader *reader;

    TagType tagType;

    Uid uid;

    bool supportsAnticollision;

    State state;

    KeyType keyType;

    unsigned char *key;

    bool sectorTrailerProtected;

    unsigned char computeNvb(unsigned char collisionPos);

    virtual unsigned char getSectorSize(unsigned char sector) = 0;

    virtual unsigned char isAddressSectorTrailer(unsigned char address) = 0;

    virtual unsigned char addressToSector(unsigned char address) = 0;

    virtual unsigned char getSectorTrailerAddress(unsigned char sector) = 0;

    void computeTagType();
};

#endif // __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_H__
