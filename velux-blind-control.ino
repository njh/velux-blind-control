//
// Arduino Sketch to allow control of Velux blinds
// by hacking a Velux remote control
//
// D2: connected to the Close button (down)
// D3: connected to the Stop button
// D4: connected to the Open button (up)
//
// Connect to the Arduino's serial port at 9600N1
//
// Send a command followed by a newline character:
//   close
//   stop
//   open
//

#include <EveryTimerB.h>

enum {
    stateStopped,
    stateOpening,
    stateClosing
} currentState = stateStopped;

#define FULLY_OPEN   (100)
#define FULLY_CLOSED (0)

#define PIN_CLOSE  (2)
#define PIN_STOP   (3)
#define PIN_OPEN   (4)


int currentPosition = FULLY_OPEN;
int targetPosition = FULLY_OPEN;
bool reportState = true;


void updatePostion() {
    if (currentState == stateOpening) {
        currentPosition += 10;
        if (currentPosition > FULLY_OPEN) {
            currentPosition = FULLY_OPEN;
            currentState = stateStopped;
        }
          reportState = true;
    } else if (currentState == stateClosing) {
        currentPosition -= 10;
        if (currentPosition < FULLY_CLOSED) {
            currentPosition = FULLY_CLOSED;
            currentState = stateStopped;
        }
          reportState = true;
    }

    if (currentState != stateStopped && currentPosition == targetPosition) {
        // We have reached the target position
        currentState = stateStopped;
        reportState = true;
    }
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

    TimerB2.initialize();
    TimerB2.attachInterrupt(updatePostion);
    TimerB2.setPeriod(1600000); // 16 seconds / 10 steps = 1600000 microseconds

    Serial.begin(9600);
    Serial.println("# velux-blind-control");
}

void pulsePin(int pin)
{
    // Flash LED
    digitalWrite(LED_BUILTIN, HIGH);

    pinMode(pin, OUTPUT); // Disable high-impedence
    digitalWrite(pin, LOW);
    delay(200);
    pinMode(pin, INPUT); // Enable high-impedance

    digitalWrite(LED_BUILTIN, LOW);
}

void closeCommand()
{
    targetPosition = FULLY_CLOSED;
    currentState = stateClosing;
    pulsePin(PIN_CLOSE);
}

void stopCommand()
{
    targetPosition = currentPosition;
    currentState = stateStopped;
    pulsePin(PIN_STOP);
}

void openCommand()
{
    targetPosition = FULLY_OPEN;
    currentState = stateOpening;
    pulsePin(PIN_OPEN);
}

boolean isInteger(String str) {
    unsigned int stringLength = str.length();

    if (stringLength == 0) {
        return false;
    }

    for(unsigned int i = 0; i < stringLength; ++i) {
        char chr = str.charAt(i);
        if (!isDigit(chr)) {
            return false;
        }
    }
    return true;
}

void loop() {
    if (Serial.available() > 0)
    {
        String command = Serial.readStringUntil('\n');
        if (command.equalsIgnoreCase("close")) {
            closeCommand();
            Serial.println("ok");
        } else if (command.equalsIgnoreCase("open")) {
            openCommand();
            Serial.println("ok");
        } else if (command.equalsIgnoreCase("stop")) {
            stopCommand();
            Serial.println("ok");
        } else if (isInteger(command)) {
            // Keep things simple and only open or close (nothing in-between)
            int i = command.toInt();
            if (i < 50) {
               closeCommand();
            } else if (i >= 50) {
               openCommand();
            }
            Serial.println("ok");
        } else {
            Serial.println("error");
        }
    };

    // FIXME: report current position less frequently?
    if (reportState == true) {
        Serial.print("target-position: ");
        Serial.println(targetPosition, DEC);
        Serial.print("current-position: ");
        Serial.println(currentPosition, DEC);

        switch(currentState) {
           case stateClosing:
              Serial.println("state: closing");
           break;

           case stateOpening:
              Serial.println("state: opening");
           break;

           case stateStopped:
              Serial.println("state: stopped");
           break;

           default:
              Serial.println("state: error");
           break;
        }
        reportState = false;
    }
}
