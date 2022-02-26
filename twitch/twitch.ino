// Adjust these vars to change twitch characteristics
double magnitude = 1; // in mm
double avgvelocity = 10 ; //in mm/s
double interval = 30; // in s time between twitch
int runTime = 1; // in minutes

//Define pins according to wiring hook ups
#define STP 4 //step pin for motor 1
#define DIR 3 //direction pin for motor 1
#define EN 2 //enable pin for motor 1
#define STP2 7 //step pin for motor 2
#define DIR2 8 //direction pin for motor 2
#define EN2 6 //enable pin for motor 2
#define STSW 12 //switch position indicator

// Initialize variables
int amplitude;
int startSwitchState =0;
int x;
int delayTime;
int delayInterval;

void setup() {
  
  Serial.begin(9600); //Open Serial connection for debugging
  
  pinMode(STP, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(EN, OUTPUT);
  pinMode(STSW, INPUT);
  pinMode(STP2, OUTPUT);
  pinMode(DIR2, OUTPUT);
  pinMode(EN2, OUTPUT);
  pinMode(STSW, INPUT);
  
  digitalWrite(EN, LOW); //enables movement of motor 1
  digitalWrite(EN2, LOW); //enables movement of motor 2
  
  amplitude = (magnitude/0.2)*16; //calculate number of steps required to reach magnitude
  delayTime = (magnitude/avgvelocity)*1000000/amplitude-40; //calculate delay time in microseconds required to attain avg velocity
  delayInterval = interval*1000; //convert input interval to milliseconds
}

void loop() {

startSwitchState = digitalRead(STSW);

while (STSW==HIGH) //occurs only while switch is turned on
  {
    digitalWrite(DIR, HIGH); //Pull direction pin high to move "forward"
    
    for (int x=0;x<amplitude;x++) //move forward until desired magnitude is reached
      {
        digitalWrite(STP,HIGH); //Trigger one step forward
        digitalWrite(STP,LOW); //Pull step pin low so it can be triggered again
        delayMicroseconds(delayTime); //wait calculated delay between steps
      }
      
    digitalWrite(DIR, LOW); //Pull direction pin low to move "reverse"
    
    for (int x=0;x<amplitude;x++) //move backward until return to "home" position
      {
        digitalWrite(STP,HIGH); //Trigger one step forward
        digitalWrite(STP,LOW); //Pull step pin low so it can be triggered again
        delayMicroseconds(delayTime); //wait calculated delay between steps
      }
    
    startSwitchState = digitalRead(STSW); //check switch status
    delay(delayInterval); //wait calculated time between twitches
    digitalWrite(DIR2, HIGH); //Pull direction pin high to move "forward"
    
    for (int x=0;x<amplitude;x++) //move forward until desired magnitude is reached
      {
        digitalWrite(STP2,HIGH); //Trigger one step forward
        digitalWrite(STP2,LOW); //Pull step pin low so it can be triggered again
        delayMicroseconds(delayTime); //wait calculated delay between steps
      }
    
    digitalWrite(DIR2, LOW); //Pull direction pin low to move "reverse"
    
    for (int x=0;x<amplitude;x++) //move backward to return to "home" position
      {
        digitalWrite(STP2,HIGH); //Trigger one step forward
        digitalWrite(STP2,LOW); //Pull step pin low so it can be triggered again
        delayMicroseconds(delayTime); //wait calculated delay between steps
      }
    
    startSwitchState = digitalRead(STSW); //check swtich status
    delay(delayInterval); //wait calculated time between twitches
  }
}
