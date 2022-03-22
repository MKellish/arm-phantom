// Adjust these vars to change twitch characteristics
double magnitude = 1; // in mm
double avgvelocity = 1 ; //in mm/s
double interval = 5; // in s time between twitch
int runTime = 1; // in minutes

//Define pins according to wiring hook ups
#define STP 6 //step pin
#define DIR 7 //direction pin

// Initialize variables
int amplitude;
int x;
int delayTime;
int delayInterval;

void setup() {

  Serial.begin(9600); //Open Serial connection for debugging

  pinMode(STP, OUTPUT);
  pinMode(DIR, OUTPUT);

  amplitude = (magnitude/0.2)*16; //calculate number of steps required to reach magnitude
  delayTime = (magnitude/avgvelocity)*1000000/amplitude-40; //calculate delay time in microseconds required to attain avg velocity
  delayInterval = interval*1000; //convert input interval to milliseconds
}

void loop() {

while (true==true) //occurs only while switch is turned on
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

    //startSwitchState = digitalRead(STSW); //check switch status
    delay(delayInterval); //wait calculated time between twitches
  }
}
