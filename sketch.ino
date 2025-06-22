#include <DHT.h>
#include <Servo.h>

// Pin Definitions
#define DHTPIN 2
#define DHTTYPE DHT22
#define RAIN_SENSOR_PIN 3 // Push button as rain sensor
#define TRIG_PIN 4
#define ECHO_PIN 5
#define SERVO_PIN 6

// Thresholds
const float TEMPERATURE_THRESHOLD = 25.0; // Minimum temperature for drying (in °C)
const float HUMIDITY_THRESHOLD = 70.0;    // Maximum humidity for drying (in %)
const float OBSTACLE_DISTANCE_THRESHOLD = 20.0; // cm
const int RAIN_DETECTION_STATE = LOW;          // Button pressed indicates rain

// Objects
DHT dht(DHTPIN, DHTTYPE);
Servo dryingRack;

void setup() {
  Serial.begin(9600);
  dht.begin();
  dryingRack.attach(SERVO_PIN);

  pinMode(RAIN_SENSOR_PIN, INPUT_PULLUP); // Push button with pull-up configuration
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  dryingRack.write(0); // Start with the rack retracted
  Serial.println("System Initialized");
}

void loop() {
  // Read humidity and temperature
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Validate sensor readings
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Error: Failed to read from DHT sensor!");
    delay(2000); // Retry after 2 seconds
    return;
  }

  // Read rain sensor (push button state)
  bool isRaining = digitalRead(RAIN_SENSOR_PIN) == RAIN_DETECTION_STATE;

  // Read obstacle distance
  long duration = getUltrasonicDistance();
  float distance = duration * 0.034 / 2;

  // Display data on Serial Monitor
  Serial.print("Humidity: "); Serial.print(humidity); Serial.print("% ");
  Serial.print("Temperature: "); Serial.print(temperature); Serial.println("°C");
  Serial.print("Obstacle Distance: "); Serial.print(distance); Serial.println(" cm");
  Serial.print("Rain Detected: "); Serial.println(isRaining ? "Yes" : "No");

  // Decision Making
  if (isRaining || distance < OBSTACLE_DISTANCE_THRESHOLD) {
    dryingRack.write(0); // Retract the rack
    Serial.println("Rack Retracted: Rain or Obstacle Detected");
  } else if (temperature >= TEMPERATURE_THRESHOLD && humidity <= HUMIDITY_THRESHOLD) {
    dryingRack.write(90); // Extend the rack
    Serial.println("Rack Extended: Conditions Suitable for Drying");
  } else {
    dryingRack.write(0); // Retract the rack
    Serial.println("Rack Retracted: Unsuitable Drying Conditions");
  }

  delay(2000); // Delay for 2 seconds
}

// Function to measure ultrasonic sensor distance
long getUltrasonicDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  return pulseIn(ECHO_PIN, HIGH);
}
