#include <ReaderMFRC522.h>
#include <RegisterBasedSPIDevice.h>
#include <MifareClassic.h>

#define SS_PIN          10
#define RST_PIN         3
#define SLICE_LEN       4
#define ADDRESS         1
#define FROM            0

RegisterBasedSPIDevice device(SS_PIN);
ReaderMFRC522 reader(&device, RST_PIN);
MifareClassic tag(&reader);

unsigned char keyA[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
unsigned char writeSlice[SLICE_LEN] = { 0x99, 0x90, 0x91, 0x93 };
unsigned char readSlice[SLICE_LEN] = {};

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
        Serial.println("Writing block slice...");
        if (tag.writeBlockSlice(ADDRESS, FROM, SLICE_LEN, writeSlice)) {
            Serial.println("Reading block slice...");
            if (tag.readBlockSlice(ADDRESS, FROM, SLICE_LEN, readSlice)) {
                for (int i = 0; i < SLICE_LEN; i++) {
                    Serial.print(readSlice[i], HEX);
                    Serial.print(" ");
                }
            }
        }
        Serial.println();
        tag.halt();
        delay(1000);
    }
}
