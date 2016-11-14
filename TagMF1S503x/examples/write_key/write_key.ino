#include <ReaderMFRC522.h>
#include <RegisterBasedSPIDevice.h>
#include <MifareClassic.h>
#include <TagMF1S503x.h>

#define SS_PIN          10
#define RST_PIN         3

#define KEY_SIZE        6
#define SECTOR          4

RegisterBasedSPIDevice device(SS_PIN);
ReaderMFRC522 reader(&device, RST_PIN);
TagMF1S503x tag(&reader);

unsigned char keyA[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
unsigned char newKeyB[KEY_SIZE] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };
unsigned char ok = 'n';

void setup() {
    Serial.begin(9600);
    Serial.println("Initializing...");
    reader.initialize();
    tag.setupAuthenticationKey(MifareClassic::KEY_A, keyA);

    Serial.println("Are you sure you want to write the key?\n"
            "With wrong permission it will destroy the block of the card forever!\n\n"
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
        Serial.println("Writing key...");
        if (tag.writeKey(SECTOR, MifareClassic::KEY_B, keyA, newKeyB)) {
            Serial.println("Key wrote successfully!");
        } else {
            Serial.println("Error when writing key!");
        }
        tag.halt();
        delay(1000);
    }
}