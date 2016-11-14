#include <ReaderMFRC522.h>
#include <RegisterBasedSPIDevice.h>
#include <MifareClassic.h>
#include <TagMF1S503x.h>

#define SS_PIN      10
#define RST_PIN     3
#define BLOCK       2

RegisterBasedSPIDevice device(SS_PIN);
ReaderMFRC522 reader(&device, RST_PIN);
TagMF1S503x tag(&reader);

unsigned char keyA[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
unsigned char writeBuf[18] = { 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };
unsigned char readBuf[18] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

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
        if (tag.writeBlock(BLOCK, writeBuf)) {
            Serial.println("Write operation successfully performed to the block.");
        }
        if (tag.readBlock(BLOCK, readBuf)) {
            Serial.println("Read operation successfully performed from the block.");
            for (int i = 0; i < 16; i++) {
                Serial.print(readBuf[i], HEX);
                Serial.print(" ");
            }
        }
        Serial.println();
        tag.halt();
        delay(1000);
    }
}
