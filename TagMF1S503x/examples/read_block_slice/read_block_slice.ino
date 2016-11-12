#include <ReaderMFRC522.h>
#include <RegisterBasedSPIDevice.h>
#include <TagMF1S503x.h>

#define SS_PIN          10
#define RST_PIN         3
#define SLICE_ADDRESS   3
#define SLICE_FROM      10
#define SLICE_LEN       6

RegisterBasedSPIDevice device(SS_PIN);
ReaderMFRC522 reader(&device, RST_PIN);
TagMF1S503x tag(&reader);

unsigned char keyA[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
unsigned char slice[SLICE_LEN];

void setup() {
    Serial.begin(9600);
    Serial.println("Initialing...");
    reader.initialize();
    tag.setupAuthenticationKey(Tag::KEY_A, keyA);
    Serial.println("Waiting card proximity...");
}

void loop() {
    if (tag.activate()) {
        Serial.println("Card detected");
        Serial.println("Reading block slice...");
        if (tag.readBlockSlice(SLICE_ADDRESS, SLICE_FROM, SLICE_LEN, slice)) {
            for (int i = 0; i < SLICE_LEN; i++) {
                Serial.print(slice[i], HEX);
                Serial.print(" ");
            }
        }
        Serial.println();
        tag.halt();
        delay(1000);
    }
}
