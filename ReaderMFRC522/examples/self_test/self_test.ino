#include <Arduino.h>
#include <RegisterBasedSPIDevice.h>
#include <ReaderMFRC522.h>

#define SS_PIN      10
#define RST_PIN     3
#define LED_PIN     A1

RegisterBasedSPIDevice device(SS_PIN);
ReaderMFRC522 reader(&device, RST_PIN);

void setup() {
    Serial.begin(9600);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    Serial.println("Initializing...");

    reader.initialize();

    Serial.print("Version: ");
    switch (reader.getVersion()) {
    case ReaderMFRC522::CLONE:
        Serial.println("clone");
        break;
    case ReaderMFRC522::V0_0:
        Serial.println("v0_0");
        break;
    case ReaderMFRC522::V1_0:
        Serial.println("v1_0");
        break;
    case ReaderMFRC522::V2_0:
        Serial.println("v2_0");
        break;
    default:
        Serial.println("unknown");
        return;
    }

    Serial.print("Self test: ");
    if (reader.performSelfTest()) {
        digitalWrite(LED_PIN, HIGH);
        Serial.println("Passed!");
    } else {
        Serial.println("**failed**");
    }

    Serial.println("Done");
}

void loop() {
}
