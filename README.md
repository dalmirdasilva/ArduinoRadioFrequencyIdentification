# Arduino RFID Driver

[Documentation.pdf](Documentation.pdf)

## Pinout

```
Board	-> Ardiomp

SDA	-> 10 (SS_PIN defined in the sketch)
SCK	-> 13
MOSI	-> 11
MISO	-> 12
IRQ	-> 2 (int 0)
GND	-> GND
RST	-> 3 (RST_PIN defined in the sketch)
3.3v	-> 3.3v
```

## Dependency
This project depends on my other library called: ArduinoDevice (https://github.com/dalmirdasilva/ArduinoDevice). ArduinoDevice library implements the communication layer between the Arduino and the Reader. It also has different capabilities to handle different protocols - like I2C and SPI. 

First, you need to clone that repo, then go into its folder and execute ```make install```. Installing libraries (On Linux) is basically linking them into ```~/Arduino/libraries```. On Window you will have to copy and past the files to the correct destination - which I don't know! 

## Work plan

### Readers

- ReaderMFRC522

Fully implemented.

### Tags

- MifareClassic

Fully implemented.

- MifareUltralight

Not implemented yet.


## Examples

- ReaderMFR522 Self test.
``` c++
#include <Arduino.h>
#include <RegisterBasedSPIDevice.h>
#include <ReaderMFRC522.h>

#define SS_PIN      10
#define RST_PIN     3
#define LED_PIN     A1

RegisterBasedSPIDevice device(SS_PIN);
ReaderMFRC522 reader(&device, RST_PIN);

void setup() {
    Serial.begin(9600);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    Serial.println("Initializing...");

    reader.initialize();

    Serial.print("Version: ");
    switch (reader.getVersion()) {
    case ReaderMFRC522::CLONE:
        Serial.println("clone");
        break;
    case ReaderMFRC522::V0_0:
        Serial.println("v0_0");
        break;
    case ReaderMFRC522::V1_0:
        Serial.println("v1_0");
        break;
    case ReaderMFRC522::V2_0:
        Serial.println("v2_0");
        break;
    default:
        Serial.println("unknown");
        return;
    }

    Serial.print("Self test: ");
    if (reader.performSelfTest()) {
        digitalWrite(LED_PIN, HIGH);
        Serial.println("Passed!");
    } else {
        Serial.println("**failed**");
    }

    Serial.println("Done");
}

void loop() {
}
```


- ReaderMFRC522 FIFO overflow test.

``` c++
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
```

- ReaderMFRC522 blink led using timer and interruption.

``` c++
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
```

- MifareClassic tag detection.

``` c++
#include <RegisterBasedSPIDevice.h>
#include <ReaderMFRC522.h>
#include <MifareClassic.h>

#define SS_PIN      10
#define RST_PIN     3

RegisterBasedSPIDevice device(SS_PIN);
ReaderMFRC522 reader(&device, RST_PIN);
MifareClassic tag(&reader);

unsigned char keyA[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

void setup() {
    Serial.begin(9600);
    Serial.println("Initializing...");
    reader.initialize();
    tag.setupAuthenticationKey(MifareClassic::KEY_A, keyA);
    Serial.println("Waiting card proximity...");
}

void loop() {
    if (tag.request()) {
        Serial.println("Card detected.");
        delay(1000);
    }
}
```


- MifareClassic UID dump.

``` c++
#include <ReaderMFRC522.h>
#include <RegisterBasedSPIDevice.h>
#include <MifareClassic.h>

#define SS_PIN      10
#define RST_PIN     3

RegisterBasedSPIDevice device(SS_PIN);
ReaderMFRC522 reader(&device, RST_PIN);
MifareClassic tag(&reader);

unsigned char keyA[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

void setup() {
    Serial.begin(9600);
    Serial.println("Initializing...");
    reader.initialize();
    tag.setupAuthenticationKey(MifareClassic::KEY_A, keyA);
    Serial.println("Waiting card proximity...");
}

void loop() {
    if (tag.activate()) {
        Serial.print("Card detected.\nuid: ");
        Tag::Uid uid = tag.getUid();
        for (int i = 0; i < uid.size; i++) {
            Serial.print(uid.uid[i], HEX);
            Serial.print(" ");
        }
        Serial.println("\nDone.");
        tag.halt();
        delay(1000);
    }
}
```


- MifareClassic dump whole card.

``` c++
#include <ReaderMFRC522.h>
#include <RegisterBasedSPIDevice.h>
#include <MifareClassic.h>

#define SS_PIN      10
#define RST_PIN     3

RegisterBasedSPIDevice device(SS_PIN);
ReaderMFRC522 reader(&device, RST_PIN);
MifareClassic tag(&reader);

unsigned char keyA[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
unsigned char buffer[18];

void setup() {
    Serial.begin(9600);
    Serial.println("Initializing...");
    reader.initialize();
    tag.setupAuthenticationKey(MifareClassic::KEY_A, keyA);
    Serial.println("Waiting card proximity...");
}

void loop() {
    if (tag.activate()) {
        Serial.println("Card detected.");
        for (int address = 63; address >= 0; address--) {
            if ((address + 1) % 4 == 0) {
                Serial.print("\nSector: ");
                Serial.println(address / 4);
            }
            if (tag.readBlock(address, buffer)) {
                for (int j = 0; j < 16; j++) {
                    if (buffer[j] < 0x10) {
                        Serial.print("0");
                    }
                    Serial.print(buffer[j], HEX);
                    Serial.print(" ");
                }
                Serial.println();
            } else {
                Serial.println("Read error.");
            }
        }
        tag.halt();
        delay(1000);
    }
}
```


- MifareClassic read a key.

``` c++
#include <ReaderMFRC522.h>
#include <RegisterBasedSPIDevice.h>
#include <MifareClassic.h>

#define SS_PIN          10
#define RST_PIN         3

#define SECTOR          4
#define KEY_SIZE        6

RegisterBasedSPIDevice device(SS_PIN);
ReaderMFRC522 reader(&device, RST_PIN);
MifareClassic tag(&reader);

unsigned char keyA[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
unsigned char key[KEY_SIZE];

void setup() {
    Serial.begin(9600);
    Serial.println("Initializing...");
    reader.initialize();
    tag.setupAuthenticationKey(MifareClassic::KEY_A, keyA);
    Serial.println("Waiting card proximity...");
}

void loop() {
    if (tag.activate()) {
        Serial.println("Card detected.");
        Serial.println("Reading key...");
        if (tag.readKey(SECTOR, MifareClassic::KEY_B, key)) {
            for (int i = 0; i < KEY_SIZE; i++) {
                Serial.print(key[i], HEX);
                Serial.print(" ");
            }
        }
        Serial.println();
        tag.halt();
        delay(1000);
    }
}
```
