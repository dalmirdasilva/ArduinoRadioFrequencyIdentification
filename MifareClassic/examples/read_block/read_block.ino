#include <ReaderMFRC522.h>
#include <RegisterBasedSPIDevice.h>
#include <MifareClassic.h>

#define SS_PIN      10
#define RST_PIN     3
#define ADDRESS     0

RegisterBasedSPIDevice device(SS_PIN);
ReaderMFRC522 reader(&device, RST_PIN);
MifareClassic tag(&reader);

unsigned char keyA[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
unsigned char buf[18] = { 0 };

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
        if (tag.readBlock(ADDRESS, buf)) {
            Serial.println("Successfully read.");
            for (int i = 0; i < 16; i++) {
                Serial.print(buf[i], HEX);
                Serial.print(" ");
            }
        }
        Serial.println();
        tag.halt();
        delay(1000);
    }
}
