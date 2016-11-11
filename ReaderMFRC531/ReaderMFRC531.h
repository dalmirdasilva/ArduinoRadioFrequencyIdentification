/**
 * Arduino - Radio Frequency Identification MFRC531
 *
 * @author Dalmir da Silva <dalmirdasilva@gmail.com>
 */
#ifndef __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_READER_MFRC531_H__
#define __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_READER_MFRC531_H__ 1

#include <RegisterBasedDevice.h>
#include <Arduino.h>
#include <Reader.h>

/**
 *
 *
 * NOT IMPLEMENTED
 *
 *
 */
class ReaderMFRC531: public Reader, public RegisterBasedDevice {

public:

    enum Register {
    };

    enum Command {
    };

    enum Mask {
    };

    enum Interrupt
        : unsigned int {
    };

    enum Version {
    };

    ReaderMFRC531(RegisterBasedDevice *device, unsigned char resetPin);

    virtual ~ReaderMFRC531();

    void initialize();

    inline void sendCommand(unsigned char command);

    void softReset();

    void setAntennaOn();

    void setAntennaOff();

    void configureTimer(unsigned int prescaler, unsigned int reload, bool autoStart, bool autoRestart);

    void startTimer();

    void stopTimer();

    void enableInterrupt(unsigned int interrupt);

    void disableInterrupt(unsigned int interrupt);

    void clearInterrupt(unsigned int interrupt);

    void flushQueue();

    void setWaterLevel(unsigned char level);

    int generateRandomId(unsigned char *buf);

    int communicate(unsigned char command, unsigned char *send, unsigned char *receive, unsigned char sendLen, bool checkCrc);

    inline int communicate(unsigned char command, unsigned char *send, unsigned char *receive, unsigned char sendLen);

    int tranceive(unsigned char *send, unsigned char *receive, unsigned char sendLen, bool checkCrc);

    inline int tranceive(unsigned char *send, unsigned char *receive, unsigned char sendLen);

    int authenticate(unsigned char *send);

    unsigned int calculateCrc(unsigned char *buf, unsigned char len);

    void calculateCrc(unsigned char *buf, unsigned char len, unsigned char *dst);

    bool waitForRegisterBits(unsigned char reg, unsigned char mask, unsigned long timeout);

    inline bool waitForRegisterBits(unsigned char reg, unsigned char mask);

    Version getVersion();

    bool performSelfTest();

    void setBitFraming(unsigned char rxAlign, unsigned char txLastBits);

    unsigned char getCollisionPosition();

    void setuptForAnticollision();

    int readRegisterBlock(unsigned char reg, unsigned char *buf, unsigned char len);

    int readRegisterBlock(unsigned char reg, unsigned char *buf, unsigned char len, unsigned char rxAlign);

    unsigned char writeRegisterBlock(unsigned char reg, unsigned char *buf, unsigned char len);

    void turnOffEncryption();

    bool hasValidCrc(unsigned char *buf, unsigned char len);

};

#endif // __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_READER_MFRC531_H__
