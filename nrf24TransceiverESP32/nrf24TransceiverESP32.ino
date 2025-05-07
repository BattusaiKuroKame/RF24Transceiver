// ESP32 Transceiver
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// //ARDUINO UNO
// RF24 radio(9, 10);  // CE, CSN (change for ESP32 if needed)

//ESP32
RF24 radio(2, 4);  // CE, CSN (change for ESP32 if needed)

// Address must differ for each pair of nodes (pipe 0 for reading)
const byte address[6] = "00001";

String serialInput = "";

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Initializing nRF24L01...");
  if (!radio.begin()) {
    Serial.println("Radio hardware is not responding!");
    while (1);
  }

  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_1MBPS);
  // radio.setAutoAck(false);
  radio.openWritingPipe(address);
  radio.openReadingPipe(0, address);
  radio.startListening();

  Serial.println("Ready. Type '# your_message' to transmit.");
}

void loop() {
  // 1. Check for received messages
  if (radio.available()) {
    char received[32] = "";
    radio.read(&received, sizeof(received));
    Serial.print("Received: ");
    Serial.println(received);
  }

  // 2. Check for serial input
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      processSerialCommand();
      serialInput = "";
    } else {
      serialInput += c;
    }
  }
}

void processSerialCommand() {
  serialInput.trim();
  if (serialInput.startsWith("# ")) {
    String message = serialInput.substring(2);
    if (sendMessage(message)) {
      Serial.println("Message sent successfully: " + (message.length() > 5 ? message.substring(0, 5) + "....." : message));
    } else {
      Serial.println("Message was not acknowledged.");
    }
  } else {
    Serial.println("Invalid command. Use '#send your_message'");
  }
}

bool sendMessage(String message) {
  radio.stopListening();  // Switch to transmit mode

  char buffer[32];
  message.toCharArray(buffer, sizeof(buffer));

  bool success = radio.write(&buffer, sizeof(buffer));

  radio.startListening();  // Return to receive mode
  return success;
}
