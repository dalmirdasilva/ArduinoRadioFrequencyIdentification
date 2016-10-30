#include <RegisterBasedSPIDevice.h>
#include <RadioFrequencyIdentificationMFRC522.h>

#define SS_PIN      10
#define RST_PIN     3
#define LED_PIN     A1

RegisterBasedSPIDevice device(SS_PIN);
RadioFrequencyIdentificationMFRC522 rfid(&device, RST_PIN);

volatile bool irq_trig = false;

void irq() {
    irq_trig = true;
}

void setup() {
    Serial.begin(9600);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    Serial.println("initializing");
    
    rfid.initialize();
    rfid.flushQueue();
    rfid.setWaterLevel(0x05);
    rfid.disableInterrupt(RadioFrequencyIdentificationMFRC522::COM_ALL_IRQ);
    rfid.enableInterrupt(RadioFrequencyIdentificationMFRC522::COM_HI_ALERT_IRQ);
    rfid.clearInterrupt(RadioFrequencyIdentificationMFRC522::COM_HI_ALERT_IRQ);
    attachInterrupt(0, irq, FALLING);
    
    Serial.println(rfid.readRegister(RadioFrequencyIdentificationMFRC522::COM_IRQ), BIN);
    Serial.println(rfid.readRegister(RadioFrequencyIdentificationMFRC522::FIFO_LEVEL), DEC);
    Serial.println(rfid.readRegister(RadioFrequencyIdentificationMFRC522::COM_IRQ), BIN);
    for (int i = 0; i < 50; i++) {
        rfid.writeRegister(RadioFrequencyIdentificationMFRC522::FIFO_DATA, i);
    }
    Serial.println(rfid.readRegister(RadioFrequencyIdentificationMFRC522::COM_IRQ), BIN);
    Serial.println(rfid.readRegister(RadioFrequencyIdentificationMFRC522::FIFO_LEVEL), DEC);

    Serial.println("keep sending y to the serial until FIFO gets full and a irq is triggered...");
}

void loop() {
    if (irq_trig) {
        RadioFrequencyIdentificationMFRC522::COM_IRQbits irq;
        irq.value = rfid.readRegister(RadioFrequencyIdentificationMFRC522::COM_IRQ);
        if (irq.HI_ALERT_IRQ) {
            digitalWrite(LED_PIN, HIGH);
            rfid.clearInterrupt(RadioFrequencyIdentificationMFRC522::COM_HI_ALERT_IRQ);
        }
        irq_trig = false;
    }
    while (Serial.available() > 0 && Serial.read() == 'y') {
        rfid.writeRegister(RadioFrequencyIdentificationMFRC522::FIFO_DATA, 0xff);
        Serial.println(rfid.readRegister(RadioFrequencyIdentificationMFRC522::COM_IRQ), BIN);
        Serial.println(rfid.readRegister(RadioFrequencyIdentificationMFRC522::FIFO_LEVEL), DEC);
    }
}