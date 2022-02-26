//INPUTS
double fluidVelocity = 16.0; // in cm/s
double pulsatility = 0.5; // Pulsatility Index in decimal form
int dispenseTime = 5; // in minutes

//CALCULATIONS
const int stepsPerSecond = (fluidVelocity*0.0125*0.0125)/(0.6225*0.6225)*10000/0.25; // Calculate steps per second relating a 125 um radius tube to a 6.225 mm radius syringe (5mL)
//with 1/16th micro step (0.25 is the number of um per microstep) and conversion magnitude factor
const int microsecondsPerStep = 1000000/double(stepsPerSecond); //Invert steps per second to get seconds per step (with conversion to make microseconds)

//Declare pin functions on Arduino
#define STPA 51 //Step Pin
#define DIRA 53 //Direction Pin
#define MS1A 27 //Microstep Channel 1 Pin
#define MS2A 29 //Microstep Channel 2 Pin
#define MS3A 31 //Microstep Channel 3 Pin
#define ENA 25 //Enable Pin
#define FWDA 35 //Forward Manual Button Pin
#define REVA 37 //Reverse Manual Button Pin
#define STSW 22 //Manual Start Switch Pin

//Define Variable
int x=0; //counter
int z=0; //counter
double delaytime[500]; //array to hold a variable delaytime (up to 500 steps per second)
unsigned long arr[500]; //array to read a variable delaytime if verification is desired
int buttonStateFwd = 0; //varaiable to read if forward button is pressed
int buttonStateRev = 0; //variable to read if reverse button is pressed
int startSwitchState = 0; //variable to read if switch is on or off

void setup() {

  Serial.begin(9600); //Open Serial connection
  
  //Define all control pins as output and reading pins as input
  pinMode(STPA, OUTPUT);
  pinMode(DIRA, OUTPUT);
  pinMode(MS1A, OUTPUT);
  pinMode(MS2A, OUTPUT);
  pinMode(MS3A, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(FWDA, INPUT);
  pinMode(REVA, INPUT);
  pinMode(STSW, INPUT);
  
  digitalWrite(ENA, LOW); // Enable Motor
  digitalWrite(DIRA, HIGH); //Set to move forward
  digitalWrite(MS1A, LOW); //Set to Full-step Microstepping Mode
  digitalWrite(MS2A, LOW);
  digitalWrite(MS3A, LOW);
  
  for (int i=0; i<stepsPerSecond; i++) //make an array where number of elements = number of steps per second
    {
      //Delay time in microseconds that takes the average microsteps per second and adds a sinusoidal component for pulsatility (sin from 0 to 2*PI in linear increments of i
      //The "-40" terms is to account the ~20 microsecond it takes to write to a pin (each cycle writes a pin HIGH and then LOW and then delays)
      //The input pulsatility is the pulsatility index. Amplitude of sine wave is +/-, so the pulsatility must be multiplied by 0.5 when made the amplitude.
      delaytime[i]=(microsecondsPerStep-40)+((microsecondsPerStep-40)*0.5*pulsatility)*sin(double(i)/(stepsPerSecond-1)*2*3.14159265);
    }
}

//Main loop
void loop(){
  
  buttonStateFwd = digitalRead(FWDA); //Check button state
  buttonStateRev = digitalRead(REVA); //Check button state
  startSwitchState = digitalRead(STSW); //Check switch state
  
  while (buttonStateFwd==HIGH) //While forward button is pressed
    {
      digitalWrite(DIRA, HIGH); //Pull direction pin high to move "forward"
      digitalWrite(STPA,HIGH); //Trigger one step forward
      digitalWrite(STPA,LOW); //Pull step pin low so it can be triggered again
      delay(3); //Delay arbitrary amount (can be made lower for faster movement
      buttonStateFwd = digitalRead(FWDA); // Recheck if button is still pressed
    }
  
  while (buttonStateRev==HIGH) //While reverse button is pressed
    {
      digitalWrite(DIRA, LOW); //Pull direction pin low to move "reverse"
      digitalWrite(STPA,HIGH); //Trigger one step forward
      digitalWrite(STPA,LOW); //Pull step pin low so it can be triggered again
      delay(3); //Delay arbitrary amount (can be made lower for faster movement
      buttonStateRev = digitalRead(REVA); // Recheck if button is still pressed
    }
  
  while (startSwitchState==HIGH) //While start switch is "ON" (stays in loop until full dispense time)
    {
      digitalWrite(DIRA, HIGH); //Pull direction pin high to move "forward"
      digitalWrite(MS1A, HIGH); //Set to 1/16th microstepping
      digitalWrite(MS2A, HIGH);
      digitalWrite(MS3A, HIGH);
      
      for (x; x<dispenseTime*60; x++) //For x number of seconds (dispenseTime in minutes times 60 seconds per minutes
        {
          for (int y=0; y<stepsPerSecond; y++) //For each second take y steps
            {
              digitalWrite(STPA,HIGH); //Trigger one step forward
              digitalWrite(STPA,LOW); //Pull step pin low so it can be triggered again
              arr[y]=micros(); //write delay time to verification array
              delayMicroseconds(delaytime[y]); //delay the calculated number of microseconds
            }
        }
      
      for (z; z<stepsPerSecond; z++) //For reading verification
        {
          Serial.println(arr[z]); //Write delay times in serial output for verification if desired
        }
      
      startSwitchState = digitalRead(STSW); //Recheck switch state
    }
  
  digitalWrite(MS1A, LOW); //Reset to full-step mode
  digitalWrite(MS2A, LOW);
  digitalWrite(MS3A, LOW);
  x=0; //Reset counter
  z=0; //Reset Counter
}
