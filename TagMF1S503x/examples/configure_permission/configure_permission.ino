#include <ReaderMFRC522.h>
#include <RegisterBasedSPIDevice.h>
#include <MifareClassic.h>
#include <TagMF1S503x.h>

#define SS_PIN          10
#define RST_PIN         3

#define KEY_SIZE        6
#define SECTOR          4
#define BLOCK           0

RegisterBasedSPIDevice device(SS_PIN);
ReaderMFRC522 reader(&device, RST_PIN);
TagMF1S503x tag(&reader);

unsigned char keyA[KEY_SIZE] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
unsigned char keyB[KEY_SIZE] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
unsigned char ok = 'n';

void setup() {
    Serial.begin(9600);
    Serial.println("Initializing...");
    reader.initialize();
    tag.setupAuthenticationKey(MifareClassic::KEY_A, keyA);
    Serial.println("Are you sure you want to write to the sector trailer?\n"
            "With wrong permission it will lockup the block of the card forever!\n\n"
            "Press y for yes, n for no.");
    while ((ok = Serial.read()) != 'y' && ok != 'n')
        ;
    if (ok == 'n') {
        Serial.println("Cancelled.");
    } else {
        Serial.println("Disabling sector trailer write protection...");
        tag.setSectorTrailerProtected(false);
        Serial.println("Waiting card proximity...");
    }
}

void loop() {
    if (ok == 'y' && tag.activate()) {
        Serial.println("Card detected.");
        Serial.println("Writing new Access condition...");
        if (tag.setAccessCondition(SECTOR, BLOCK, MifareClassic::CONDITION_7, keyA, keyB)) {
            Serial.println("Access condition wrote successfully!");
        } else {
            Serial.println("Error when writing key!");
        }
        tag.halt();
        delay(1000);
    }
}