#include "RadioFrequencyIdentificationMFRC522.h"
#include <Arduino.h>

RadioFrequencyIdentificationMFRC522::RadioFrequencyIdentificationMFRC522(unsigned char ssPin, unsigned char resetPin)
        : RadioFrequencyIdentification(), RegisterBasedSPIDevice(ssPin), resetPin(resetPin) {
    pinMode(resetPin, OUTPUT);
    digitalWrite(resetPin, LOW);
}

void RadioFrequencyIdentificationMFRC522::initialize() {
    digitalWrite(ssPin, LOW);
    if (digitalRead(resetPin) == LOW) {
        digitalWrite(resetPin, HIGH);
        delay(50);
    } else {
        softReset();
    }

    // TAuto=1; f(Timer) = 6.78MHz/TPreScaler
    writeRegister(T_MODE, 0x8d);

    // TModeReg[3..0] + TPrescalerReg
    writeRegister(T_PRESCALER, 0x3e);
    writeRegister(T_RELOAD_L, 0x1e);
    writeRegister(T_RELOAD_H, 0);

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
    configureRegisterBits(TX_CONTROL, TX_CONTROL_TX_RF_EN, 0xff);
}

void RadioFrequencyIdentificationMFRC522::setAntennaOff() {
    configureRegisterBits(TX_CONTROL, TX_CONTROL_TX_RF_EN, 0x00);
}
