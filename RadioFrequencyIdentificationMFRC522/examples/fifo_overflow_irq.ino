#include <Arduino.h>
#include <RegisterBasedSPIDevice.h>
#include <RadioFrequencyIdentificationMFRC522.h>

#define SS_PIN      10
#define RST_PIN     3

RegisterBasedSPIDevice device(SS_PIN);
RadioFrequencyIdentificationMFRC522 rfid(&device, RST_PIN);

volatile bool irq_trig = false;

void irq() {
    irq_trig = true;
}

void setup() {
    Serial.begin(9600);
    Serial.println("init");

    pinMode(A0, OUTPUT);
    digitalWrite(A0, LOW);
    
    rfid.initialize();
    rfid.flushQueue();
    rfid.setWaterLevel(0x05);
    rfid.clearInterrupt(RadioFrequencyIdentificationMFRC522::HI_ALERT_IRQ);
    rfid.enableInterrupt(RadioFrequencyIdentificationMFRC522::HI_ALERT_IRQ);
    attachInterrupt(0, irq, RISING);
    
    Serial.println(rfid.readRegister(RadioFrequencyIdentificationMFRC522::FIFO_LEVEL), DEC);
    Serial.println(rfid.readRegister(RadioFrequencyIdentificationMFRC522::COM_IRQ), HEX);
    for (int i = 0; i < 50; i++) {
        rfid.writeRegister(RadioFrequencyIdentificationMFRC522::FIFO_DATA, i);
    }
    Serial.println(rfid.readRegister(RadioFrequencyIdentificationMFRC522::COM_IRQ), HEX);
    Serial.println(rfid.readRegister(RadioFrequencyIdentificationMFRC522::FIFO_LEVEL), DEC);

    Serial.println("done");
}

void loop() {

    if (irq_trig) {
        RadioFrequencyIdentificationMFRC522::COM_IRQbits irq;
        irq.value = rfid.readRegister(RadioFrequencyIdentificationMFRC522::COM_IRQ);
        if (irq.HI_ALERT_IRQ) {
            digitalWrite(A0, HIGH);
            rfid.clearInterrupt(RadioFrequencyIdentificationMFRC522::HI_ALERT_IRQ);
        }
        irq_trig = false;
    }

    while (Serial.available() > 0 && Serial.read() == 'y') {
        rfid.writeRegister(RadioFrequencyIdentificationMFRC522::FIFO_DATA, 0xff);
    }
}