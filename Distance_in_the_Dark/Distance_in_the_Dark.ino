// Photocell on analogue pin A0.
#define photocell A5
// The Ultrasonic Trigger pin.
#define USTrigger 2
// The Ultrasonic Echo pin.
#define USEcho 3
// The LED which indicates the ambient lightlevel has been taken.
#define WarningLEDLow 4
// The LED that indicates the room is dark and still, compared to ambient levels.
#define WarningLEDMid 5
// The LED that indicates the room is dark and still, compared to ambient levels.
#define WarningLEDHigh 6
// The LED which indicates the ambient lightlevel has been taken.
#define ambientLED 7

// Defined the length of the queue (longer => more latency, but more smoothing).
#define queueLength 10

//Creating globals to be used in most functions below.
double lightVal, sum, ambientLightLevel, counter = 0, filteredVal, queue[queueLength], movementTimer = 0;
//The scale is how much we weigh our previous result vs. our current result.
double scaleVal = 0.95;
//Set up variables to check the round trip time and distance of the ultrasonic sensor.
double echoTime, echoDist;

//Using queue to smooth the signal ever more.
void queuePush(double nextVal) {
  for (int i = queueLength - 1; i > 0; i--) {
    queue[i] = queue[i - 1];
  }
  queue[0] = nextVal;
}


void setup() {
  
  //Set up serial comms at 9600 Baud
  Serial.begin(9600);
  
  // Set I/O state of pins
  pinMode(ambientLED, OUTPUT);
  pinMode(WarningLEDHigh, OUTPUT);
  pinMode(WarningLEDMid, OUTPUT);
  pinMode(WarningLEDLow, OUTPUT);
  pinMode(USTrigger, OUTPUT);
  pinMode(USEcho, INPUT);
  pinMode(photocell, INPUT);

  //Initialise the light level and queue
  ambientLightLevel = 1023;
  for (int i = 0; i < queueLength; i++) {
    queue[i] = 300;
  }

  //Get ambient light level over 250 cycles
  while (counter++ < 250) {
    
    //Get the sensor value and filter it
    lightVal = analogRead(photocell);
    filteredVal = scaleVal * queue[0] + (1 - scaleVal) * lightVal;
    queuePush(filteredVal);
    
    //Get the sum of the past "queueLength" values
    for (int i = 0; i < queueLength; i++) {
      sum += queue[i];
    }
    sum = sum / queueLength;
    //Print the current signal
    //Serial.println(sum);
  }

  //Set the Ambient light level and flash the ambient indicator LED
  ambientLightLevel = sum;
  //Serial.println(sum);
  digitalWrite(ambientLED, HIGH);
  delay(200);
  digitalWrite(ambientLED, LOW);
  digitalWrite(USTrigger, LOW);
}


//Read the photoresistor and output to LED & serial monitor.
void loop() {
  //Check the photocell over a period of time
  for(int cycleCount = 0; cycleCount < 15; cycleCount++){
    //Get the sensor value and filter it
    lightVal = analogRead(photocell);
    filteredVal = scaleVal * queue[0] + (1 - scaleVal) * lightVal;
    queuePush(filteredVal);
  }
  
  //Get the sum of the past "queueLength" values
  for (int i = 0; i < queueLength; i++) {
    sum += queue[i];
  }
  sum = sum / queueLength;
    
  //Print the current signal & ratio
  //Serial.print(sum);
  //Serial.print(" ");
  //Serial.println(ambientLightLevel / sum);


  //If light level is below the threshold, then LED set to ON
  if (ambientLightLevel / sum > 1.015) {
    digitalWrite(USTrigger, HIGH);
    delayMicroseconds(10);
    digitalWrite(USTrigger, LOW);
    // Read the rebounded soundwave, convert the round trip time to seconds using speed of sound
    echoTime = pulseIn(USEcho, HIGH);
    // Calculating the distance
    echoDist = echoTime*0.034/2;
    
//    Serial.println(echoDist);
    //Warning level: RED
    if(echoDist < 10){
      digitalWrite(WarningLEDLow, LOW);
      digitalWrite(WarningLEDMid, LOW);
      digitalWrite(WarningLEDHigh, HIGH);
    }
    //Warning level: YELLOW
    else if(echoDist < 20){
      digitalWrite(WarningLEDLow, LOW);
      digitalWrite(WarningLEDHigh, LOW);
      digitalWrite(WarningLEDMid, HIGH);
    }
    //Warning level: GREEN
    else{
      digitalWrite(WarningLEDMid, LOW);
      digitalWrite(WarningLEDHigh, LOW);
      digitalWrite(WarningLEDLow, HIGH);
    }
  }
  else {
    //This is to turn off all LEDs if it is not dark
    digitalWrite(WarningLEDLow, LOW);
    digitalWrite(WarningLEDMid, LOW);
    digitalWrite(WarningLEDHigh, LOW);
  }
}
