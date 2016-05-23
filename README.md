# Blindmobile

Simple and cheap mobile phone for elderly and visually impaired persons. It has 12 buttons, 10 for the digits and two for phone call control. It has one switch for mobile on/off. Due to it's target audience, it's adapted to produce voice to make it easier to use. For example, whenever a button is pressed it "says" what button was pressed and if it's currently typing a phone number it "says" the complete number typed.

 * Voice buttons.
 * Voice status.
 * Quick number call (press single digit button + call button).
 * Conventional AA cells for power source, can use rechargable ones.

# Hardware

First idea was to make this project really fast so I had to re-use already existing modules. It shouldn't be hard to build a PCB with all these modules together, it would reduce alot the footprint of the mobile but who knows in the future.

* Arduino Nano for the brain (ATmega328, 32KB code).
* SIM900A for GSM module.
* PCB antenna with IPEX connector.
* BY8001 for MP3 player module.
* Micro SDCard.
* LMXXX OpAmp module for audio mixing.
* Doubleside Prototype PCB 5x7 for keypad.
* 10 PCB switches 12x12mm with rounded caps.
* 2 PCB switches 12x12mm with squared caps.
* 1 DPDT switch small footprint, PCB pins.

# Firmware

The software was written using the Arduino IDE. To communicate with GSM, conventional AT commands are used in a Software Serial connection.  **TODO:** Finish this document..
