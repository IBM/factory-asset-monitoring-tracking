#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define GPS_ENABLE 5
#define LED 13

// Set frequency channel
#define RF95_FREQ 868.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup()
{
  pinMode(LED, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  pinMode(RFM95_RST, OUTPUT);

  while (!Serial);
  Serial.begin(9600);
  delay(100);

  Serial.println("Feather LoRa TX Test!");

  pinMode(GPS_ENABLE, OUTPUT);
  digitalWrite(GPS_ENABLE, HIGH);

  // Configure serial port for debugging
  Serial1.begin(9600); // GPS serial port
  Serial.begin(9600);
  delay(100);
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: ");
  Serial.println(RF95_FREQ);
  rf95.setFrequency(RF95_FREQ);
  rf95.setTxPower(23, false);
}

void loop()
{
  char radio_data[1000],c=0;
  String incoming_gps_data,year;

  c=0;
  while(c !=10) {
    if(Serial1.available()) {

      // Read data from GPS module
      c=Serial1.read();
      incoming_gps_data+=c;
    }
  }

  if(incoming_gps_data.startsWith("$GPRMC,")) {
      digitalWrite(13, HIGH);

      // Clean data
      incoming_gps_data.replace("$GPRMC,","");
      incoming_gps_data=incoming_gps_data.substring(0,incoming_gps_data.lastIndexOf(','));
      incoming_gps_data=incoming_gps_data.substring(0,incoming_gps_data.lastIndexOf(','));
      incoming_gps_data=incoming_gps_data.substring(0,incoming_gps_data.lastIndexOf(','));
      incoming_gps_data.replace(",,",",0,");

      // Update year to 4 digits
      year = incoming_gps_data.substring(incoming_gps_data.length()-2,incoming_gps_data.length());
      incoming_gps_data=incoming_gps_data.substring(0,incoming_gps_data.length()-2)+"20"+year;
      incoming_gps_data.toCharArray(radio_data, incoming_gps_data.length()+1);

      Serial.println(radio_data);
      Serial.println(incoming_gps_data);

      // Send radio data and wait until buffer cleared
      rf95.send((uint8_t *)radio_data, incoming_gps_data.length()+1);
      rf95.waitPacketSent();

      digitalWrite(13, LOW);
      delay(10000);
    }
}
