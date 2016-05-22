// BY8001 MP3 Player Library, Example Program 1; Sequential Playback Demo 
// Date: June 2015
// Author: borland of Arduino forum 
// No affiliation with Balway Electronics Technology or Shenzhen Electronics Technology Ltd.
// Released in public domain.
// 
// This example sketch uses SoftwareSerial to communnicate with BY8001
// and sequentially plays tracks, & sends track file names to Arduino IDE for display on serial monitor 
//
// Recommend module ABC keypad mode set to '111' in order to support all serial commands 
// Uses Arduino's hardware serial port to connect to PC

// Requires 3.3V TTL level RX/TX such as Ardiuno Mini Pro (8Mhz, 3.3V)
// can use 5V/3.3V level shifter or try 1K ohm resistor on TX line as shown in BY8001 datasheet
// Connect Arduino's RX pin to BY8001's TX pin 3
// Connect Arduino's TX pin to BY8001's RX pin 2
//

#include <SoftwareSerial.h>
//#include <BY8001_2.h>

#define MP3_RX_PIN 10
#define MP3_TX_PIN 9
SoftwareSerial audioSerial(MP3_RX_PIN, MP3_TX_PIN);  // RX, TX
char audioReadBuffer[16];

#define BUTTON_STATE_WAIT 0
#define BUTTON_STATE_DOWN 1
#define BUTTON_STATE_UP 2


static unsigned int mapValueToButton[] = {131, 241, 337, 421, 497, 566, 632, 694, 756, 818, 882, 950};
#define BUTTON_COUNT (sizeof(mapValueToButton)/sizeof(int))
#define BUTTON_VALUE_MARGIN 30

static unsigned char buttonPressed = 0;


unsigned char buttonFromValue(int buttonValue) {

  static unsigned char buttonMap[] = {
    'Y', '0', 'X', '7', '8', '9', '6', '5', '4', '1', '2', '3' 
  };
  
  // Which button is being pressed?
  for(unsigned char i = 0 ; i < BUTTON_COUNT ; i++ ) {
    unsigned int minValue = mapValueToButton[i] - BUTTON_VALUE_MARGIN;
    unsigned int maxValue = mapValueToButton[i] + BUTTON_VALUE_MARGIN;
    if( buttonValue >= minValue && buttonValue <= maxValue ) {
      //return (unsigned char)(i/2 + 0x30);
      return buttonMap[i];
    }
  }
  return 0;
}

typedef enum _app_state_list {
  APP_STATE_IDLE = 0,
  APP_STATE_
} app_state_list;

typedef enum _app_action_list {
  APP_ACTION_KEYPAD,
  APP_ACTION_NEWCALL,
  APP_ACTION_HANGUP,
  APP_ACTION_
} app_action_list;

void applicationStateMachine(unsigned char action,int data)
{
  static unsigned char oldState = APP_STATE_IDLE;
}

void serialDumpHex(unsigned char *ptr,unsigned char len) {
  Serial.print(" hex = ");
  static unsigned char hexTable[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C','D','E','F'};
  for( unsigned char i = 0 ; i < len ; i++ ) {  
    Serial.write(hexTable[(ptr[i] >> 4) & 0xF]);
    Serial.write(hexTable[(ptr[i] >> 0) & 0xF]);
    Serial.print(" ");
  }
  Serial.println(" ");
}

// ---------------------------------------------------------------
// BY8001 Specific Functions

bool audioInitialize() {
  pinMode(MP3_RX_PIN,INPUT);
  pinMode(MP3_TX_PIN,OUTPUT);
  audioSerial.begin(9600);
}

bool audioIsReplyOK(unsigned char dataSize) {
  return (audioReadBuffer[0] == 0x4F &&
  audioReadBuffer[1] == 0x4B &&
  audioReadBuffer[2+dataSize+0] == 0x0D &&
  audioReadBuffer[2+dataSize+1] == 0x0A);
}

bool audioGetFirmware(unsigned short *fwVersion) {
  unsigned char cmd[] = {0x7E, 0x03, 0x14, 0x17, 0xEF};
  audioSerial.write(cmd,sizeof(cmd));
  if( audioSerial.readBytes(audioReadBuffer,6) != 6 ) {
    return false;
  }

  // Expect command reply to be:
  // 4F 4B 32 30 0D 0A      O K 2 0 
  if( !audioIsReplyOK(2) ) {
    return false;
  }

  // We're good.
  audioReadBuffer[4] = '\0';
  *fwVersion = strtoul(audioReadBuffer+2,NULL,16);
  return true;
}

unsigned char getCheckCode(unsigned char *cmd) {
  unsigned char code = 0;
  unsigned char len = cmd[1];
  for(unsigned char i = 1 ; i < len ; i++) {
    code ^= cmd[i];
  }
  return code;
}

bool audioAsyncPlayIndex(unsigned short index) {
  unsigned char cmd[] = {0x7E, 0x05, 0x41, 0x00, 0x01, 0x00, 0xEF};
  cmd[4] = (unsigned char)(index & 0xFF);
  cmd[5] = getCheckCode(cmd);

  serialDumpHex(cmd,sizeof(cmd));

  while ( audioSerial.available() >0 ) audioSerial.read();
  audioSerial.write(cmd,sizeof(cmd));

  if( audioSerial.readBytes(audioReadBuffer,2) != 2 ) {
    return false;
  }

  // Should get "OK"
  if( audioReadBuffer[0] != 'O' ||
    audioReadBuffer[1] != 'K' ) {
    return false;
  }

  return true;
}

bool audioSyncPlayIndex(unsigned short index) {
  if(!audioAsyncPlayIndex(index)) {
    return false;
  }

  // Wait for "STOP"
  if( audioSerial.readBytes(audioReadBuffer,4) != 4 ) {
    return false;
  }

  if( audioReadBuffer[0] != 'S' ||
    audioReadBuffer[1] != 'T' ||
    audioReadBuffer[2] != 'O' ||
    audioReadBuffer[3] != 'P' ) {
      return false;
    }

  // Audio played successfully.
  return true;
}

// ---------------------------------------------------------------

void initializeButtonMatrix() {
  Serial.println("\nButton matrix:");
  for(unsigned char i = 0 ; i < BUTTON_COUNT ; i++ ) {
    unsigned int minValue = mapValueToButton[i] - BUTTON_VALUE_MARGIN;
    unsigned int maxValue = mapValueToButton[i] + BUTTON_VALUE_MARGIN;
    Serial.print(" ");
    Serial.print(i,DEC);
    Serial.print(" ");
    Serial.print(minValue,DEC);
    Serial.print(" ");
    Serial.println(maxValue,DEC);
  }
}

void triggerButtonDown(char button) {
  Serial.print("BUTTON DOWN: ");
  Serial.println(button,HEX);
}

void triggerButtonUp(char button) {
  Serial.print("BUTTON UP: ");
  Serial.println(button,HEX);

  if( button == 'X' ) {
    Serial.println("Button X pressed."); 
  } else if( button == 'Y' ) {
    Serial.println("Button Y pressed."); 
  } else if( button >= '0' && button <= '9' ) {
    //mp3.playTrackByIndexNumber(button-0x30+1);
    if( !audioSyncPlayIndex(1) ) {
      Serial.println("Failed to play index 1");
    }
  }
}

void processButtonRead(int buttonValue) {
  static unsigned char state = 0;
  char button;

  /*Serial.print("processButtonRead (buttonValue=");
  Serial.print(buttonValue,DEC);
  Serial.print(" state=");
  Serial.print(state,DEC);
  Serial.println(")");*/
  
  switch(state)
  {
    case BUTTON_STATE_WAIT:
      if( buttonFromValue(buttonValue) == 0x00 ) {
        // No button pressed, keep state
        return;
      }

      Serial.println("Detected button press...");
      
      state = BUTTON_STATE_DOWN;
      button = buttonFromValue(buttonValue);

      if( buttonPressed != button ) {
        buttonPressed = button;
        triggerButtonDown(button);
      }
      break;
    case BUTTON_STATE_DOWN:
      if( buttonFromValue(buttonValue) == 0x00 ) {
        // button was released
        Serial.println("Detected button release...");
        state = BUTTON_STATE_UP;
        triggerButtonUp(buttonPressed);
        buttonPressed = 0;
        state = BUTTON_STATE_WAIT;
        return;
      }
      // Other button was pressed... ignore
      Serial.println("Detected other button press.. ignoring...");
      break;
  }
}

void setup() {

  Serial.begin(9600);  // set serial monitor baud rate to Arduino IDE
 
  digitalWrite(A0, HIGH);

  initializeButtonMatrix(); 

  delay(1000);  // allow time for BY8001 cold boot; may adjust depending on flash storage size

  // MP3 Module Initialization
  audioInitialize();

  unsigned short fwVersion;
  if( audioGetFirmware(&fwVersion) ) {
    Serial.print("BY8001 module firmware version: ");
    Serial.println(fwVersion,DEC);
  } else {
    Serial.println("Failed to get BY8001 firmware version!");
  }
  
  return;
}

void loop() {
  
  static word totalTime, elapsedTime, playback;
  
  delay(100);

  processButtonRead(analogRead(0) & 0x3FF);
}
