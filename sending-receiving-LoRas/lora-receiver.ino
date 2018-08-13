#include <SPI.h>
#include <RH_RF95.h>

#define LED 13
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3

// Set frequency channel
#define RF95_FREQ 868.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup()
{
  pinMode(LED, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);

  // Configure serial port for debugging
  Serial1.begin(9600); // GPS serial port
  delay(100);
  Serial.begin(9600);
  delay(100);

  Serial.println("> Feather LoRa RX");

  // Reset the radio
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  //
  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: ");
  Serial.println(RF95_FREQ);

  // Set frequency, power and slow-long range
  rf95.setFrequency(RF95_FREQ);
  rf95.setTxPower(23, false);
}

void loop()
{
  // Check radio availability
  if (rf95.available())
  {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    // Start receiving messages
    if (rf95.recv(buf, &len))
    {
      digitalWrite(LED, HIGH);
      Serial.println((char*)buf);
      delay(10);
      digitalWrite(LED, LOW);
    }
     else
    {
      Serial.println("Receive failed");
    }
  }
}
