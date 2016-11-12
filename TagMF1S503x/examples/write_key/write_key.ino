#include <ReaderMFRC522.h>
#include <RegisterBasedSPIDevice.h>
#include <TagMF1S503x.h>

#define SS_PIN          10
#define RST_PIN         3

#define KEY_SIZE        6
#define SECTOR          1

RegisterBasedSPIDevice device(SS_PIN);
ReaderMFRC522 reader(&device, RST_PIN);
TagMF1S503x tag(&reader);

unsigned char keyA[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
unsigned char newKey[KEY_SIZE] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };
unsigned char ok = 'n';

void setup() {
    Serial.begin(9600);
    Serial.println("Initialing...");
    reader.initialize();
    tag.setupAuthenticationKey(Tag::KEY_A, keyA);
    
    Serial.println("Are you sure you want to write the key?\n"
            "With wrong permission it will destroy the block of the card forever!\n\n"
            "Press y for yes, n do no.");
    while ((ok = Serial.read()) != 'y' && ok != 'n')
        ;
    if (ok == 'n') {
        Serial.println("Cancelled.");
    } else {
        Serial.println("Waiting card proximity...");
    }
}

void loop() {
    if (ok == 'y' && tag.activate()) {
        Serial.println("Card detected.");
        Serial.println("Writing key...");
        if (tag.writeKey(SECTOR, Tag::KEY_B, newKey)) {
            Serial.println("Key wrote successfuly!");
        } else {
            Serial.println("Error when writing key!");
        }
        tag.halt();
        delay(1000);
    }
}
