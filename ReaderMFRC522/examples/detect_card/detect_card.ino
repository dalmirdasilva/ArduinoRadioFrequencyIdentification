#include <RegisterBasedSPIDevice.h>
#include <ReaderMFRC522.h>
#include <TagMF1S503x.h>

#define SS_PIN      10
#define RST_PIN     3

RegisterBasedSPIDevice device(SS_PIN);
ReaderMFRC522 reader(&device, RST_PIN);
TagMF1S503x tag(&reader);

void setup() {
    Serial.begin(9600);
    Serial.println("initialing");
    reader.initialize();
    Serial.println("done");
}

void loop() {
    if (tag.request()) {
        Serial.println("Card detected.");
    }
}