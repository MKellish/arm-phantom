//INPUTS
double fluidVelocity = 23.0; // in cm/s
double pulsatility = 0.0; // Pulsatility Index in decimal form
int dispenseTime = 5; // in minutes

//CALCULATIONS
int stepsPerSecond = (fluidVelocity*0.0125*0.0125)/(0.6225*0.6225)*10000/0.25; // Calculate steps per second relating a 125 um radius tube to a 6.225 mm radius syringe (5mL)
//with 1/16th micro step (0.25 is the number of um per microstep) and conversion magnitude factor
int microsecondsPerStep = 1000000/double(stepsPerSecond); //Invert steps per second to get seconds per step (with conversion to make microseconds)

//Declare pin functions on Arduino
#define STPV 50 //Step Pin
#define DIRV 52 //Direction Pin
#define MS1V 24 //Microstep Channel 1 Pin
#define MS2V 26 //Microstep Channel 2 Pin
#define MS3V 28 //Microstep Channel 3 Pin
#define ENV 22 //Enable Pin
#define FWDV 35 //Forward Manual Button Pin
#define REVV 37 //Reverse Manual Button Pin
#define STSW 23 //Manual Start Switch Pin

//Define Variable
int x=0; //counter
int z=0; //counter
double delaytime[500]; //array to hold a variable delaytime (up to 500 steps per second)
unsigned long arr[500]; //array to read a variable delaytime if verification is desired
int buttonStateFwd = 0; //varaiable to read if forward button is pressed
int buttonStateRev = 0; //variable to read if reverse button is pressed
int startSwitchState = 0; //variable to read if switch is on or off

void setup() {
 
  Serial.begin(9600); //Open Serial connection for debugging
  
  //Define all control pins as output and reading pins as input
  pinMode(STPV, OUTPUT);
  pinMode(DIRV, OUTPUT);
  pinMode(MS1V, OUTPUT);
  pinMode(MS2V, OUTPUT);
  pinMode(MS3V, OUTPUT);
  pinMode(ENV, OUTPUT);
  pinMode(FWDV, INPUT);
  pinMode(REVV, INPUT);
  pinMode(STSW, INPUT);
  
  digitalWrite(ENV, LOW); // Enable Motor
  digitalWrite(DIRV, HIGH); //Set to move forward
  digitalWrite(MS1V, LOW); //Set to Full-step Microstepping Mode
  digitalWrite(MS2V, LOW);
  digitalWrite(MS3V, LOW);
  
  for (int i=0; i<stepsPerSecond; i++)//make an array where number of elements = number of steps per second
    {
      //Delay time in microseconds that takes the average microsteps per second and adds a sinusoidal component for pulsatility (sin from 0 to 2*PI in linear increments of i
      //The "-40" terms is to account the ~20 microsecond it takes to write to a pin (each cycle writes a pin HIGH and then LOW and then delays)
      //The input pulsatility is the pulsatility index. Amplitude of sine wave is +/-, so the pulsatility must be multiplied by 0.5 when made the amplitude.
      delaytime[i]=(microsecondsPerStep-40)+((microsecondsPerStep-40)*0.5*pulsatility)*sin(double(i)/(stepsPerSecond-1)*2*3.14159265);
    }
}

//Main loop
void loop() {
  
  buttonStateFwd = digitalRead(FWDV); //Check button state
  buttonStateRev = digitalRead(REVV); //Check button state
  startSwitchState = digitalRead(STSW); //Check switch state
  
  while (buttonStateFwd==HIGH) //While forward button is pressed
    {
      digitalWrite(DIRV, HIGH); //Pull direction pin high to move "forward"
      digitalWrite(STPV,HIGH); //Trigger one step forward
      digitalWrite(STPV,LOW); //Pull step pin low so it can be triggered again
      delay(3); //Delay arbitrary amount (can be made lower for faster movement
      buttonStateFwd = digitalRead(FWDV); // Recheck if button is still pressed
    }
  
  while (buttonStateRev==HIGH) //While reverse button is pressed
    {
      digitalWrite(DIRV, LOW); //Pull direction pin low to move "reverse"
      digitalWrite(STPV,HIGH); //Trigger one step forward
      digitalWrite(STPV,LOW); //Pull step pin low so it can be triggered again
      delay(3); //Delay arbitrary amount (can be made lower for faster movement
      buttonStateRev = digitalRead(REVV); // Recheck if button is still pressed
    }
  
  while (startSwitchState==HIGH) //While start switch is "ON" (stays in loop until full dispense time
    {
      digitalWrite(DIRV, HIGH); //Pull direction pin high to move "forward"
      digitalWrite(MS1V, HIGH); //Set to 1/16th microstepping
      digitalWrite(MS2V, HIGH);
      digitalWrite(MS3V, HIGH);
      
      for (x; x<dispenseTime*60; x++) //For x number of seconds (dispenseTime in minutes times 60 seconds per minutes
        {
          for (int y=0; y<stepsPerSecond; y++) //For each second take y steps
            {
              digitalWrite(STPV,HIGH); //Trigger one step forward
              digitalWrite(STPV,LOW); //Pull step pin low so it can be triggered again
              arr[y]=micros(); //write delay time to verification array
              delayMicroseconds(delaytime[y]); //delay the calculated number of microseconds
            }
        }
    }
  
  for (z; z<stepsPerSecond; z++) //For reading verification (check delay times calculations)
    {
      Serial.println(arr[z]); //Write delay times in serial output for verification if desired
    }
  
  startSwitchState = digitalRead(STSW); //Recheck switch state
  digitalWrite(MS1V, LOW); //Reset to full-step mode
  digitalWrite(MS2V, LOW);
  digitalWrite(MS3V, LOW);
  x=0; //Reset counter
  z=0; //Reset counter
}
