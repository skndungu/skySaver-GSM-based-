#include <SofftwareSerial.h>
SofftwareSerial cell(9,10);   // creates a fake serial port
char phone_no1 [] = "+254712283875";
char phone_no2 [] = "+254717625275";

const int smoke1 = A0;
const int smoke2 = A1;
const int flameSensor1 = A2;
const int relayPin = 2;       // attached to relay in1
const int normalStatus = 3;
const int alert = 4;
const int levelSensor = A3;
const int levelPin = 5;
const int skyLight = 6;  // attached to relay in2
const int alarm = 7;   // activates the alarm for alerting everyone

int smokeVal1 = 0; //variable to store the read smoke value from the first sensor1
int smokeVal2 = 0; // sstores value read from the second sensor
int flameVal = 0;
int level = 0;

void setup()
{
Serial.begin(9600);
cell.begin(9600); // initialise the GSM
delay(3000);     // time to register to network
pinMode(relayPin,OUTPUT);
pinMode(normalStatus,OUTPUT);
pinMode(alert,OUTPUT);
pinMode(skyLight,OUTPUT);
pinMode(alarm,OUTPUT);
}

void loop ()
{
  do
  {
  takeReading(); // fun ction that keeps checking whether there is the presence of fire or not
  checkLevel();  // ckecks the level of the water in the storage
  }

  while(smokeVal1 < 100 || smokeVal2 < 100 || flameVal <100);  // smoke or flame absent
  {
    normalStatus();
  }

  if (smokeVal1 > 100 || smokeVal2 > 100 || flameVal > 100)   // smoke or flame present
  {
    alertSignals();
    PhysicalAlertsOn();  // alarm and skyLight on
    phoneCall();
    putOffFire();  // function to start sprinkling water to the fire

    if (smokeVal1 < 100 || smokeVal2 < 100 || flameVal <100)
    {
      stop(); // function to stop the pump if the fire goes off

    }
    else
    {
      putOffFire(); // function continues to execute as long as smoke is present
      PhysicalAlertsOff(); // alarm and skyLight off
    }
  }

}

void takeReading()
{
  smokeVal1 = analogRead(smoke1);
  smokeVal2 = analogRead(smoke2);
  flameVal = analogRead(flameSensor1);
  level = analogRead(levelSensor);
}

void normalStatus()
{
  digitalWrite(normalStatus,HIGH);
}

void alertSignals()
{
  digitalWrite(alert, HIGH);
  delay(400);
  digitalWrite(alert, LOW);
  delay(400);
}

void putOffFire()
{
  digitalWrite(relayPin,HIGH);
  delay(6000);
  digitalWrite(relayPin,LOW);
  delay(1000);

  if(level < 100)
  {
    sendMessage();
  }
}

void stop()
{
  digitalWrite(relayPin,LOW); // function to put off the pump
}

void checkLevel()
{
  if(level > 100)
  {
    digitalWrite(levelPin,HIGH);  // alert owner
  }
}

void phoneCall ()
{
  cell.begin(9600);
  delay(3000);
  cell.println("AT"); // wakes up the GSM
  cell.print("ATD");  //dial the phone_no using the AT command
  Serial.println(phone_no1);
  Serial.println(";");
  delay(1000);
  Serial.println("ATH"); // after a delay of some sec hang the call
  delay(1000);
  cell.print("ATD");  //dial the phone_no using the AT command
  Serial.println(phone_no2);
  Serial.println(";");
  delay(1000);
  Serial.println("ATH"); // after a delay of some sec hang the call
}

void PhysicalAlertsOn()
{
  digitalWrite(alarm,HIGH);
  digitalWrite(skyLight,HIGH);
}

void PhysicalAlertsOff()
{
  digitalWrite(alarm,LOW);
  digitalWrite(skyLight,LOW);
}

void sendMessage()
{
  cell.println("AT+CMGF = 1"); // sets module in text mode
  delay(1000);
  cell.println("AT+CMGS = \"+254712283875\"\r"); // water level not good do something please as soon as possible
  delay(1000);
  cell.println((char)26); // ASCII code for CTRL+Z
  delay(1000);
}
