# mkrwan1300-hybrid-relay
Hybrid LoRa/LoRaWAN relay code for the Arduino MKRWAN1300.

## Required Libraries
* [LoRa Arduino]("https://github.com/sandeepmistry/arduino-LoRa")
* [MKRWAN]("https://github.com/arduino-libraries/MKRWAN")
 
## Usage
This code is meant for the Arduino MKRWAN 1300. It is designed to receive LoRa messages via a peer-to-peer communication and then transmit those messages over a LoRaWAN network, essentially acting as an extender of LoRa devices that consolidates into a LoRaWAN gateway. 

## Notes/Improvements
* Using serial to communicate locally instead of GPIO.
* Using interrupts for LoRa message receives and serial message receives instead of doing it in the main loop. 
* Timing and antenna configuration optimizations. 
