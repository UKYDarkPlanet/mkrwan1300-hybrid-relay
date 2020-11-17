#include <MKRWAN.h>
#include <SPI.h>
#include <LoRa.h>

LoRaModem modem;

#define ACK 0x24
int emergency_pin = 1;
int connected = 0;
volatile bool has_serial = true;
String station_name = "S1";

void setup() {
  Serial.begin(115200);
  while(!Serial);
  pinMode(emergency_pin, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  if(!LoRa.begin(915E6)) {
    Serial.println("LoRa init failed");
    while(1);
  }
  LoRa.setSyncWord(0x34);
  Serial.println("LoRa init succeeded");
  
}

void loop() {
    // ================ Continuously look for LoRa transmissions from clients ================
    if(digitalRead(emergency_pin) == HIGH) {
    Serial.println("Looking for packet");
    int pack_size = LoRa.parsePacket();
    if (pack_size) {
      String outgoing = "";
      Serial.println("Received Packet: ");
      while(LoRa.available()) {
        outgoing += (char)LoRa.read();
      }
      // node packet acknowledgement
      byte node_id = outgoing.charAt(0);
      byte seq_num = outgoing.charAt(1);
      Serial.println(outgoing);
      LoRa.beginPacket();
      LoRa.write(ACK);
      LoRa.write(node_id);
      LoRa.write(seq_num);
      LoRa.endPacket(true);
      LoRa.end();

      // ================ Begin send to LoRaWAN ================
      modem.begin(US915);
      modem.init();
      modem.minPollInterval(60);

      Serial.println("Rejoining the network");
      connected = modem.joinOTAA("aee3a8f14e9478d8", "4c194e20d396b5f7d3e1551e4cd320de");
      while(!connected) {
        Serial.println("I keep trying");
        connected = modem.joinOTAA("aee3a8f14e9478d8", "4c194e20d396b5f7d3e1551e4cd320de");
      }
      
      delay(1000); // setup delay
      bool ack = false;
      int tries = 10;
      while(!ack && tries > 0) {
        modem.beginPacket();
        modem.print(outgoing);
        //modem.print(outgoing);
        Serial.println(modem.endPacket(true));
        char rcv[64];
        int i = 0;
        delay(4000);
        while(modem.available()) {
          rcv[i++] = (char)modem.read();
        }
        if (i > 0) { // Node RED is set up to send a GPSACK message on  recieve of good GPS data.
            Serial.print("Received: ");
            for (unsigned int j = 0; j < i; j++) {
              Serial.print(rcv[j] >> 4, HEX);
              Serial.print(rcv[j] & 0xF, HEX);
              Serial.print(" ");
            }
            Serial.println();
            ack = true;
        }
        tries--;
        delay(1000);
      }
      modem.dumb();
      connected = 0;
      // ================ End LoRaWAN Transmission ================
      // ================ Restart LoRa Receiver ================
      if(!LoRa.begin(915E6)) {
        Serial.println("LoRa init failed");
        while(1);
      }
      LoRa.setSyncWord(0xF3);
      Serial.println("LoRa init succeeded");
    } 
    } else { 
      Serial.println("Received Emergency");
      // ================ Begin send to LoRaWAN ================
      modem.begin(US915);
      modem.init();
      modem.minPollInterval(60);
    
      while(!connected) {
        Serial.println("Rejoining the network");
        connected = modem.joinOTAA("aee3a8f14e9478d8", "4c194e20d396b5f7d3e1551e4cd320de");
      }    
      delay(1000); // setup delay
      bool ack = false;
      int tries = 10;
      while(!ack && tries > 0) {
        modem.beginPacket();
        modem.print("EM");
        modem.print(station_name);
        //modem.print(outgoing);
        Serial.println(modem.endPacket(true));
        char rcv[64];
        int i = 0;
        delay(4000);
        while(modem.available()) {
          rcv[i++] = (char)modem.read();
        }
        if (i > 0) { // Node RED is set up to send a GPSACK message on  recieve of good GPS data.
            Serial.print("Received: ");
            for (unsigned int j = 0; j < i; j++) {
              Serial.print(rcv[j] >> 4, HEX);
              Serial.print(rcv[j] & 0xF, HEX);
              Serial.print(" ");
            }
            Serial.println();
            ack = true;
        }
        tries--;
        delay(1000);
      }
      modem.dumb();
      if(!LoRa.begin(915E6)) {
        Serial.println("LoRa init failed");
        while(1);
      }
      LoRa.setSyncWord(0xF3);
      Serial.println("LoRa init succeeded");
    }
    delay(250);
}
