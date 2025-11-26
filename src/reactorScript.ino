//TODO ADD TURBINES
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

const int TURBINE_PIN = 2;
//reaction parameters
const double FUEL_REACTIVITY = 0.002;  //TUUNIDA SEDA REAKTIIVSUSE JAOKS
const double CONTROL_ROD_EFFECT = 8.0; //tUUNIDA SEDA CONTROL RODSIDE JAOKS
const double DECAY_RATE = 0.05;  //JAHTUMIS KIIRUS ILMA KÜTUSETA
const double HEAT_RATE = 0.5; //SOOJA GENEREERIMIS EFFEKT
const double PUMP_EFFECT = 1.0; //PUMBA EFFEKTIIVSUS
const double MAX_REACTIVITY = 1000.0; //PUCCISPIIR(tm)
const double MAX_HEAT = 500.0; //KUUMAHOIATUS
const double STARTUP_REACTIVITY = 5.0; //KICKSTART REAKTIIVSUS KUI KÕIK VARDAD SISESTATUD

//VARS-------------
//reactor
double reactivity = 0.0;
double heat = 0.0;
double power = 0.0;
uint8_t control_rod_depth = 0;
uint8_t pump1_speed = 0;
uint8_t pump2_speed = 0;
bool turbine_enabled = 0;
uint8_t fuel_depth = 0;



//clock
unsigned long lastUpdateTime = 0;
const unsigned long TICKRATE = 1000; //update rate, 1000 = 1 update per second

void setup() {
  //initialize serial
  Serial.begin(9600);

  pinMode(TURBINE_PIN, INPUT_PULLUP); // pin low = inserted true
 
 
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

  if (fuel_depth > 200 && reactivity == 0.0){ //kickstart the reactor if its a cold start
    reactivity = STARTUP_REACTIVITY;
  }else if (fuel_depth > 10){
  dR_dt = rods_reactivity * reactivity - (CONTROL_ROD_EFFECT * (double)control_rod_depth / 50.0);

  } else { //kui kütust pole sees

    dR_dt = - DECAY_RATE * reactivity;
  }//end reactivity if else
  
  //calc reactivity and clamp it to positive
  reactivity += dR_dt * dt;
  if(reactivity<0.0) reactivity=0.0;
  
  //---------------heat simulation-------------
    //calc heat and clamp it to pos
  double dH_dt = HEAT_RATE * reactivity - PUMP_EFFECT * ((double)pump1_speed + (double)pump2_speed) / 4.0;
  heat += dH_dt * dt;
  if(heat<0.0) heat = 0.0;

}//end simulateReactor

//todo: effektiivsuspiirkond lisada turbiinile sõltuvalt reaktori iseloomust,
void simulateTurbine(){
  //kui vesi keeb tee auru
  if (heat > 100.0){
    double steam = (heat - 100.0) * 2;
    power = steam * steam;
  }
}//end turbine

void printStatus(){
    Serial.print("Fuel rod total insertion");
    Serial.print(fuel_depth);
    Serial.print(" | Control Rods: ");
    Serial.print(control_rod_depth);
    Serial.print(" | Pump1 and 2 Speed: ");
    Serial.print(pump1_speed, pump2_speed);
    Serial.print(" | Reactivity and heat");
    Serial.print(reactivity);
    Serial.print(heat);
    Serial.print("Power");
    Serial.print(power);

  
    if (reactivity > MAX_REACTIVITY) {
      Serial.print(" | WARNING: Reactivity runaway imminent!");
    }
    if (heat > MAX_HEAT) {
      Serial.print(" | ALERT: Meltdown risk - heat too high!");
    }
    Serial.println();
}//end printStatus
