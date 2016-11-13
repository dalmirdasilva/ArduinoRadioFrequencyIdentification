/**
 * Arduino - Radio Frequency Identification
 *
 * @author Dalmir da Silva <dalmirdasilva@gmail.com>
 */

#ifndef __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_READER_H__
#define __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_READER_H__ 1

#define READER_DEFAULT_TIMEOUT          300

class Reader {

public:

    enum Error {
        NO_ERROR = 0x00,
        GENERAL_ERROR = 0x01,
        TIMEOUT_ERROR = 0x02,
        COMMUNICATION_ERROR = 0x03,
        CRC_ERROR = 0x04,
        NACK = 0x05,
        COLLISION_ERROR = 0x06
    };

    Reader();

    virtual ~Reader();

    virtual inline void sendCommand(unsigned char command) = 0;

    virtual void softReset() = 0;

    virtual void setAntennaOn() = 0;

    virtual void setAntennaOff() = 0;

    virtual void configureTimer(unsigned int prescaler, unsigned int reload, bool autoStart, bool autoRestart) = 0;

    virtual void startTimer() = 0;

    virtual void stopTimer() = 0;

    virtual void enableInterrupt(unsigned int interrupt) = 0;

    virtual void disableInterrupt(unsigned int interrupt) = 0;

    virtual void clearInterrupt(unsigned int interrupt) = 0;

    virtual void flushQueue() = 0;

    virtual void setWaterLevel(unsigned char level) = 0;

    virtual int generateRandomId(unsigned char *buf) = 0;

    virtual int communicate(unsigned char command, unsigned char *send, unsigned char *receive, unsigned char sendLen, bool checkCrc) = 0;

    virtual int communicate(unsigned char command, unsigned char *send, unsigned char *receive, unsigned char sendLen) = 0;

    virtual int tranceive(unsigned char *send, unsigned char *receive, unsigned char sendLen, bool checkCrc) = 0;

    virtual int tranceive(unsigned char *send, unsigned char *receive, unsigned char sendLen) = 0;

    virtual int authenticate(unsigned char *send) = 0;

    virtual void stopCrypto() = 0;

    virtual unsigned int calculateCrc(unsigned char *buf, unsigned char len) = 0;

    virtual void calculateCrc(unsigned char *buf, unsigned char len, unsigned char *dst) = 0;

    virtual bool waitForRegisterBits(unsigned char reg, unsigned char mask, unsigned long timeout) = 0;

    virtual bool waitForRegisterBits(unsigned char reg, unsigned char mask) = 0;

    virtual bool performSelfTest() = 0;

    virtual void setBitFraming(unsigned char rxAlign, unsigned char txLastBits) = 0;

    virtual unsigned char getCollisionPosition() = 0;

    virtual void setuptForAnticollision() = 0;

    unsigned char getLastError();

    void clearLastError();

    virtual bool hasValidCrc(unsigned char *buf, unsigned char len) = 0;

protected:

    Error lastError;
};

#endif // __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_READER_H__
