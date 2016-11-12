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
    reader.flushQueue();
    reader.setWaterLevel(0x05);
    reader.disableInterrupt(ReaderMFRC522::COM_ALL_IRQ);
    reader.enableInterrupt(ReaderMFRC522::COM_HI_ALERT_IRQ);
    reader.clearInterrupt(ReaderMFRC522::COM_HI_ALERT_IRQ);
    attachInterrupt(0, irq, FALLING);

    Serial.println(reader.readRegister(ReaderMFRC522::COM_IRQ), BIN);
    Serial.println(reader.readRegister(ReaderMFRC522::FIFO_LEVEL), DEC);
    Serial.println(reader.readRegister(ReaderMFRC522::COM_IRQ), BIN);
    for (int i = 0; i < 50; i++) {
        reader.writeRegister(ReaderMFRC522::FIFO_DATA, i);
    }
    Serial.println(reader.readRegister(ReaderMFRC522::COM_IRQ), BIN);
    Serial.println(reader.readRegister(ReaderMFRC522::FIFO_LEVEL), DEC);

    Serial.println("Keep sending y to the serial until FIFO gets full and a irq is triggered...");
}

void loop() {
    if (irq_trig) {
        ReaderMFRC522::COM_IRQbits irq;
        irq.value = reader.readRegister(ReaderMFRC522::COM_IRQ);
        if (irq.HI_ALERT_IRQ) {
            digitalWrite(LED_PIN, HIGH);
            reader.clearInterrupt(ReaderMFRC522::COM_HI_ALERT_IRQ);
        }
        irq_trig = false;
    }
    while (Serial.available() > 0 && Serial.read() == 'y') {
        reader.writeRegister(ReaderMFRC522::FIFO_DATA, 0xff);
        Serial.print("COM_IRQ reg: ");
        Serial.println(reader.readRegister(ReaderMFRC522::COM_IRQ), BIN);
        Serial.print("FIFO level: ");
        Serial.println(reader.readRegister(ReaderMFRC522::FIFO_LEVEL), DEC);
    }
}