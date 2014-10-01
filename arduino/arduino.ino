//
// This file is part of DoorOpener.
// Copyright (c) 2014 Jacob Dawid <jacob@omg-it.works>
//
// DoorOpener is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// DoorOpener is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public
// License along with DoorOpener.
// If not, see <http://www.gnu.org/licenses/>.
//

#define RELAIS_DOOR_OUT_1 7
#define RELAIS_DOOR_OUT_2 8
#define BELL_IN 2
#define BEEP_OUT 13

#define BUFFER_SIZE 256

char *readBuffer = (char*)malloc(sizeof(char) * BUFFER_SIZE);
int readBufferPosition = 0;

void setup() {
  // Set up serial port connection
  Serial.begin(9600);
  
  // Set pin IO modes
  pinMode(RELAIS_DOOR_OUT_1, OUTPUT);
  pinMode(RELAIS_DOOR_OUT_2, OUTPUT);
  pinMode(BELL_IN, INPUT);
  pinMode(BEEP_OUT, OUTPUT);

  // Turn off door opener
  digitalWrite(RELAIS_DOOR_OUT_1, LOW);
  digitalWrite(RELAIS_DOOR_OUT_2, LOW);
}

void loop() {
  while(Serial.available() > 0) {
    // Read data from serial port if available
    char c = Serial.read();
    
    // If line ends or maximum buffer size reached, handle line
    if(c == '\n' || c == '\r' || (readBufferPosition == BUFFER_SIZE - 1)) {
      readBuffer[readBufferPosition] = '\0';
      handleInputLine();
      readBufferPosition = 0;
    // Otherwise, append character
    } else {
      readBuffer[readBufferPosition] = c;
      readBufferPosition++;
    }
  }
  
  // Check if bell rings at bell input pin
  if(digitalRead(BELL_IN) == HIGH) {
    //Serial.println("bell");
  }
}

// Handles a server command
void handleInputLine() {
  Serial.println("ACK");
  if (readBuffer[0] == '1') {
      digitalWrite(RELAIS_DOOR_OUT_1, HIGH);
  } else if (readBuffer[0] == '0') {
      digitalWrite(RELAIS_DOOR_OUT_1, LOW);
  }
  
  if (readBuffer[1] == '1') {
      digitalWrite(RELAIS_DOOR_OUT_2, HIGH);
  } else if (readBuffer[1] == '0') {
      digitalWrite(RELAIS_DOOR_OUT_2, LOW);
  }
}
