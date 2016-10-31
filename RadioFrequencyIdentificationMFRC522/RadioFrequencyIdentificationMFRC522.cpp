#include "RadioFrequencyIdentificationMFRC522.h"
#include <Arduino.h>

RadioFrequencyIdentificationMFRC522::RadioFrequencyIdentificationMFRC522(
        RegisterBasedDevice *device, unsigned char resetPin)
        : RadioFrequencyIdentification(), device(device), resetPin(resetPin) {
    pinMode(resetPin, OUTPUT);
    digitalWrite(resetPin, LOW);
}

void RadioFrequencyIdentificationMFRC522::initialize() {
    if (digitalRead(resetPin) == LOW) {
        digitalWrite(resetPin, HIGH);
        delay(50);
    } else {
        softReset();
    }
    clearRegisterBits(AUTO_TEST, AUTO_TEST_ENABLE);

//    // TAuto=1; f(Timer) = 6.78MHz/TPreScaler
//    device->writeRegister(T_MODE, 0x8d);
//
//    // TModeReg[3..0] + TPrescalerReg
//    device->writeRegister(T_PRESCALER_LOW, 0x3e);
//    device->writeRegister(T_RELOAD_LOW, 0x1e);
//    device->writeRegister(T_RELOAD_HIGH, 0);

// 100% ASK
    writeRegister(TX_ASK, 0x40);

    // CRC Initial value 0x6363  ???
    writeRegister(MODE, 0x3d);

    // ClearBitMask(STATUS2, 0x08);       //MFCrypto1On=0
    // Write_AddicoreRFID(RxSelReg, 0x86);       //RxWait = RxSelReg[5..0]
    // Write_AddicoreRFID(RFCfgReg, 0x7f);           //RxGain = 48dB

    // Open the antenna
    setAntennaOn();
}

void RadioFrequencyIdentificationMFRC522::softReset() {
    writeRegister(COMMAND, SOFT_RESET);
}

void RadioFrequencyIdentificationMFRC522::setAntennaOn() {
    setRegisterBits(TX_CONTROL, TX_CONTROL_TX_RF_EN);
}

void RadioFrequencyIdentificationMFRC522::setAntennaOff() {
    clearRegisterBits(TX_CONTROL, TX_CONTROL_TX_RF_EN);
}

int RadioFrequencyIdentificationMFRC522::readRegisterBlock(unsigned char reg,
        unsigned char *buf, unsigned char len) {

    // MSB == 1 is for reading. LSB is not used in address.
    return device->readRegisterBlock(((reg << 1) & 0x7e) | 0x80, buf, len);
}

int RadioFrequencyIdentificationMFRC522::readRegisterBlock(unsigned char reg,
        unsigned char *buf, unsigned char len, unsigned char rxAlign) {
    int blockSize = readRegisterBlock(reg, buf, len);
    if (blockSize > 0 && rxAlign > 0) {
        rxAlign &= 0x07;
        unsigned char mask = 0;
        for (unsigned char i = rxAlign; i <= 7; i++) {
            mask |= (1 << i);
        }

        // Only bit positions rxAlign..7 in buf[0] are updated.
        buf[0] = (buf[0] & ~mask) | (buf[0] & mask);
    }
    return blockSize;
}

unsigned char RadioFrequencyIdentificationMFRC522::writeRegisterBlock(unsigned char reg,
        unsigned char *buf, unsigned char len) {

    // MSB == 0 is for writing. LSB is not used in address.
    return device->writeRegisterBlock((reg << 1) & 0x7e, buf, len);
}

void RadioFrequencyIdentificationMFRC522::configureTimer(unsigned int prescaler,
        unsigned int reload, bool autoStart, bool autoRestart) {
    T_MODEbits timerMode;
    timerMode.value = readRegister(T_MODE);
    timerMode.T_PRESCALER_HI = (prescaler >> 8) & 0x0f;
    timerMode.T_AUTO = autoStart;
    timerMode.T_GATED = (unsigned char) 0;
    timerMode.T_AUTO_RESTART = autoRestart;
    writeRegister(T_MODE, timerMode.value);
    writeRegister(T_PRESCALER_LOW, prescaler & 0xff);
    writeRegister(T_RELOAD_HIGH, (reload >> 8) & 0xff);
    writeRegister(T_RELOAD_LOW, reload & 0xff);
}

void RadioFrequencyIdentificationMFRC522::startTimer() {
    setRegisterBits(CONTROL, CONTROL_T_START_NOW);
}

void RadioFrequencyIdentificationMFRC522::stopTimer() {
    setRegisterBits(CONTROL, CONTROL_T_STOP_NOW);
}

void RadioFrequencyIdentificationMFRC522::enableInterrupt(Interrupt interrupt) {
    setRegisterBits(MFR522_INT_TO_EN_REG(interrupt), MFR522_INT_TO_EN_MASK(interrupt));
}

void RadioFrequencyIdentificationMFRC522::disableInterrupt(Interrupt interrupt) {
    clearRegisterBits(MFR522_INT_TO_EN_REG(interrupt), MFR522_INT_TO_EN_MASK(interrupt));
}

void RadioFrequencyIdentificationMFRC522::clearInterrupt(Interrupt interrupt) {

    // 0x7f: first bit 0 indicates that the marked bits in the register are cleared
    configureRegisterBits(MFR522_INT_TO_IRQ_REG(interrupt),
            (MFR522_INT_TO_IRQ_MASK(interrupt)) | 0x80, 0x7f);
}

void RadioFrequencyIdentificationMFRC522::flushQueue() {
    setRegisterBits(FIFO_LEVEL, FIFO_LEVEL_FLUSH_BUFFER);
}

void RadioFrequencyIdentificationMFRC522::setWaterLevel(unsigned char level) {
    writeRegister(WATER_LEVEL, WATER_LEVEL_WATER_LEVEL & level);
}

int RadioFrequencyIdentificationMFRC522::generateRandomId(unsigned char buf[10]) {

    // Stop any active command.
    sendCommand(IDLE);

    // Clear all seven interrupt request bits
    clearInterrupt(COM_ALL_IRQ);

    // FlushBuffer = 1, FIFO initialization
    flushQueue();

    // Send command
    sendCommand(GENERATE_RANDOM_ID);

    // Wait for command to complete.
    waitForRegisterBits(COM_IRQ, COM_IRQ_IDLE_IRQ);

    // FlushBuffer = 1, FIFO initialization
    flushQueue();

    // Transfers 25 bytes from the internal buffer to the FIFO buffer.
    sendCommand(MEM);

    // Wait for command to complete.
    waitForRegisterBits(COM_IRQ, COM_IRQ_IDLE_IRQ);
    sendCommand(IDLE);
    return readRegisterBlock(FIFO_DATA, buf, 10);
}

int RadioFrequencyIdentificationMFRC522::tranceiveData(unsigned char *output,
        unsigned char *input, unsigned char outputLen, bool checkCRC) {
    return communicate(TRANSCEIVE, output, input, outputLen, checkCRC);
}

int RadioFrequencyIdentificationMFRC522::communicate(Command command, unsigned char *output,
        unsigned char *input, unsigned char outputLen, bool checkCRC) {

    int len = 0;
    COM_IRQbits irq;
    BIT_FRAMINGbits frame;
    ERRORbits error;
    CONTROLbits control;

    lastError = NO_ERROR;

    // Stop any active command.
    sendCommand(IDLE);

    // Clear all seven interrupt request bits
    clearInterrupt(COM_ALL_IRQ);

    // FlushBuffer = 1, FIFO initialization
    flushQueue();

    // Write sendData to the FIFO
    writeRegisterBlock(FIFO_DATA, output, outputLen);

    // Execute the command
    sendCommand(command);

    if (command == TRANSCEIVE) {

        // StartSend=1, transmission of data starts
        setRegisterBits(BIT_FRAMING, BIT_FRAMING_START_SEND);
    }

    // Wait for the command to complete.
    // If timer was configured and T_AUTO flag is active in T_MODE register,
    // timer will start automatically after all data is transmitted.
    // See: configureTimer method
    do {
        irq.value = readRegister(COM_IRQ);

        // Timer interrupt - nothing received
        if (irq.TIMER_IRQ) {
            lastError = TIMEOUT_ERROR;
            return -1;
        }

    } while (!irq.IDLE_IRQ && !irq.RX_IRQ);

    // Stop now if any errors except collisions were detected.
    // ErrorReg[7..0] bits are: WrErr TempErr reserved BufferOvfl CollErr CRCErr ParityErr ProtocolErr
    error.value = readRegister(ERROR);
    if (error.BUFFER_OVFL || error.PARITY_ERR || error.PROTOCOL_ERR || error.COLL_ERR) {
        lastError = COMMUNICATION_ERROR;
        return -1;
    }

    // If the caller wants data back, get it from the MFRC522.
    if (input != NULL) {

        // Get received data from FIFO
        len = readRegisterBlock(FIFO_DATA, input, readRegister(FIFO_LEVEL));

        // Perform CRC_A validation if requested.
        if (len > 0 && checkCRC) {

            control.value = readRegister(CONTROL);

            // In this case a MIFARE Classic NAK is not OK.
            if (len == 1 && control.RX_LAST_BITS == 4) {
                lastError = MIFARE_NACK;
                return -1;
            }

            // We need at least the CRC_A value and all 8 bits of the last byte must be received.
            // NOTE: (unsigned char) len is fine here, len > 0 and is less than FIFO size: 64
            // NOTE: control.RX_LAST_BITS = 0 means 8 bits.
            if (len < 2 || control.RX_LAST_BITS != 0
                    || !hasValisCRC(input, (unsigned char) len)) {
                lastError = CRC_ERROR;
                return -1;
            }
        }
    }
    return len;
}

bool RadioFrequencyIdentificationMFRC522::hasValisCRC(unsigned char *buf,
        unsigned char len) {
    unsigned int expectedCRC, returnedCRC;

    // Verify CRC_A - do our own calculation and store the control in controlBuffer.
    expectedCRC = calculateCRC(buf, len - 2);
    returnedCRC = buf[len - 1];
    returnedCRC <<= 8;
    returnedCRC |= buf[len - 2];
    return expectedCRC == returnedCRC;
}

unsigned int RadioFrequencyIdentificationMFRC522::calculateCRC(unsigned char *buff,
        unsigned char len) {

    unsigned int crc = 0;

    // Stop any active command.
    sendCommand(IDLE);

    // Clear all seven interrupt request bits
    clearInterrupt(DIV_ALL_IRQ);

    // FlushBuffer = 1, FIFO initialization
    flushQueue();

    // Write sendData to the FIFO
    writeRegisterBlock(FIFO_DATA, buff, len);

    // Start the calculation
    sendCommand(CALC_CRC);

    // Wait for the CRC calculation to complete.
    waitForRegisterBits(DIV_IRQ, DIV_IRQ_CRC_IRQ);

    // Stop calculating CRC for new content in the FIFO.
    sendCommand(IDLE);

    crc = readRegister(CRC_RESULT_HIDH);
    crc <<= 8;
    crc |= readRegister(CRC_RESULT_LOW) & 0xff;
    return crc;
}

bool RadioFrequencyIdentificationMFRC522::waitForRegisterBits(unsigned char reg,
        unsigned char mask, unsigned long timeout) {
    unsigned char v;
    unsigned long start = millis();
    do {
        v = readRegister(reg);
    } while (!(v & mask) && start + timeout > millis());
    return (v & mask) > 0;
}

bool RadioFrequencyIdentificationMFRC522::performSelfTest() {
    unsigned char *firmwareReference;
    unsigned char buffer[64] = { 0 };
    writeRegister(AUTO_TEST, 0x00);
    softReset();
    flushQueue();
    writeRegisterBlock(FIFO_DATA, buffer, 25);
    sendCommand(MEM);
    writeRegister(AUTO_TEST, AUTO_TEST_ENABLE);
    writeRegister(FIFO_DATA, 0x00);
    sendCommand(CALC_CRC);
    waitForRegisterBits(DIV_IRQ, DIV_IRQ_CRC_IRQ, 100);
    readRegisterBlock(FIFO_DATA, buffer, 64);
    switch (getVersion()) {
    case CLONE:
        firmwareReference = (unsigned char *) FM17522_FIRMWARE_REFERENCE;
        break;
    case V0_0:
        firmwareReference = (unsigned char *) MFRC522_FIRMWARE_REFERENCE_V0_0;
        break;
    case V1_0:
        firmwareReference = (unsigned char *) MFRC522_FIRMWARE_REFERENCE_V1_0;
        break;
    case V2_0:
        firmwareReference = (unsigned char *) MFRC522_FIRMWARE_REFERENCE_V2_0;
        break;
    default:
        return false;
    }
    for (unsigned char i = 0; i < 64; i++) {
        if (buffer[i] != pgm_read_byte(&(firmwareReference[i]))) {
            return false;
        }
    }
    return true;
}

void RadioFrequencyIdentificationMFRC522::setBitFraming(unsigned char rxAlign,
        unsigned char txLastBits) {
    BIT_FRAMINGbits f;
    f.value = readRegister(BIT_FRAMING);
    f.RX_ALIGN = rxAlign;
    f.TX_LAST_BITS = txLastBits;
    writeRegister(BIT_FRAMING, f.value);
}

bool RadioFrequencyIdentificationMFRC522::sendRequestTypeA() {
    unsigned char output = MIFARE_REQUEST;
    setBitFraming(0, 0x07);

    //  25ms before timeout, auto start timer at the end of the transmission
    configureTimer(0xa9, 0x03e8, true, false);
    tranceiveData(&output, NULL, 1);
    return lastError == NO_ERROR;
}

bool RadioFrequencyIdentificationMFRC522::sendWakeUp() {
    unsigned char output = MIFARE_WAKE_UP;
    setBitFraming(0, 0x07);

    //  25ms before timeout, auto start timer at the end of the transmission
    configureTimer(0xa9, 0x03e8, true, false);
    tranceiveData(&output, NULL, 1);
    return lastError == NO_ERROR;
}

bool RadioFrequencyIdentificationMFRC522::sendHalt() {
    unsigned char buf[4] = { MIFARE_HLT_A, 0, 0, 0 };

    // Calculate CRC_A
    unsigned int crc = calculateCRC(buf, 2);
    buf[2] = (crc >> 8) & 0xff;
    buf[3] = crc & 0xff;

    // If the PICC responds with any modulation during a period of 1 ms after the end of the frame containing the
    // HLTA command, this response shall be interpreted as 'not acknowledge'.
    tranceiveData(buf, NULL, 4);
    return lastError != NO_ERROR;
}

RadioFrequencyIdentificationMFRC522::Version RadioFrequencyIdentificationMFRC522::getVersion() {
    return (Version) readRegister(VERSION);
}
