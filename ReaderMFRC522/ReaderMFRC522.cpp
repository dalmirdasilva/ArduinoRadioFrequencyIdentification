#include <Arduino.h>
#include <ReaderMFRC522.h>

ReaderMFRC522::ReaderMFRC522(RegisterBasedDevice *device, unsigned char resetPin)
        : Reader(), device(device), resetPin(resetPin) {
    pinMode(resetPin, OUTPUT);
    digitalWrite(resetPin, LOW);
}

ReaderMFRC522::~ReaderMFRC522() {
}

inline void ReaderMFRC522::sendCommand(unsigned char command) {
    writeRegister(COMMAND, command);
}

void ReaderMFRC522::initialize() {
    if (digitalRead(resetPin) == LOW) {
        digitalWrite(resetPin, HIGH);
        delay(50);
    } else {
        softReset();
    }
    clearRegisterBits(AUTO_TEST, AUTO_TEST_ENABLE);

    // 100% ASK
    writeRegister(TX_ASK, 0x40);

    // CRC Initial value 0x6363
    writeRegister(MODE, 0x3d);

    // Open the antenna
    setAntennaOn();
}

void ReaderMFRC522::softReset() {
    sendCommand(SOFT_RESET);
}

void ReaderMFRC522::setAntennaOn() {
    setRegisterBits(TX_CONTROL, TX_CONTROL_TX_RF_EN);
}

void ReaderMFRC522::setAntennaOff() {
    clearRegisterBits(TX_CONTROL, TX_CONTROL_TX_RF_EN);
}

int ReaderMFRC522::readRegisterBlock(unsigned char reg, unsigned char *buf, unsigned char len) {

    // MSB == 1 is for reading. LSB is not used in address.
    return device->readRegisterBlock(((reg << 1) & 0x7e) | 0x80, buf, len);
}

int ReaderMFRC522::readRegisterBlock(unsigned char reg, unsigned char *buf, unsigned char len, unsigned char rxAlign) {
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

unsigned char ReaderMFRC522::writeRegisterBlock(unsigned char reg, unsigned char *buf, unsigned char len) {

    // MSB == 0 is for writing. LSB is not used in address.
    return device->writeRegisterBlock((reg << 1) & 0x7e, buf, len);
}

void ReaderMFRC522::configureTimer(unsigned int prescaler, unsigned int reload, bool autoStart, bool autoRestart) {
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

void ReaderMFRC522::startTimer() {
    setRegisterBits(CONTROL, CONTROL_T_START_NOW);
}

void ReaderMFRC522::stopTimer() {
    setRegisterBits(CONTROL, CONTROL_T_STOP_NOW);
}

void ReaderMFRC522::enableInterrupt(unsigned int interrupt) {
    setRegisterBits(MFR522_INT_TO_EN_REG(interrupt), MFR522_INT_TO_EN_MASK(interrupt));
}

void ReaderMFRC522::disableInterrupt(unsigned int interrupt) {
    clearRegisterBits(MFR522_INT_TO_EN_REG(interrupt), MFR522_INT_TO_EN_MASK(interrupt));
}

void ReaderMFRC522::clearInterrupt(unsigned int interrupt) {

    // 0x7f: first bit 0 indicates that the marked bits in the register are cleared
    configureRegisterBits(MFR522_INT_TO_IRQ_REG(interrupt), (MFR522_INT_TO_IRQ_MASK(interrupt)) | 0x80, 0x7f);
}

void ReaderMFRC522::flushQueue() {
    setRegisterBits(FIFO_LEVEL, FIFO_LEVEL_FLUSH_BUFFER);
}

void ReaderMFRC522::setWaterLevel(unsigned char level) {
    writeRegister(WATER_LEVEL, WATER_LEVEL_WATER_LEVEL & level);
}

int ReaderMFRC522::generateRandomId(unsigned char *buf) {

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

int ReaderMFRC522::tranceive(unsigned char *send, unsigned char *receive, unsigned char sendLen, bool checkCrc) {
    return communicate(TRANSCEIVE, send, receive, sendLen, checkCrc);
}

inline int ReaderMFRC522::tranceive(unsigned char *send, unsigned char *receive, unsigned char sendLen) {
    return tranceive(send, receive, sendLen, false);
}

int ReaderMFRC522::communicate(unsigned char command, unsigned char *send, unsigned char *receive, unsigned char sendLen, bool checkCrc) {

    unsigned char len = 0;
    COM_IRQbits irq;
    ERRORbits error;
    CONTROLbits control;

    lastError = NO_ERROR;

    // 25ms before timeout, auto start timer at the end of the transmission
    configureTimer(0xf9, 0x03e8, true, false);

    // Stop any active command.
    sendCommand(IDLE);

    // Clear all seven interrupt request bits
    clearInterrupt(COM_ALL_IRQ);

    // FlushBuffer = 1, FIFO initialization
    flushQueue();

    // Write sendData to the FIFO
    writeRegisterBlock(FIFO_DATA, send, sendLen);

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

    if (error.COLL_ERR) {
        lastError = COLLISION_ERROR;
        return -1;
    }

    if (error.BUFFER_OVFL || error.PARITY_ERR || error.PROTOCOL_ERR) {
        lastError = COMMUNICATION_ERROR;
        return -1;
    }

    len = readRegister(FIFO_LEVEL);
    control.value = readRegister(CONTROL);

    // Get received data from FIFO
    len = readRegisterBlock(FIFO_DATA, receive, len);

    // In this case a MIFARE Classic NAK is not OK.
    if (len == 1 && control.RX_LAST_BITS == 4 && (receive[0] != MFR522_SAK_BITS && receive[0] != MFR522_ACK_BITS)) {
        lastError = NACK;
        return -1;
    }

    // We need at least the CRC_A value and all 8 bits of the last byte must be received.
    // NOTE: casting (unsigned char) len is fine here, len > 0 and is less than FIFO size: 64
    // NOTE: control.RX_LAST_BITS = 0 means 8 bits.
    if (checkCrc && (len < 2 || control.RX_LAST_BITS != 0 || !hasValidCrc(receive, (unsigned char) len))) {
        lastError = CRC_ERROR;
        return -1;
    }

    return len;
}

inline int ReaderMFRC522::communicate(unsigned char command, unsigned char *send, unsigned char *receive, unsigned char sendLen) {
    return communicate(command, send, receive, sendLen, false);
}

int ReaderMFRC522::authenticate(unsigned char *send) {
    unsigned char receive;
    return communicate(MF_AUTHENT, send, &receive, 12);
}

void ReaderMFRC522::turnOffEncryption() {
    clearRegisterBits(STATUS2, STATUS2_MF_CRYPTO1_ON);
}

bool ReaderMFRC522::hasValidCrc(unsigned char *buf, unsigned char len) {
    if (len <= 2) {
        return false;
    }
    unsigned char crc[2];
    calculateCrc(buf, len - 2, crc);
    return (buf[len - 2] == crc[0]) && (buf[len - 1] == crc[1]);
}

unsigned int ReaderMFRC522::calculateCrc(unsigned char *buf, unsigned char len) {
    unsigned int dst;
    calculateCrc(buf, len, (unsigned char *) &dst);
    return dst;
}

void ReaderMFRC522::calculateCrc(unsigned char *buf, unsigned char len, unsigned char *dst) {

    // Stop any active command.
    sendCommand(IDLE);

    // Clear all seven interrupt request bits
    clearInterrupt(DIV_ALL_IRQ);

    // FlushBuffer = 1, FIFO initialization
    flushQueue();

    // Write sendData to the FIFO
    writeRegisterBlock(FIFO_DATA, buf, len);

    // Start the calculation
    sendCommand(CALC_CRC);

    // Wait for the CRC calculation to complete.
    waitForRegisterBits(DIV_IRQ, DIV_IRQ_CRC_IRQ);

    // Stop calculating CRC for new content in the FIFO.
    sendCommand(IDLE);

    if (dst != NULL) {
        dst[0] = readRegister(CRC_RESULT_LOW);
        dst[1] = readRegister(CRC_RESULT_HIGH);
    }
}

bool ReaderMFRC522::waitForRegisterBits(unsigned char reg, unsigned char mask, unsigned long timeout) {
    unsigned char v;
    unsigned long start = millis();
    do {
        v = readRegister(reg);
    } while (!(v & mask) && start + timeout > millis());
    return (v & mask) > 0;
}

inline bool ReaderMFRC522::waitForRegisterBits(unsigned char reg, unsigned char mask) {
    return waitForRegisterBits(reg, mask, READER_DEFAULT_TIMEOUT);
}

bool ReaderMFRC522::performSelfTest() {
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

void ReaderMFRC522::setBitFraming(unsigned char rxAlign, unsigned char txLastBits) {
    BIT_FRAMINGbits f;
    f.value = readRegister(BIT_FRAMING);
    f.RX_ALIGN = rxAlign;
    f.TX_LAST_BITS = txLastBits;
    writeRegister(BIT_FRAMING, f.value);
}

unsigned char ReaderMFRC522::getCollisionPosition() {
    COLLbits coll;
    coll.value = readRegister(COLL);
    return coll.COLL_POS > 0 ? coll.COLL_POS : 32;
}

void ReaderMFRC522::setuptForAnticollision() {
    clearRegisterBits(COLL, COLL_VALUES_AFTER_COLL);
}

ReaderMFRC522::Version ReaderMFRC522::getVersion() {
    return (Version) readRegister(VERSION);
}
