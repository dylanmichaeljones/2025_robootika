//Pins
const int FUEL_ROD_PIN = 2;
const int CONTROL_ROD_POT = A0;
const int PUMP_POT = A1;

//reaction parameters
const double FUEL_REACTIVITY = 0.0001;  //TUUNIDA SEDA REAKTIIVSUSE JAOKS
const double CONTROL_ROD_EFFECT = 0.05; //tUUNIDA SEDA CONTROL RODSIDE JAOKS
const double DECAY_RATE = 0.05;  //JAHTUMIS KIIRUS ILMA KÜTUSETA
const double HEAT_RATE = 0.5; //SOOJA GENEREERIMIS EFFEKT
const double PUMP_EFFECT = 1.0; //PUMBA EFFEKTIIVSUS
const double MAX_REACTIVITY = 1000.0; //PUCCISPIIR(tm)
const double MAX_HEAT = 500.0; //KUUMAHOIATUS

//VARS
//reactor
double reactivity = 0.0;
double heat = 0.0;
uint8_t control_depth = 0;
uint8_t pump_speed = 0;
bool fuel_inserted = 0;

//clock
unsigned long lastUpdateTime = 0;
const unsigned long TICKRATE = 1000; //update rate, 1000 = 1 update per second

void setup() {
  //initialize serial
  Serial.begin(9600);
 
  //pins startup
  pinMode(FUEL_ROD_PIN, INPUT_PULLUP); // pin low = inserted true
 
  //finish setup message
  Serial.println("Reactor online, weapons online, all systems nominal");
}//end setup

void loop() {
  
  unsigned long currentTime = millis();
  if(currentTime-lastUpdateTime >= TICKRATE){ //checks if its time to update
  readInputs();
  simulateReactor();
  printStatus();
  
  //set current time
  lastUpdateTime = currentTime;
  }
}//end loop

void readInputs(){
  fuel_inserted = (digitalRead(FUEL_ROD_PIN) == LOW);
  int control_raw = analogRead(CONTROL_ROD_POT);
  int pump_raw = analogRead(PUMP_POT);
  
  control_depth = map(control_raw, 0, 1023, 0, 1023);
  pump_speed = map(pump_raw, 0, 1023, 0, 100);
}

void simulateReactor(){
  //delta time calculation
  double dt = (double)TICKRATE / 1000.0;
  double dR_dt; //delta reactivity
  
  //reactivity func
  if (fuel_inserted && reactivity == 0.0){ //kickstart the reactor if its a cold start
    reactivity = 5.0;
  }else if (fuel_inserted){
  dR_dt =  FUEL_REACTIVITY * reactivity * reactivity - CONTROL_ROD_EFFECT * (double)control_depth;

  } else {
    dR_dt = - DECAY_RATE * reactivity;
  }//end reactivity if else
  
  //calc reactivity and clamp it to positive
  reactivity += dR_dt * dt;
  if(reactivity<0.0) reactivity=0.0;
  
  //calc heat and clamp it to pos
  double dH_dt = HEAT_RATE * reactivity - PUMP_EFFECT * (double)pump_speed;
  heat += dH_dt * dt;
  if(heat<0.0) heat = 0.0;
}//end simulateReactor

void printStatus(){
    Serial.print("Fuel Rods: ");
    Serial.print(fuel_inserted ? "Yes" : "No");
    Serial.print(" | Control Rods: ");
    Serial.print(control_depth);
    Serial.print(" | Pump Speed: ");
    Serial.print(pump_speed);
    Serial.print(" | Reactivity: ");
    Serial.print(reactivity);
    Serial.print(" | Heat: ");
    Serial.print(heat);
  
    if (reactivity > MAX_REACTIVITY) {
      Serial.print(" | WARNING: Reactivity runaway imminent!");
    }
    if (heat > MAX_HEAT) {
      Serial.print(" | ALERT: Meltdown risk - heat too high!");
    }
    Serial.println();
}//end printStatus
