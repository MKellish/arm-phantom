// Adjust these vars to change tremor characteristics
double magnitude = 0.1; // in mm
double freq = 1; // in Hz
int runTime = 1; // in minutes
double probeDist = 103.95; // in mm distance center of hinge to measurement location

// Define pins according to wiring hook ups
#define STP 9 //step pin
#define DIR 8 //direction pin
#define MS1 12 //microstep 1 pin
#define MS2 11 //microstep 2 pin
#define MS3 10 //microstep 3 pin
#define EN 13 //enable pin
#define STSW 6 //switch position indicator

// Initialize variables
int amplitude;
int delayTime;
int startSwitchState =0;
int x;

void setup() {
  
  Serial.begin(9600); //Open Serial connection for debugging
  
  pinMode(STP, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(MS3, OUTPUT);
  pinMode(EN, OUTPUT);
  pinMode(STSW, INPUT);
  
  amplitude = ((magnitude/probeDist)*132.84)/0.0254*16*1.55; //calculate number of steps to reach desired mangitude
  delayTime = (freq/2)/amplitude*1000; //calculate delay time in microseconds between steps to reach desired frequency
  
  // enables motor movement at full step mode
  digitalWrite(EN, LOW);
  digitalWrite(MS1, LOW);
  digitalWrite(MS2, LOW);
  digitalWrite(MS3, LOW);
  
  // brings motor to minimum extension position
  digitalWrite(DIR, HIGH);
  
  for(x= 1; x<520; x++) //Loop the forward stepping enough times for motion to be visible
    {
      digitalWrite(STP,HIGH); //Trigger one step forward
      delay(1);
      digitalWrite(STP,LOW); //Pull step pin low so it can be triggered again
      delay(1);
    }
  
  // positions motor to hold arm parallel to mounting plate
  digitalWrite(DIR, LOW);
  
  for(x= 1; x<251; x++) //Loop the forward stepping enough times for motion to be visible
    {
      digitalWrite(STP,HIGH); //Trigger one step forward
      delay(1);
      digitalWrite(STP,LOW); //Pull step pin low so it can be triggered again
      delay(1);
    }
  
  // sets to 1/16 microstep mode
  digitalWrite(MS1, HIGH);
  digitalWrite(MS2, HIGH);
  digitalWrite(MS3, HIGH);
}

void loop(){
  
  startSwitchState = digitalRead(STSW);
  
  while (true==true)
    {
      digitalWrite(DIR, LOW); //Pull direction pin low to move "forward"
      
      for (int x=0;x<amplitude;x++) //move forward until desired magnitude is reached
        {
          digitalWrite(STP,HIGH); //Trigger one step forward
          digitalWrite(STP,LOW); //Pull step pin low so it can be triggered again
          delay(delayTime); //wait calculated delay between steps
        }
  
      digitalWrite(DIR, HIGH); //Pull direction pin high to move "reverse"
      
      for (int x=0;x<amplitude;x++) //move backward until return to "home" position
        {
          digitalWrite(STP,HIGH); //Trigger one step forward
          digitalWrite(STP,LOW); //Pull step pin low so it can be triggered again
          delay(delayTime); //wait calculated delay between steps
        }
  
      startSwitchState = digitalRead(STSW); //check switch status
    }
}
