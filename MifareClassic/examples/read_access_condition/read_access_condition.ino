#include <ReaderMFRC522.h>
#include <RegisterBasedSPIDevice.h>
#include <MifareClassic.h>

#define SS_PIN              10
#define RST_PIN             3

#define KEY_SIZE            6

#define ADDRESS             3

RegisterBasedSPIDevice device(SS_PIN);
ReaderMFRC522 reader(&device, RST_PIN);
MifareClassic tag(&reader);

unsigned char keyA[KEY_SIZE] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

void setup() {
    Serial.begin(9600);
    Serial.println("Initializing...");
    reader.initialize();
    tag.setupAuthenticationKey(MifareClassic::KEY_A, keyA);
    Serial.println("Waiting card proximity...");
}

void loop() {
    if (tag.activate()) {
        MifareClassic::Access access;
        Serial.println("Card detected.");
        if (tag.getAccessCondition(ADDRESS, &access)) {
            Serial.print("Access conditions is: ");
            Serial.println(access, BIN);
        }
        tag.halt();
        delay(1000);
    }
}