/*
 * ESP32 PWM Thermal Governor for Tesla P40
 * Reads DS18B20 temperature and controls fan speed via PWM
 */

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Pin definitions (match hardware_schematic.md)
const int ONE_WIRE_BUS = 4;   // DS18B20 data pin
const int PWM_PIN = 18;       // MOSFET gate PWM output

// PWM settings
const int PWM_FREQ = 25000;   // 25 kHz PWM frequency (good for fans)
const int PWM_CHANNEL = 0;    // LEDC channel 0
const int PWM_RESOLUTION = 8; // 8-bit resolution (0-255)

// Temperature thresholds (°C) and corresponding PWM duty (0-255)
// Adjust these values to tune the fan curve
struct FanPoint {
  float temp;
  uint8_t duty;
};

const FanPoint fanCurve[] = {
  {30.0, 0},   // Below 30°C: fan off
  {40.0, 64},  // 40°C: ~25% duty
  {50.0, 128}, // 50°C: ~50% duty
  {60.0, 192}, // 60°C: ~75% duty
  {70.0, 255}  // 70°C and above: 100% duty
};
const size_t fanCurveSize = sizeof(fanCurve) / sizeof(fanCurve[0]);

// OneWire and DallasTemperature setup
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(115200);
  while (!Serial) { ; } // Wait for serial port (optional)

  // Initialize temperature sensor
  sensors.begin();
  Serial.println("ESP32 PWM Thermal Governor");
  Serial.print("DS18B20 on GPIO");
  Serial.println(ONE_WIRE_BUS);
  Serial.print("PWM output on GPIO");
  Serial.println(PWM_PIN);

  // Setup PWM (LEDC)
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(PWM_PIN, PWM_CHANNEL);
  ledcWrite(PWM_CHANNEL, 0); // Start with fan off
}

void loop() {
  // Request temperature reading
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);

  // Check if reading is valid
  if (tempC == -127.0 || tempC == 85.0) {
    Serial.println("Error: Could not read temperature");
    // Keep current fan state or set to safe value? We'll set to max for safety.
    ledcWrite(PWM_CHANNEL, 255);
    delay(2000);
    return;
  }

  // Compute PWM duty based on fan curve (linear interpolation)
  uint8_t duty = 0;
  if (tempC <= fanCurve[0].temp) {
    duty = fanCurve[0].duty;
  } else if (tempC >= fanCurve[fanCurveSize - 1].temp) {
    duty = fanCurve[fanCurveSize - 1].duty;
  } else {
    // Find segment
    for (size_t i = 0; i < fanCurveSize - 1; ++i) {
      if (tempC >= fanCurve[i].temp && tempC <= fanCurve[i + 1].temp) {
        // Linear interpolation
        float t1 = fanCurve[i].temp;
        float t2 = fanCurve[i + 1].temp;
        uint8_t d1 = fanCurve[i].duty;
        uint8_t d2 = fanCurve[i + 1].duty;
        float ratio = (tempC - t1) / (t2 - t1);
        duty = static_cast<uint8_t>(d1 + ratio * (d2 - d1));
        break;
      }
    }
  }

  // Apply PWM
  ledcWrite(PWM_CHANNEL, duty);

  // Debug output
  Serial.print("Temp: ");
  Serial.print(tempC);
  Serial.print("°C, PWM duty: ");
  Serial.print(duty);
  Serial.print(" (");
  Serial.print(static_cast<int>((duty / 255.0) * 100));
  Serial.println("%)");

  // Wait before next reading
  delay(2000);
}