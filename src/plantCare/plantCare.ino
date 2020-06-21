
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define ADC_MAX 1023  // Max value for 10 bit ADC
#define ADC_MIN 300 // minimum value of ADC, when moisture is 100%
#define SAMPLES_PER_MEASUREMENT 100.0 // Samples per measurement
#define SENSOR_PIN A0  // Analog input
#define SENSOR_POWER_PIN 2 // Powerpin for moisture sensor
#define PUMP_CONTROL_PIN 3 // Control pin for the pump
#define WATERING_TIME 5 //seconds
#define TIME_BETWEEN_WATERING 60UL //min
#define WATERING_THRESHOLD 50  // Percent moisture in soild

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
float sensor_value = 0;
int number_of_waterings = 0;
float max_moisture_value = 0;
float min_moisture_value = 100;

void setupDisplay() {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(1000); // Pause for 1 seconds
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
}

void updateDisplay(String msg) {
    Serial.println("ClearDisplay!");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(msg);
    display.display();
}

void updateMaxMoisture(float current_value) {
  if(current_value > max_moisture_value) {
    max_moisture_value = current_value;
  }
}

void updateMinMoisture(float current_value) {
  if(current_value < min_moisture_value) {
    min_moisture_value = current_value;
  }
}

void setup() { 
 Serial.begin(9600);
 pinMode(SENSOR_POWER_PIN, OUTPUT);
 pinMode(PUMP_CONTROL_PIN, OUTPUT);
 setupDisplay();
} 

void loop() {
 digitalWrite(SENSOR_POWER_PIN, HIGH);
 delay(30);
 sensor_value = 0;
 for (int i = 0; i < SAMPLES_PER_MEASUREMENT; i++) 
 { 
   sensor_value = sensor_value + analogRead(SENSOR_PIN); 
   delay(1); 
 } 
 sensor_value = map(sensor_value/SAMPLES_PER_MEASUREMENT, ADC_MIN, ADC_MAX, 100, 0);
 Serial.println(sensor_value);
 updateMaxMoisture(sensor_value);
 updateMinMoisture(sensor_value);
 updateDisplay("Moisture: " + String(sensor_value) + "%\n"
                + "Nr. of waterings:" + String(number_of_waterings) + "\n"
                + "Max moisture: " + String(max_moisture_value) + "%\n"
                + "Min moisture: " + String(min_moisture_value) + "%");
 digitalWrite(SENSOR_POWER_PIN, LOW);
 if(sensor_value <= WATERING_THRESHOLD)
 {
  digitalWrite(PUMP_CONTROL_PIN, HIGH);
  delay(WATERING_TIME);
  digitalWrite(PUMP_CONTROL_PIN, LOW);
  number_of_waterings++;
 }
 delay(TIME_BETWEEN_WATERING * 1000UL * 60UL);
}
