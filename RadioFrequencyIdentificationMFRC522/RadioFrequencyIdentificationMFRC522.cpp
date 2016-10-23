#include "RadioFrequencyIdentificationMFRC522.h"
#include <Arduino.h>

RadioFrequencyIdentificationMFRC522::RadioFrequencyIdentificationMFRC522(RegisterBasedDevice *device, unsigned char resetPin)
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

//    // TAuto=1; f(Timer) = 6.78MHz/TPreScaler
//    device->writeRegister(T_MODE, 0x8d);
//
//    // TModeReg[3..0] + TPrescalerReg
//    device->writeRegister(T_PRESCALER_LOW, 0x3e);
//    device->writeRegister(T_RELOAD_LOW, 0x1e);
//    device->writeRegister(T_RELOAD_HIGH, 0);

// 100% ASK
    device->writeRegister(TX_ASK, 0x40);

    // CRC Initial value 0x6363  ???
    device->writeRegister(MODE, 0x3d);

    // ClearBitMask(STATUS2, 0x08);       //MFCrypto1On=0
    // Write_AddicoreRFID(RxSelReg, 0x86);       //RxWait = RxSelReg[5..0]
    // Write_AddicoreRFID(RFCfgReg, 0x7f);           //RxGain = 48dB

    // Open the antenna
    setAntennaOn();
}

void RadioFrequencyIdentificationMFRC522::softReset() {
    device->writeRegister(COMMAND, SOFT_RESET);
}

void RadioFrequencyIdentificationMFRC522::setAntennaOn() {
    device->configureRegisterBits(TX_CONTROL, TX_CONTROL_TX_RF_EN, 0xff);
}

void RadioFrequencyIdentificationMFRC522::setAntennaOff() {
    device->configureRegisterBits(TX_CONTROL, TX_CONTROL_TX_RF_EN, 0x00);
}

int RadioFrequencyIdentificationMFRC522::readRegisterBlock(unsigned char reg, unsigned char *buf, unsigned char len) {

    // MSB == 1 is for reading. LSB is not used in address.
    return device->readRegisterBlock(((reg << 1) & 0x7e) | 0x80, buf, len);
}

unsigned char RadioFrequencyIdentificationMFRC522::writeRegisterBlock(unsigned char reg, unsigned char *buf, unsigned char len) {

    // MSB == 0 is for writing. LSB is not used in address.
    return device->writeRegisterBlock((reg << 1) & 0x7e, buf, len);
}

void RadioFrequencyIdentificationMFRC522::configureTimer(unsigned int prescaler, unsigned int reload, bool autoStart, bool autoRestart) {
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
    configureRegisterBits(CONTROL, CONTROL_T_START_NOW, 0xff);
}

void RadioFrequencyIdentificationMFRC522::stopTimer() {
    configureRegisterBits(CONTROL, CONTROL_T_STOP_NOW, 0xff);
}

void RadioFrequencyIdentificationMFRC522::enableInterrupt(Interrupt interrupt) {
    bool div = interrupt > COM_ALL_IRQ;
    unsigned char reg = COM_I_EN, mask = COM_I_EN_INTERRUPT_EN, value = interrupt & 0xff;
    if (div) {
        reg = DIV_I_EN;
        mask = DIV_I_EN_INTERRUPT_EN;
        value = (interrupt >> 8) & 0xff;
    }
    configureRegisterBits(reg, mask, value);
}

void RadioFrequencyIdentificationMFRC522::clearInterrupt(Interrupt interrupt) {
    bool div = interrupt > COM_ALL_IRQ;
    unsigned char reg = COM_IRQ, mask = COM_IRQ_ALL_IRQ, value = interrupt & 0xff;
    if (div) {
        reg = DIV_IRQ;
        mask = DIV_IRQ_ALL_IRQ;
        value = (interrupt >> 8) & 0xff;
    }
    configureRegisterBits(reg, mask, value);
}

void RadioFrequencyIdentificationMFRC522::flushQueue() {
    configureRegisterBits(FIFO_LEVEL, FIFO_LEVEL_FLUSH_BUFFER, 0xff);
}

void RadioFrequencyIdentificationMFRC522::setWaterLevel(unsigned char level) {
    writeRegister(WATER_LEVEL, WATER_LEVEL_WATER_LEVEL & level);
}

int RadioFrequencyIdentificationMFRC522::generateRandomId(unsigned char buf[10]) {

    clearInterrupt(COM_IDLE_IRQ);

    COM_IRQbits irq;

    // Stop any active command.
    sendCommand(IDLE);

    // Clear all seven interrupt request bits
    clearInterrupt(COM_ALL_IRQ);

    // FlushBuffer = 1, FIFO initialization
    flushQueue();

    // Send command
    sendCommand(GENERATE_RANDOM_ID);

    // Wait for command to complete.
    do {
        irq.value = readRegister(COM_IRQ);
    } while (!irq.IDLE_IRQ);

    Serial.println(readRegister(FIFO_LEVEL), HEX);

    clearInterrupt(COM_IDLE_IRQ);

    // FlushBuffer = 1, FIFO initialization
    flushQueue();
    Serial.print("readRegister(FIFO_LEVEL): ");
    Serial.println(readRegister(FIFO_LEVEL), HEX);

    // Transfers 25 bytes from the internal buffer to the FIFO buffer.
    sendCommand(MEM);
    Serial.println(readRegister(FIFO_LEVEL), HEX);

    // Wait for command to complete.
    do {
        irq.value = readRegister(COM_IRQ);
    } while (!irq.IDLE_IRQ);

    Serial.print("last: ");
    Serial.println(readRegister(FIFO_DATA), HEX);

    sendCommand(IDLE);

    return readRegisterBlock(FIFO_DATA, buf, 10);
}

int RadioFrequencyIdentificationMFRC522::executeCommand(Command command, unsigned char *output, unsigned char *input, unsigned char outputLen, bool checkCRC) {

    int len = 0;
    COM_IRQbits irq;

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
    // In PCD_Init() we set the TAuto flag in TModeReg. This means the timer automatically starts when the PCD stops transmitting.
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
    ERRORbits error;
    error.value = readRegister(ERROR);
    if (error.BUFFER_OVFL || error.PARITY_ERR || error.PROTOCOL_ERR || error.COLL_ERR) {
        lastError = COMMUNICATION_ERROR;
        return -1;
    }

    // If the caller wants data back, get it from the MFRC522.
    if (input != NULL) {

        // Number of bytes in the FIFO
        int inpulLen = readRegister(FIFO_LEVEL);

        // Get received data from FIFO
        len = readRegisterBlock(FIFO_DATA, input, inpulLen);

        // Perform CRC_A validation if requested.
        if (len > 0 && checkCRC) {

            // Verify CRC_A - do our own calculation and store the control in controlBuffer.
            unsigned int expectedCRC = calculateCRC(input, inpulLen - 2);
            unsigned int returnedCRC = input[inpulLen - 1];
            returnedCRC <<= 8;
            returnedCRC |= input[inpulLen - 2] & 0xff;
            if (expectedCRC != returnedCRC) {
                lastError = CRC_ERROR;
                return -1;
            }
        }
    }
    return len;
}

unsigned int RadioFrequencyIdentificationMFRC522::calculateCRC(unsigned char *buff, unsigned char len) {

    unsigned int crc = 0;
    DIV_IRQbits irq;

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
    do {

        // DivIrqReg[7..0] bits are: Set2 reserved reserved MfinActIRq reserved CRCIRq reserved reserved
        irq.value = readRegister(DIV_IRQ);
    } while (!irq.CRC_IRQ);

    // Stop calculating CRC for new content in the FIFO.
    sendCommand(IDLE);

    crc = readRegister(CRC_RESULT_HIDH);
    crc <<= 8;
    crc |= readRegister(CRC_RESULT_LOW) & 0xff;
    return crc;
}
