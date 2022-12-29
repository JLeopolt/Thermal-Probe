#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "max6675.h"

// Component configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1 // No reset pin.
int thermoDO = 12; // Thermocouple pins
int thermoCS = 15; // Thermocouple pins
int thermoCLK = 14; // Thermocouple pins

// Software configuration
int number_of_trials = 3; // Number of samples to get before displaying an avg temperature value. Greater value -> Longer delays
int stable_threshold = 3; // Number of times an average has to match previous avg to be considered 'stable'.
int sensor_delay_time = 250; // Delay in ms to wait between each temperature scan. 250ms is minimum delay for thermocouple to update.

// Initialize display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// Initialize thermocouple.
MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

void setup()   {        
  // Configure serial, and start display.      
  Serial.begin(9600);
	display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  // Title
  display.setTextColor(BLACK, WHITE);
	display.setCursor(0,0);
  display.setTextSize(3);
	display.println("Thermal");
  display.println(" Probe ");
  // Print copyright disclaimer
  display.setCursor(0,48);
	display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
	display.println("(c) PyroNeon Tech.");
  display.println("All rights reserved.");
	display.display();
	delay(2000);
	display.clearDisplay();

  // Configuration Settings
  display.setTextColor(BLACK, WHITE);
	display.setCursor(0,0);
  display.setTextSize(2);
	display.println("Config.");
  
	display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
	display.print("No. Trials: ");
  display.println(number_of_trials);
  display.print("Delay P/T: ");
  display.print(sensor_delay_time);
  display.println("ms");
  display.print("Stability #: ");
  display.println(stable_threshold);

	display.display();
	delay(2000);
	display.clearDisplay();
}

// Gets the average temperature (in Celsius) over 'trials' number of trials.
float get_average_temperature(int trials){
  float sum = 0.0;
  for (int i = 0; i < trials; i++){
    sum += thermocouple.readCelsius();
    delay(sensor_delay_time);
  }
  return sum / trials;
}

float last_avg_temp = 0.0;
int times_matched = 0;

void loop() {
  // Get the average before clearing display - This should block for a short while.
  float new_avg_temp = get_average_temperature(number_of_trials);

  if(new_avg_temp == last_avg_temp){
    // Increment times matched counter.
    times_matched++;
    if(times_matched >= stable_threshold){
      // Invert colors - Indicating a stable value.
      display.setTextColor(WHITE, BLACK);
    }
  }
  else{
    // Don't change colors - Indicating an unstable value.
    display.setTextColor(WHITE, BLACK);
    // Reset times matched.
    times_matched = 0;
    // Save this temp in cache.
    last_avg_temp = new_avg_temp;
  }
  
  display.setTextSize(3);

  // For numbers less than 100, use 2 decimals.
  int decimals = 2;
  if(new_avg_temp > 999){
    // For numbers >999, DONT use decimals.
    decimals = 0;
  }
  else if(new_avg_temp > 99){
    // For numbers >99, use a single decimal.
    decimals = 1;
  }
  // Get length of the float.
  String string_avg = String(new_avg_temp, decimals);
  
  // Update the display.
  display.clearDisplay();
  int center_x = (SCREEN_WIDTH - string_avg.length() * 18 - 54)/2;
  display.setCursor(center_x,20);
  display.print(" ");
  display.print(string_avg);
  display.print("C ");
  display.display();
}
