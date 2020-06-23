/* calibrating two LPS25 absolute sensors as a single gauge pressure sensor
 * Demo for getting individual unified sensor data from the 2 LPS2X series
 * first version display/ plot differences 
 * second version display 1024 samples for external processing
 * third version specify samples and rate
 * fourth version use long integeers instead of floats.  Note that Adafruit_sensor.h
 *
 * note that the LPS25 has 2 I2C base addresses, 0x5D with no jumper
 * and 0x5C with a jumper between the gnd and sd0 pins
 */

#include <Adafruit_Sensor.h>
#include <Adafruit_LPS2X.h>
#include "Streaming.h" // C++ style output

Adafruit_LPS25 lps, lps2;
Adafruit_Sensor *lps_temp, *lps_pressure, *lps2_temp, *lps2_pressure;

//convert hectoPascals to cm of water
const float hPa2cm = 1.01972;

long nSamples;
int nMsec;
String inStr;
bool extJumper;

void setup(void) {
  pinMode( A0, OUTPUT); // set up to blink at end
  digitalWrite( A0, LOW);
  
  Serial.begin(115200);
  Serial.setTimeout(1000000); // long timeout to avoid skipping input 
  while (!Serial) {
    delay(10); // will pause Zero, Leonardo, etc until serial console opens
  }

  Serial.print( "Does the external sensor have a jumper? (y/n)");
  while( Serial.available() == 0 ) {} // wait for character
  
  inStr = (char)Serial.read();
  extJumper = inStr.startsWith("y");
  //Serial << "<" << in/str << "> length " << inStr.length() << " external jumper: " << extJumper << endl;
  
  // default address is 0x5D
  if (!lps.begin_I2C()) {
    Serial << "Failed to find LPS2X (0x5D) chip\n";
    while (1) {
      delay(10);
    }
  }
  if (!lps2.begin_I2C(0x5C)) {
    Serial <<"Failed to find LPS2X (0x5C) chip\n";
    while (1) {
      delay(10);
    }
  }
  Serial << "Both LPS2X Found!" << endl;

  Serial << "#Samples? ";
  nSamples = Serial.parseInt();
  Serial << "delay msec? ";
  nMsec = Serial.parseInt();
  
  lps_temp = lps.getTemperatureSensor();
  lps_pressure = lps.getPressureSensor();
  lps2_temp = lps2.getTemperatureSensor();
  lps2_pressure = lps2.getPressureSensor();


  Serial << endl;  // fixed an issue while allowing separation
  Serial << "samples " << nSamples << endl;
  Serial << "sampletime " << nMsec <<"e-3" << endl;
#ifdef DEBUG
  lps_temp->printSensorDetails();
  lps_pressure->printSensorDetails();
  lps2_temp->printSensorDetails();
  lps2_pressure->printSensorDetails();
#endif
}

void loop() {
  //  /* Get a new normalized sensor event */
  sensors_event_t pressure, pressure2;
  sensors_event_t temp, temp2;
  long  t; // delay() does not take into account the time in the loop
  int i;
  float startP, stopP, difPressure;
  
  Serial << "Starting pressure? ";
  
  startP = Serial.parseFloat();
  Serial << endl;
  Serial << nSamples << " Samples at " << nMsec << " msec" << endl;
  Serial << "External jumper: " << extJumper << endl;
  Serial << "Starting pressure " << startP << " cm H2O" << endl;
  
  Serial << "n\tP1\tP2\tPdif" << endl;
  for( i = 0; i < nSamples; i++ ) {
    t = millis();
    //lps_temp->getEvent(&temp);
    lps_pressure->getEvent(&pressure);
    //lps2_temp->getEvent(&temp2);
    lps2_pressure->getEvent(&pressure2);
    pressure.pressure *= hPa2cm;
    pressure2.pressure *= hPa2cm;
    
    if (extJumper ) {  // differential pressure is internal - external sensor
      difPressure = pressure.pressure - pressure2.pressure;
    } else {
      difPressure = pressure2.pressure - pressure.pressure;
    }
      
    Serial << i << "\t" << pressure.pressure << "\t" << pressure2.pressure << "\t"  << difPressure << endl;
    while( millis() < t + nMsec) {} // wait until nMsec from start of loop
  }
  Serial << "Ending pressure? ";
  stopP = Serial.parseFloat();
  Serial << endl;
  Serial << "Ending pressure " << stopP << " cm H2O" << endl;
  while(1) { // flash LED for long runs
    digitalWrite(A0, !digitalRead(A0));
    delay(500);
  }
  
}
