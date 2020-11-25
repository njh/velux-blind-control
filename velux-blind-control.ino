//
// Arduino Sketch to allow control of Velux blinds
// by hacking a Velux remote control
//
// D2: connected to the Close button (down)
// D3: connected to the Stop button
// D4: connected to the Open button (up)
//
// Connect to the Arduinos serial port at 9600N1
//
// Send a command followed by a newline character:
//   close
//   stop
//   open
//


void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

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
    pulsePin(2);
}

void stopCommand()
{
    pulsePin(3);
}

void openCommand()
{
    pulsePin(4);
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
        } else {
            Serial.println("error");
        }
    };
}
