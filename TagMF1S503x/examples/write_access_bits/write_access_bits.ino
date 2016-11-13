#include <ReaderMFRC522.h>
#include <RegisterBasedSPIDevice.h>
#include <TagMF1S503x.h>

#define SS_PIN              10
#define RST_PIN             3

#define KEY_SIZE            6
#define ACCESS_BITS_SIZE    4

#define SECTOR              1

RegisterBasedSPIDevice device(SS_PIN);
ReaderMFRC522 reader(&device, RST_PIN);
TagMF1S503x tag(&reader);

// Choose correct keys or you will lock the card.
unsigned char keyA[KEY_SIZE] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
unsigned char keyB[KEY_SIZE] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

// Check access bits integrity.
unsigned char accessBits[ACCESS_BITS_SIZE] = { 0xff, 0x07, 0x80, 0x68 };
unsigned char ok = 'n';

void setup() {
    Serial.begin(9600);
    Serial.println("Initializing...");
    reader.initialize();
    tag.setupAuthenticationKey(Tag::KEY_A, keyA);

    Serial.println("Are you sure you want to write access bits into the trailer sector?\n"
            "With wrong permission, it will lockout the block of the card forever!\n\n"
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
        Serial.println("Writing access bits...");
        if (tag.writeAccessBits(SECTOR, accessBits, keyA, keyB)) {
            Serial.println("Access bits wrote successfully!");
        } else {
            Serial.println("Error when writing access bits!");
        }
        tag.halt();
        delay(1000);
    }
}