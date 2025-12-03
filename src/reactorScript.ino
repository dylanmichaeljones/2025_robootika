#include <TM1637Display.h>
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
const int DISPLAY_FUEL = 5;
const int DISPLAY_CONTROL = 6;
const int DISPLAY_PUMPS = 7;
//vesi
const double STARTING_WATER = 2000.0;

const int TURBINE_PIN = 2;
//reaction parameters
const double FUEL_REACTIVITY = 0.0005;  //TUUNIDA SEDA REAKTIIVSUSE JAOKS
const double CONTROL_ROD_EFFECT = 0.8; //tUUNIDA SEDA CONTROL RODSIDE JAOKS
const double DECAY_RATE = 0.05;  //reaktiivsuse langus  KIIRUS ILMA KÜTUSETA
const double HEAT_RATE = 0.1; //SOOJA GENEREERIMIS EFFEKT
const double COOLING_EFFECT = 1.0;
const double PUMP_EFFECT = 0.5; //PUMBA EFFEKTIIVSUS
const double PUMP_COOLING_EFFECT = 0.2;
const double MAX_REACTIVITY = 3000.0; //PUCCISPIIR(tm)
const double MAX_HEAT = 500.0; //KUUMAHOIATUS
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



//clock
unsigned long lastUpdateTime = 0;
const unsigned long TICKRATE = 1000; //update rate, 1000 = 1 update per second


//initialize displays
TM1637Display PumpDisplay = TM1637Display(DISPLAY_CLOCK, DISPLAY_PUMPS);
TM1637Display FuelDisplay = TM1637Display(DISPLAY_CLOCK, DISPLAY_FUEL);
TM1637Display ControlDisplay = TM1637Display(DISPLAY_CLOCK, DISPLAY_CONTROL);

void setup() {
  //initialize serial
  Serial.begin(9600);

  pinMode(TURBINE_PIN, INPUT_PULLUP); // pin low = inserted true

  PumpDisplay.clear();
  FuelDisplay.clear();
  ControlDisplay.clear();
  PumpDisplay.setBrightness(7);
  FuelDisplay.setBrightness(7);
  ControlDisplay.setBrightness(7);
 
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
  
  //set current time
  lastUpdateTime = currentTime;
  }
}//end loop

void readInputs(){
  turbine_enabled = (digitalRead(TURBINE_PIN) == LOW);

  int control_raw = analogRead(CONTROL_ROD_POT);
  int pump1_raw = analogRead(PUMP1_POT);
  int pump2_raw = analogRead(PUMP2_POT);

  int fuel1_raw = analogRead(FUEL1_POT);
  int fuel2_raw = analogRead(FUEL2_POT);
  int fuel3_raw = analogRead(FUEL3_POT);
  
  int fuel1_depth = map(fuel1_raw, 0, 1023, 0, 1023);
  int fuel2_depth = map(fuel2_raw, 0, 1023, 0, 1023);
  int fuel3_depth = map(fuel3_raw, 0, 1023, 0, 1023);

  fuel_depth = fuel1_depth + fuel2_depth + fuel3_depth;

  control_rod_depth = map(control_raw, 0, 1023, 0, 1023);
  pump1_speed = map(pump1_raw, 0, 1023, 0, 100);
  pump2_speed = map(pump2_raw, 0, 1023, 0, 100);
}//end readinputs

//todo add water level simualtion if still 2 boring 4 u
void simulateReactor(){
  //delta time calculation
  double dt = (double)TICKRATE / 1000.0;
  double dR_dt; //delta reactivity
  
  //TODO ADD MORE PUMPS AND FUEL BELOW
  //reactivity func
  int rods_reactivity = fuel_depth * FUEL_REACTIVITY; //reaktiivsus jagatakse 3 varda vahel

  if (fuel_depth > 100 && reactivity == 0.0){ //kickstart the reactor if its a cold start
    reactivity = STARTUP_REACTIVITY;
  }else if (fuel_depth > 10 && reactivity > 3.0){
  dR_dt = rods_reactivity * reactivity - (CONTROL_ROD_EFFECT * (double)control_rod_depth / 50.0);

  } else { //kui kütust pole sees

  dR_dt = rods_reactivity * reactivity 
       - (CONTROL_ROD_EFFECT * (double)control_rod_depth / 1023.0);
  }//end reactivity if else
  
  //calc reactivity and clamp it to positive
  reactivity += dR_dt * dt;
  if(reactivity<0.0) reactivity=0.0;
  
  //---------------heat simulation-------------
    //calc heat and clamp it to pos
  double dH_dt = HEAT_RATE * reactivity -  (PUMP_EFFECT + COOLING_EFFECT) * ((double)pump1_speed + (double)pump2_speed * PUMP_COOLING_EFFECT) / 4.0;
  heat += dH_dt * dt;
  if(heat<0.0) heat = 0.0;

}//end simulateReactor

//todo: effektiivsuspiirkond lisada turbiinile sõltuvalt reaktori iseloomust,
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
    
    //TODO: if power üle mingi 3000 on overspeed ja turbina aiblinarina
    if(turbine_enabled){
    power = steam * steam - (pump1_speed + pump2_speed);
    }//turbine
  }
}//end turbine

void displayInfo(){
  int pumpsInfo = static_cast<int>(pump1_speed + pump2_speed);
  int fuelInfo = static_cast<int>(fuel_depth);
  int controlInfo = static_cast<int>(control_rod_depth);
  PumpDisplay.showNumberDec(pumpsInfo);
  FuelDisplay.showNumberDec(fuelInfo);
  ControlDisplay.showNumberDec(controlInfo);
}

void printStatus() {
    char buffer[128];

    // Header
    Serial.println("+--------------------------------------------------------------------------------+");
    Serial.print("  fuel_depth");
    Serial.print(fuel_depth);
    Serial.print("  control_rod_depth");
    Serial.print(control_rod_depth);
    Serial.print("  pumps");
    Serial.print( pump1_speed);
    Serial.print(", ");
    Serial.print( pump2_speed);
    Serial.println();
    Serial.print("  reactivity");
    Serial.print( reactivity);
    Serial.print("  heat");
    Serial.print( heat);
    Serial.print("  power");
    Serial.print( power);
    Serial.print("  turbine");
    Serial.print( turbine_enabled);
    Serial.print("  water level");
    Serial.print(water_level);
    Serial.println();
    Serial.println("+--------------------------------------------------------------------------------+");

    // Warnings
    if (reactivity > MAX_REACTIVITY) {
        Serial.println("⚠️  WARNING: Reactivity runaway imminent!");
    }
    if (heat > MAX_HEAT) {
        Serial.println("🔥 ALERT: Meltdown risk — heat too high!");
    }
}//endprintstatus
