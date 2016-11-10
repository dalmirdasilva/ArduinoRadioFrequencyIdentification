#include <ReaderMFRC522.h>
#include <RegisterBasedSPIDevice.h>
#include <TagMF1S503x.h>

#define SS_PIN      10
#define RST_PIN     3

RegisterBasedSPIDevice device(SS_PIN);
ReaderMFRC522 reader(&device, RST_PIN);
TagMF1S503x tag(&reader);

#define SLICE_LEN       10
unsigned char keyA[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
unsigned char slice[SLICE_LEN];

void setup() {
    Serial.begin(9600);
    Serial.println("initialing");
    reader.initialize();
    tag.setupAuthenticationKey(Tag::KEY_A, keyA);
    Serial.println("done");
}

void loop() {
    if (tag.activate()) {
        Serial.println("Card detected");
        Serial.println("Reading block slice...");
        if (tag.readBlockSlice(1, 0, SLICE_LEN, slice)) {
            for (int i = 0; i < SLICE_LEN; i++) {
                Serial.print(slice[i], HEX);
                Serial.print(" ");
            }
        } else {
            Serial.print("Last error was: ");
            Serial.println(reader.getLastError());
        }
        Serial.println();
        tag.halt();
        delay(2000);
    }
}
