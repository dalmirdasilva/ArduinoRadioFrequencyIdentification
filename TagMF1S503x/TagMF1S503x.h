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

#define TAG_MF1S503X_KEY_TO_POS(key)                    ((type == KEY_A) ? 0 : 10)
#define TAG_MF1S503X_ADDR_IS_SEC_TRAILER(address)       ((address % SECTOR_SIZE) == (SECTOR_SIZE - 1))

/**
 * MIFARE Classic 1K
 */
class TagMF1S503x: public Tag {

    static const unsigned char KEY_SIZE = 0x06;
    static const unsigned char SECTOR_SIZE = 0x04;
    static const unsigned char ATQA_ANTICOLLISION_BIT = 0x04;
    static const unsigned char SAK_BIT = 0x20;

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

    TagMF1S503x(Reader *reader);

    bool activateIdle();

    bool activateWakeUp();

    bool detect(unsigned char command);

    bool request();

    bool wakeUp();

    bool select();

    bool halt();

    bool authenticate(unsigned char address, KeyType type, unsigned char *key);

    bool readBlock(unsigned char address, unsigned char *buf);

    bool writeBlock(unsigned char address, unsigned char *buf);

    bool readBlockSlice(unsigned char address, unsigned char from, unsigned char to, unsigned char *buf);

    bool writeBlockSlice(unsigned char address, unsigned char from, unsigned char to, unsigned char *buf);

    int readByte(unsigned char address, unsigned char pos);

    /**
     * It just work if you call setupAuthenticationKey before it.
     * As it uses read and write operation, and the internal logic of the MF1S503x
     * ensures that the commands are executed only after a successful authentication.
     * Therefore, it is not possible to execute those 2 operations without re-authenticate.
     * As this method is intended to do they both, it requires that the read and write
     * operations are auto-authenticated.
     */
    bool writeByte(unsigned char address, unsigned char pos, unsigned char value);

    bool decrement();

    bool increment();

    bool restore();

    bool transfer();

    bool setBlockType(unsigned char address, BlockType type);

    bool readAccessBits(unsigned char sector, unsigned char *buf);

    bool writeAccessBits(unsigned char sector, unsigned char *buf);

    bool setBlockPermission(unsigned char address, unsigned char permission);

    bool writeKey(unsigned char sector, KeyType type, unsigned char *key);

    bool readKey(unsigned char sector, KeyType type, unsigned char *key);

    void setupAuthenticationKey(KeyType keyType, unsigned char *key);

    void setAllowSectorTrailerWrite(bool allow);

private:

    KeyType keyType;

    unsigned char *key;

    bool allowSectorTrailerWrite;

    unsigned char computeNvb(unsigned char collisionPos);
};

#endif // __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_MF1S503X_H__
