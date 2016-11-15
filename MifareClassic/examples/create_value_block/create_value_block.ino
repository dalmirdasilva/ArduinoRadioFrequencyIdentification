#include <ReaderMFRC522.h>
#include <RegisterBasedSPIDevice.h>
#include <MifareClassic.h>

#define SS_PIN              10
#define RST_PIN             3

#define KEY_SIZE            6

#define SECTOR              5
#define ADDRESS             20

RegisterBasedSPIDevice device(SS_PIN);
ReaderMFRC522 reader(&device, RST_PIN);
MifareClassic tag(&reader);

unsigned char keyA[KEY_SIZE] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
unsigned char keyB[KEY_SIZE] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

void setup() {
    Serial.begin(9600);
    Serial.println("Initializing...");
    reader.initialize();
    tag.setupAuthenticationKey(MifareClassic::KEY_A, keyA);
    tag.setSectorTrailerProtected(false);
    Serial.println("Waiting card proximity...");
}

void loop() {
    if (tag.activate()) {
        Serial.println("Card detected.");
        if (!tag.setAccessCondition(SECTOR, 3, MifareClassic::CONDITION_3, keyA, keyB)) {
            Serial.print("Cannot set access condition for sector trailer.");
            delay(1000);
            return;
        }
        tag.setupAuthenticationKey(MifareClassic::KEY_B, keyB);
        if (!tag.setAccessCondition(5, 0, MifareClassic::CONDITION_6, keyA, keyB)) {
            Serial.print("Cannot set access condition for block.");
            delay(1000);
            return;
        }
        if (tag.createValueBlock(ADDRESS, 0, 0)) {
            Serial.print("Value block created successfully!");
        }
        tag.halt();
        delay(5000);
    }
}