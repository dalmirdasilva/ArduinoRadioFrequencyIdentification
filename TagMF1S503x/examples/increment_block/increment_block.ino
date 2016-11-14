#include <ReaderMFRC522.h>
#include <RegisterBasedSPIDevice.h>
#include <MifareClassic.h>
#include <TagMF1S503x.h>

#define SS_PIN              10
#define RST_PIN             3
#define KEY_SIZE            6

#define ADDRESS             20
#define DELTA               1

RegisterBasedSPIDevice device(SS_PIN);
ReaderMFRC522 reader(&device, RST_PIN);
TagMF1S503x tag(&reader);

unsigned char keyB[KEY_SIZE] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

void setup() {
    Serial.begin(9600);
    Serial.println("Initializing...");
    reader.initialize();
    tag.setupAuthenticationKey(MifareClassic::KEY_B, keyB);
    Serial.println("Waiting card proximity...");
}

void loop() {
    if (tag.activate()) {
        Serial.println("Card detected.");
        if (tag.increment(ADDRESS, DELTA) && tag.transfer(ADDRESS)) {
            Serial.print("Value incremented successfully!");
        }
        tag.halt();
        delay(5000);
    }
}