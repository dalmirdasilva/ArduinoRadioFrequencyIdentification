#include <Arduino.h>
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
    rfid.stopTimer();
    
    // 1s timeout
    rfid.configureTimer(0x66, 0xffff, false, true);
    rfid.clearInterrupt(RadioFrequencyIdentificationMFRC522::COM_TIMER_IRQ);
    rfid.enableInterrupt(RadioFrequencyIdentificationMFRC522::COM_TIMER_IRQ);

    attachInterrupt(0, irq, FALLING);
    rfid.startTimer();

    Serial.println("done");
}

void loop() {
    if (irq_trig) {
        RadioFrequencyIdentificationMFRC522::COM_IRQbits irq;
        irq.value = rfid.readRegister(RadioFrequencyIdentificationMFRC522::COM_IRQ);
        if (irq.TIMER_IRQ) {
            digitalWrite(LED_PIN, !digitalRead(LED_PIN));
            rfid.clearInterrupt(RadioFrequencyIdentificationMFRC522::COM_TIMER_IRQ);
        }
        irq_trig = false;
    }
}