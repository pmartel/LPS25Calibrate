#include <Adafruit_Sensor.h>

// Demo for getting individual unified sensor data from the 2 LPS2X series
// first version display/ plot differences 
// second version display 1024 samples for external processing
// third version specify samples and rate
// fourth version use long integeers instead of floats.  Note that Adafruit_sensor.h
    // defines pressure and temperature as floats anyhow.  Scaling for floats for reference is 
    // pressure->pressure = (unscaled_pressure / 4096.0);
    // temp->temperature = (unscaled_temp / 480) + 42.5;



#include <Adafruit_LPS2X_int.h>
#include "Streaming.h" // C++ style output

Adafruit_LPS2X_int lps, lps2;
Adafruit_Sensor *lps_temp, *lps_pressure, *lps2_temp, *lps2_pressure;

long nSamples;
int nMsec;
String inStr;

void setup(void) {
  pinMode( A0, OUTPUT); // set up to blink at end
  digitalWrite( A0, LOW);
  
  Serial.begin(115200);
  while (!Serial) {
    delay(10); // will pause Zero, Leonardo, etc until serial console opens
  }

  Serial << "version 1 skip 3" << endl;
  Serial << "Dual Adafruit LPS2X data samples.  Save for external processing.\n";
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
  Serial.setTimeout(60000);
  Serial << "#Samples? ";
  nSamples = Serial.parseInt();
  Serial << "delay msec? ";
  nMsec = Serial.parseInt();
  
  
  lps_temp = lps.getTemperatureSensor();
  lps_pressure = lps.getPressureSensor();
  lps2_temp = lps2.getTemperatureSensor();
  lps2_pressure = lps2.getPressureSensor();


  Serial << "Both LPS2X Found!" << endl;
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
  long i;

  //Serial << endl;
  //Serial << nSamples << " Samples at " << nMsec << " msec" << endl;
  Serial << "n\tP1\tP2\tT1\tT2" << endl;
  for( i = 0; i < nSamples; i++ ) {
    t = millis();
    lps_temp->getEvent(&temp);
    lps_pressure->getEvent(&pressure);
    lps2_temp->getEvent(&temp2);
    lps2_pressure->getEvent(&pressure2);
    Serial << i << "\t" << (long)pressure.pressure << "\t" << (long)pressure2.pressure << "\t" << (long)temp.temperature << "\t" << (long)temp2.temperature << endl;
    while( millis() < t + nMsec) {} // wait until nMsec from start of loop
  }
  while(1) { // flash LED for long runs
    digitalWrite(A0, digitalRead(A0) ^ 1);
    delay(500);
  }
  
}
