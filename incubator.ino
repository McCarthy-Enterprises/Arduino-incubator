#include <DHT.h>
#include <DHT_U.h>

// Define the pins
const int green1 = 2;
const int green2 = 3;
const int blue = 4;
const int red = 9;
const int envSensor = 5;
const int heater = 6;
const int beeper = 7;
const int sushBtn = 8;

// Variables for managing beeper silence
bool silenced = false;
unsigned long silenceStartTime = 0;  // Time when the button was pressed

// Debounce settings
const unsigned long debounceDelay = 50;  // Debounce time in milliseconds
unsigned long lastButtonPress = 0;  // Timestamp of the last button press

// DHT11 setup
DHT dht(envSensor, DHT11);

// Desired humidity and temperature
const float targetTemp = 37.5;  // for chickens
const int targetHumidity = 53;  // for chickens, then 63% for final 3 days

void setup() {
  // Set the pin modes
  pinMode(green1, OUTPUT);
  pinMode(green2, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(heater, OUTPUT);
  pinMode(beeper, OUTPUT);
  pinMode(sushBtn, INPUT);

  // Begin debugging
  Serial.begin(9600);

  // Initialize the sensor
  dht.begin();
}

void blinkled(int lednum) {
  digitalWrite(lednum, HIGH);
  delay(500);
  digitalWrite(lednum, LOW);
  delay(500);
}

void beep() {
  if (!silenced) {
    digitalWrite(beeper, HIGH);
  }
}

void loop() {
  int switchstate = digitalRead(sushBtn);
  float humidity = dht.readHumidity();
  float temp = dht.readTemperature();

  // Check if sensor readings are valid
  if (isnan(humidity) || isnan(temp)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Display sensor readings for debugging
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print("%, Temp: ");
  Serial.print(temp);
  Serial.println("C");

  // Reset all LEDs and heater
  digitalWrite(green1, LOW);
  digitalWrite(green2, LOW);
  digitalWrite(blue, LOW);
  digitalWrite(red, LOW);
  digitalWrite(heater, LOW);
  digitalWrite(beeper, LOW);  // Ensure beeper is off initially

  // Control LEDs and beeper based on humidity and temperature
  bool humidityLow = humidity < targetHumidity;
  bool humidityHigh = humidity > targetHumidity;
  bool tempLow = temp < targetTemp;
  bool tempHigh = temp > targetTemp;

  if (humidityLow) {
    blinkled(blue);  // Blink blue LED when humidity is too low
    if (!silenced) {
      beep();  // Beep continuously if not silenced
    }
  } else if (humidityHigh) {
    digitalWrite(blue, LOW);  // Turn off blue LED when humidity is high
    digitalWrite(green1, HIGH);  // Light green1 LED when humidity is too high
    if (!silenced) {
      beep();  // Beep continuously if not silenced
    }
  }

  if (tempLow) {
    digitalWrite(heater, HIGH);  // Turn heater ON if temperature is too low
    blinkled(red);  // Blink red LED when temperature is too low
    if (!silenced) {
      beep();  // Beep continuously if not silenced
    }
  } else if (tempHigh) {
    digitalWrite(heater, LOW);  // Turn heater OFF if temperature is too high
    blinkled(red);  // Blink red LED when temperature is too high
    blinkled(green2);  // Blink green2 LED when temperature is too high
    if (!silenced) {
      beep();  // Beep continuously if not silenced
    }
  }

  // Ideal conditions
  if (!humidityLow && !humidityHigh && !tempLow && !tempHigh) {
    digitalWrite(green1, HIGH);
    digitalWrite(green2, HIGH);  // Light both green LEDs when conditions are ideal
  }

  // Silence button logic with debounce
  if (switchstate == HIGH) {
    unsigned long currentTime = millis();
    if (currentTime - lastButtonPress > debounceDelay) {
      silenced = true;
      silenceStartTime = millis();  // Record the time when the button is pressed
      lastButtonPress = currentTime;  // Update the timestamp of the last button press
    }
  }

  // Reactivate beeper after 5 minutes (300000 milliseconds)
  if (silenced && millis() - silenceStartTime >= 300000) {
    silenced = false;  // After 5 minutes, allow beeper to activate again
  }

  // Ensure beeper stops if silenced
  if (silenced) {
    digitalWrite(beeper, LOW);
  }
}
