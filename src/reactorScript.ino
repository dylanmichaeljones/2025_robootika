#include <TM1637Display.h>
#include <FastLED.h> // ADDED: FastLED library for RGB strip

//Pins------------
//fuel rods
const int FUEL1_POT = A1;
const int FUEL2_POT = A2;
const int FUEL3_POT = A3;
//control rod
const int CONTROL_ROD_POT = A0;
//Pumps
const int PUMP1_POT = A4;
const int PUMP2_POT = A5;
const int TURPIN_LED = 3;
const int TURPIN = 2;
const int DISPLAY_CLOCK = 4;
const int DISPLAY_REACTIVITY = 5;
const int DISPLAY_POWER = 6;
const int DISPLAY_HEAT = 7;
const int DISPLAY_WATER = 8;
const int TURBINE_PIN = 2;

// ADDED: FastLED Definitions
#define NUM_LEDS 9       // 3 rows of 3 LEDs (for 3 fuel rods)
#define LED_PIN 8        // Data pin for your LED strip
#define FUEL_COLOR CRGB::Green // Color for the fuel rods
CRGB leds[NUM_LEDS];     // The LED array
// END ADDED

//vesi
const double STARTING_WATER = 2000.0;


//reaction parameters
const double FUEL_REACTIVITY = 0.0005;  //TUUNIDA SEDA REAKTIIVSUSE JAOKS
const double CONTROL_ROD_EFFECT = 1; //tUUNIDA SEDA CONTROL RODSIDE JAOKS
const double DECAY_RATE = 0.05;  //reaktiivsuse langus  KIIRUS ILMA KÜTUSETA
const double HEAT_RATE = 0.1; //SOOJA GENEREERIMIS EFFEKT
const double COOLING_EFFECT = 1.0;
const double PUMP_EFFECT = 0.5; //PUMBA EFFEKTIIVSUS
const double PUMP_COOLING_EFFECT = 0.2;
const double MAX_REACTIVITY = 10000.0; //PUCCISPIIR(tm)
const double MAX_HEAT = 110.0; //KUUMAHOIATUS
const double MIN_WATERLEVEL = 1750.0; //VEETASEME HOIATUS
const double STARTUP_REACTIVITY = 5.0; //KICKSTART REAKTIIVSUS KUI KÕIK VARDAD SISESTATUD
const double STEAM_EFFECT = 0.05;

//VARS-------------
//reactor
double reactivity = 0.0;
double heat = 18.0; //startup heat
double power = 0.0;
int control_rod_depth = 0;
int pump1_speed = 0;
int pump2_speed = 0;
bool turbine_enabled = 0;
int fuel_depth = 0;
double water_level = 2000.0;

// ADDED: Global individual fuel rod depths (for LED control)
int fuel1_depth_global = 0;
int fuel2_depth_global = 0;
int fuel3_depth_global = 0;
// END ADDED

//clock
unsigned long lastUpdateTime = 0;
const unsigned long TICKRATE = 1000; //update rate, 1000 = 1 update per second


//initialize displays
TM1637Display TempDisplay = TM1637Display(DISPLAY_CLOCK, DISPLAY_HEAT);
TM1637Display ReactivityDisplay = TM1637Display(DISPLAY_CLOCK, DISPLAY_REACTIVITY);
TM1637Display PowerDisplay = TM1637Display(DISPLAY_CLOCK, DISPLAY_POWER);
TM1637Display WaterLevelDisplay = TM1637Display(DISPLAY_CLOCK, DISPLAY_WATER);

// ADDED: FastLED Helper Function
void setFuelRodLeds(int fuelValue, int startLed, CRGB color) {
  uint8_t brightness = map(fuelValue, 0, 1023, 0, 255);
  for (int i = 0; i < 3; i++) {
    leds[startLed + i] = color;
    leds[startLed + i].nscale8(brightness);
  }
}
// END ADDED

void setup() {
  //initialize serial
  Serial.begin(9600);

  // ADDED: Initialize FastLED
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(255);
  FastLED.clear();
  FastLED.show();
  // END ADDED

  pinMode(TURBINE_PIN, INPUT_PULLUP); // pin low = inserted true
  pinMode(TURPIN_LED, OUTPUT); // Ensure TURPIN_LED is set as OUTPUT

  TempDisplay.clear();
  ReactivityDisplay.clear();
  PowerDisplay.clear();
  WaterLevelDisplay.clear();
  TempDisplay.setBrightness(7);
  ReactivityDisplay.setBrightness(7);
  PowerDisplay.setBrightness(7);
  WaterLevelDisplay.setBrightness(7);
 
 
  //finish setup message
  Serial.println("Reactor online, weapons online, all systems nominal");
}//end setup

void loop() {
  
  unsigned long currentTime = millis();
  if(currentTime-lastUpdateTime >= TICKRATE){ //checks if its time to update
    readInputs();
    simulateReactor();
    simulateTurbine();
    printStatus();
    displayInfo();
    displayFuelRods(); // ADDED: Update the LED strip
    
  //set current time
  lastUpdateTime = currentTime;
  }
}//end loop

void readInputs(){
  turbine_enabled = (digitalRead(TURBINE_PIN) == LOW);

  int control_raw = analogRead(CONTROL_ROD_POT);
  int pump1_raw = analogRead(PUMP1_POT);
  int pump2_raw = analogRead(PUMP2_POT);

  // Read and store the raw analog values into the global variables for LED display
  fuel1_depth_global = analogRead(FUEL1_POT);
  fuel2_depth_global = analogRead(FUEL2_POT);
  fuel3_depth_global = analogRead(FUEL3_POT);
  int fuel1_raw = analogRead(FUEL1_POT);
  int fuel2_raw = analogRead(FUEL2_POT);
  int fuel3_raw = analogRead(FUEL3_POT);
  
  // Use the global values for the local depth calculation
  int fuel1_depth = map(fuel1_depth_global, 0, 1023, 0, 1023);
  int fuel2_depth = map(fuel2_depth_global, 0, 1023, 0, 1023);
  int fuel3_depth = map(fuel3_depth_global, 0, 1023, 0, 1023);

  fuel_depth = fuel1_depth + fuel2_depth + fuel3_depth;

  control_rod_depth = map(control_raw, 0, 1023, 0, 1023);
  pump1_speed = map(pump1_raw, 0, 1023, 0, 100);
  pump2_speed = map(pump2_raw, 0, 1023, 0, 100);
}//end readinputs

//todo add water level simualtion if still 2 boring 4 u
void simulateReactor(){
  //delta time calculation
  double dt = (double)TICKRATE / 1000.0; //DT delta time
  double dR_dt; //delta taim reaktiivsus ajas
 

// --- Reactivity calculation ---
double rods_reactivity = (fuel_depth / 3.0) * FUEL_REACTIVITY; 
double control_effect = CONTROL_ROD_EFFECT * (double)control_rod_depth;

// kickstart
if (reactivity == 0.0 && fuel_depth > 100) {
    reactivity = STARTUP_REACTIVITY;
}

// reactivity growth when all things applied
dR_dt = (rods_reactivity * (1.0 + reactivity)) - control_effect;


reactivity += dR_dt * dt;

//clamp reactivity to pos.
if (reactivity < 0.0) reactivity = 0.0;
  //---------------heat simulation-------------
    //calc heat and clamp it to pos
  double dH_dt = HEAT_RATE * reactivity -  (PUMP_EFFECT + COOLING_EFFECT) * ((double)pump1_speed + (double)pump2_speed * PUMP_COOLING_EFFECT) / 4.0;
  heat += dH_dt * dt;
  if(heat<0.0) heat = 0.0;

}//end simulateReactor

void simulateTurbine(){
  double dt = (double)TICKRATE / 1000.0;
  
  turbine_enabled = (digitalRead(TURPIN) == LOW);
  digitalWrite(TURPIN_LED, turbine_enabled ? HIGH : LOW);

  //kui vesi soe tee auru ja vähenda vett
  if (heat > 70.0){
    double steam = (heat - 70) * STEAM_EFFECT;

    //kui vett on üle miinimumi ära fläshboili seda
    if (water_level > 100.0){
      if (water_level > 2000.0){
      water_level = 2000.0;
      }

      heat = heat - COOLING_EFFECT * steam;
      if(heat > 100.0){
        heat = 100.0;
      }
    }
    
    double steam_cooling_effect = steam * COOLING_EFFECT;


    //veekadu on aur korda aeg
    double evap_rate = (steam * dt);

    //vesi kaub evap ratega ja tuleb juurde pumbaga
    water_level = water_level - evap_rate + ((pump1_speed + pump2_speed) * PUMP_EFFECT);;

    //clamp water level to min max
    if(water_level < 0.0){
      water_level = 0.0;
    }

    if(turbine_enabled){
    power = steam * steam - (pump1_speed + pump2_speed);
    }//turbine
  }
}//end turbine

void displayInfo(){
  int tempInfo = static_cast<int>(heat);
  int reactivityInfo = static_cast<int>(reactivity);
  int powerInfo = static_cast<int>(power);
  int waterLevel = static_cast<int>(water_level);
  TempDisplay.showNumberDec(tempInfo);
  ReactivityDisplay.showNumberDec(reactivityInfo);
  PowerDisplay.showNumberDec(powerInfo);
  WaterLevelDisplay.showNumberDec(waterLevel);
}

// ADDED: Function to update FastLED strip
void displayFuelRods() {
  // Fuel Rod 1: LEDs 0, 1, 2
  setFuelRodLeds(fuel1_depth_global, 0, FUEL_COLOR);
  
  // Fuel Rod 2: LEDs 3, 4, 5
  setFuelRodLeds(fuel2_depth_global, 3, FUEL_COLOR);

  // Fuel Rod 3: LEDs 6, 7, 8
  setFuelRodLeds(fuel3_depth_global, 6, FUEL_COLOR);

  FastLED.show();
}
// END ADDED

void printStatus() {
  char buffer[128];

  // Header
  Serial.println("+--------------------------------------------------------------------------------+");
  Serial.print("  fuel_depth");
  Serial.print(fuel_depth);
  Serial.print("  control_rod_depth");
  Serial.print(control_rod_depth);
  Serial.print("  pumps");
  Serial.print( pump1_speed);
  Serial.print(", ");
  Serial.print( pump2_speed);
  Serial.println();
  Serial.print("  reactivity");
  Serial.print( reactivity);
  Serial.print("  heat");
  Serial.print( heat);
  Serial.print("  power");
  Serial.print( power);
  Serial.print("  turbine");
  Serial.print( turbine_enabled);
  Serial.print("  water level");
  Serial.print(water_level);
  Serial.println();
  Serial.println("+--------------------------------------------------------------------------------+");

  // Warnings
    if (reactivity > MAX_REACTIVITY) {
        Serial.println(" WARNING: Reactivity runaway imminent!");
    }
    if (heat > MAX_HEAT) {
        Serial.println(" ALERT: Meltdown risk — heat too high!");
    }
       if (water_level < MIN_WATERLEVEL) {
        Serial.println("ALERT: Water level dangerously low!");
    }
}//endprintstatus
