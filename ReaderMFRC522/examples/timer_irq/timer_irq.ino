#include <Arduino.h>
#include <RegisterBasedSPIDevice.h>
#include <ReaderMFRC522.h>

#define SS_PIN      10
#define RST_PIN     3
#define LED_PIN     A1

RegisterBasedSPIDevice device(SS_PIN);
ReaderMFRC522 reader(&device, RST_PIN);

volatile bool irq_trig = false;

void irq() {
    irq_trig = true;
}

void setup() {
    Serial.begin(9600);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    Serial.println("Initializing...");

    reader.initialize();
    reader.stopTimer();

    // 1s timeout
    reader.configureTimer(0x66, 0xffff, false, true);
    reader.clearInterrupt(ReaderMFRC522::COM_TIMER_IRQ);
    reader.enableInterrupt(ReaderMFRC522::COM_TIMER_IRQ);

    attachInterrupt(0, irq, FALLING);
    reader.startTimer();

    Serial.println("Done.");
}

void loop() {
    if (irq_trig) {
        ReaderMFRC522::COM_IRQbits irq;
        irq.value = reader.readRegister(ReaderMFRC522::COM_IRQ);
        if (irq.TIMER_IRQ) {
            digitalWrite(LED_PIN, !digitalRead(LED_PIN));
            reader.clearInterrupt(ReaderMFRC522::COM_TIMER_IRQ);
        }
        irq_trig = false;
    }
}