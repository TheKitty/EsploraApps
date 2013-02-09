/*
  Esplora Test - assumes you have a color display attached (Adafruit 1.8" breakout or compatible) 

  This sketch allows to test the Esplora's peripherals 
  
  Created on January 18, 2013  Mike Barela http://21stdigitalhome.blogspot.com/
  Please keep attribution in derivative works, use in noncommercial and commercial
  products is granted
Per Adafruit MIT license, following comments included
***************************************************
  This is an example sketch for the Adafruit 1.8" SPI display.
  This library works with the Adafruit 1.8" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/358
   
  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************
*/

// These definitions map display functions to the Adduino Esplora display header pins
#define sclk 15
#define mosi 16
#define cs   7
#define dc   0
#define rst  1
 
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>             // SPI communications library
#include <Esplora.h>         // Arduino Esplora specific library

// Option 1: explicitly note SPI pins - this mayes display writes noticably slower
//Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, mosi, sclk, rst);

// Option 2: must use the hardware SPI pins
// This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, rst);

/*
  This array holds the last sensed state of each of the buttons read.
  Later in the code, you'll read the button states, and compare them
  to the previous states that are stored in this array. If the two
  states are different, it means that the button was either pressed or released.
 */
boolean buttonStates[8];

/*
  This array holds the names of the buttons being read.
  Later in the sketch, you'll use these names with
  the method Esplora.readButton(x), where x is one of these buttons.
 */
const byte buttons[] = {
  JOYSTICK_DOWN,
  JOYSTICK_LEFT,
  JOYSTICK_UP,
  JOYSTICK_RIGHT,
  SWITCH_RIGHT, 
  SWITCH_LEFT, 
  SWITCH_UP, 
  SWITCH_DOWN, 
};

#define sound_pin 6 // Direct sound on Esplora (not currently used but you can use with tone() function)
                    // instead of using Esplora.tone

void setup() {
  // Adafruit's supplier changed the 1.8" display slightly after Jan 10, 2012
  // so that the alignment of the TFT had to be shifted by a few pixels
  // this just means the init code is slightly different. Check the
  // color of the tab to see which init code to try. If the display is
  // cut off or has extra 'random' pixels on the top & left, try the
  // other option!

  // If your TFT's plastic wrap has a Red Tab, use the following:
  tft.initR(INITR_REDTAB);   // initialize a ST7735R chip, red tab
  // If your TFT's plastic wrap has a Green Tab, use the following:
  //tft.initR(INITR_GREENTAB); // initialize a ST7735R chip, green tab
  tft.setRotation(1);  // Set for landscape display on Esplora
  tft.setTextWrap(false); // Allow text to run off right edge
}

void loop() {
  int S1, S2, S3;   // holds values of switches 1 to 3
  DisplaySplash();  // Display splash page and main menu
  S1=HIGH; S2=HIGH; S3=HIGH;
  while(S1==HIGH && S2==HIGH && S3==HIGH) {  // keep reading buttons until one is pressed 
     S1 = Esplora.readButton(SWITCH_1);
     S2 = Esplora.readButton(SWITCH_2);  
     S3 = Esplora.readButton(SWITCH_3);
     if(S1==LOW) 
       TestOutputs();  // if Switch 1 is pressed, test outputs
     else if(S2==LOW) 
       TestLED();      // if switch 2 is [ressed, do LED mixer
     else if(S3==LOW)
       TestSound();    // if switch 3 is pressed, do buzzer test
  }  // end while (breaks out if button presed and routine done (meaning go back to splash page)
}

void DisplaySplash() {   // display first screen which is also the main menu
  tft.fillScreen(ST7735_BLACK);
  tft.setTextSize(2);
  displayString(0, 0,"Welcome to",ST7735_GREEN);
  displayString(0,16," Arduino", ST7735_GREEN); 
  displayString(0,32,"  Esplora", ST7735_GREEN);
  tft.setTextSize(1); 
  delay(500);
  displayString(0,61,"Press a button:",ST7735_WHITE);
  displayString(0,78,"- Switch 1 to test outputs", ST7735_WHITE);
  displayString(0,91,"- Switch 2 to test LED", ST7735_WHITE);
  displayString(0,104,"- Switch 3 to test sound", ST7735_WHITE);
}

void TestLED() {  // text RGB LED on Esplora interactively
  int red, green, blue;                 // valid LED values are 0-255
  int oldred=0, oldgreen=0, oldblue=0;  // values to see if a value changed (don't write to screen unless change to avoid flicker)
  red=128; green=128; blue=128;         // initially set to mid-white
  Esplora.writeRGB(red, green, blue);   // Set LED initially in center which is moderate white
  tft.fillScreen(ST7735_BLACK);         // clear screen
  displayString(0,0,"EsploraTest - RGB LED Test",ST7735_GREEN);
  displayString(0,14,"Joystick U/D is +- Red",ST7735_WHITE);
  displayString(0,24,"Joystick L/R is +- Green",ST7735_WHITE);
  displayString(0,34,"Slider L/R is +- Blue",ST7735_WHITE);
  
  displayString(0,50,"Red: ",ST7735_RED);
  displayString(0,70,"Green: ", ST7735_GREEN);
  displayString(0,90,"Blue: ", ST7735_BLUE);
  displayString(0,107,"Press down on joystick", ST7735_YELLOW);
  displayString(0,117," when done",ST7735_YELLOW);

  while(Esplora.readJoystickButton()==HIGH) {  // keep displaying values until joystick clicked down  
 
     if(Esplora.readJoystickY() > 20)   // if the joystick is moved significantly down
       if(red > 0) red--;
     if(Esplora.readJoystickY() < -20)  // if the joystick is moved significantly up
       if(red < 255) red++;
     if(Esplora.readJoystickX() > 20)   // if the joystick is moved significantly left
       if(green > 0) green--;
     if(Esplora.readJoystickX() < -20)  // if the joystick is moved significantly right
       if(green < 255) green++;
     blue=map(Esplora.readSlider(), 1024, 0, 0, 255);  // Map slider to 0-255 range for blue
 
     if(red!=oldred) {
        displayInt2(red,   40, 50, ST7735_RED,   ST7735_BLACK);
        oldred=red;
     }
     if(green!=oldgreen) {
        displayInt2(green, 40, 70, ST7735_GREEN, ST7735_BLACK);
        oldgreen=green;
     }
     if(blue!=oldblue) {
        displayInt2(blue,  40, 90, ST7735_BLUE,  ST7735_BLACK);
        oldblue=blue;
     }
     Esplora.writeRGB(red, green, blue);  // set LED color
     delay(35);  // wait a short time
  }
}

void TestSound() {  // text Sound "buzzer" on Esplora interactively
  unsigned int freq;
  unsigned int oldfreq;  // value to see if a value changed (don't write to screen unless change to avoid flicker)
  int state=0;
  freq=0;  oldfreq=0;
  Esplora.noTone(); // ensure tone is off
  tft.fillScreen(ST7735_BLACK);  // clear screen
  displayString(0,0,"EsploraTest - Buzzer",ST7735_GREEN);
  displayString(0,14,"Use slider to vary",ST7735_WHITE);
  displayString(0,24,"the sound frequency",ST7735_WHITE);
  displayString(0,34,"Switch 1 turns sound off",ST7735_WHITE);
  
  displayString(0,60,"Frequency: ", ST7735_GREEN);
  displayString(0,107,"Press down on joystick", ST7735_YELLOW);
  displayString(0,117," when done",ST7735_YELLOW);

  while(Esplora.readJoystickButton()==HIGH) {  // keep displaying values until joystick clicked down  
 
     freq=map(Esplora.readSlider(), 1024, 0, 0, 10000); // Map slider to 0-10,000 hz
 
     if(abs(freq-oldfreq)>=30) {  // try to smooth out changes, still noisy, don't know why
        displayInt2(freq, 70, 60, ST7735_GREEN, ST7735_BLACK); // display frequency value
        oldfreq=freq;
        state=1;  // slider change - we want a sound
        if(state==1) Esplora.tone(freq,5000);  // play sound for 5 seconds
     }
     if(Esplora.readButton(SWITCH_1)==LOW && state==1) {
        Esplora.noTone();  // switch activated, no sound wanted
        state=0;
     }
   }
   Esplora.noTone();   // turn off sound when you exit test
}

void TestOutputs() {
  
     // declare variables.  values and old values used to test if a value changes.  Don't
     unsigned int slidervalue = 0, oldslidervalue=9999; // update a display value if the
     unsigned int lightvalue=0, oldlightvalue=9999;     // variable is the same as the previous
     unsigned int joybuttonvalue=0, oldjoybuttonvalue=9999; // one through a button poll loop
     unsigned int micvalue;
     int celsius, old_c=-999;
     int fahrenheit, old_f=-999;
     int x_axis, y_axis, z_axis;  // accelerometer read values
     int oldx=0, oldy=0, oldz=0;  // accelerometer stored values
   // Draw fixed screen elements first and do not redraw to avoid flicker
     tft.fillScreen(ST7735_BLACK);
     // Display fixed text values
     displayString(0,0,"Esplora Test - Outputs", ST7735_GREEN);  // Colors defined in include file
     displayString(0,14,"Buttons:", ST7735_WHITE);
     displayString(0,28,"Slider:",ST7735_WHITE);
     displayString(77,28,"Light:",ST7735_WHITE);
     displayString(0,42,"Joystick:",ST7735_WHITE);
     displayString(0,56,"Microphone:",ST7735_WHITE);
     displayString(0,70,"Temp:",ST7735_WHITE);
     displayString(0,84,"Accelerometer X   Y   Z",ST7735_WHITE);
     displayString(0,107,"Press down on joystick", ST7735_YELLOW);
     displayString(0,117," when done",ST7735_YELLOW);

  while(Esplora.readJoystickButton()==HIGH) {  // keep displaying values until joystick clicked down  
 
   // for controller output, Iterate through all the buttons:
   for (byte thisButton=0; thisButton<8; thisButton++) {
    boolean lastState = buttonStates[thisButton];
    boolean newState = Esplora.readButton(buttons[thisButton]);
    if (lastState != newState) { // Something changed!
      switch(thisButton) {
        case 0:
          if(newState==1) 
            displayString(56,42,"Down",ST7735_WHITE); 
          else 
            displayString(56,42,"Down",ST7735_GREEN);
          break;
        case 1:
         if(newState==1) 
            displayString(100,42,"Left",ST7735_WHITE); 
          else 
            displayString(100,42,"Left",ST7735_GREEN);
            break;
        case 2:
          if(newState==1) 
            displayString(83,42,"Up",ST7735_WHITE); 
          else 
            displayString(83,42,"Up",ST7735_GREEN); 
          break;
        case 3:
         if(newState==1) 
            displayString(130,42,"Right",ST7735_WHITE); 
          else 
            displayString(130,42,"Right",ST7735_GREEN);
          break;
        case 4:
          if(newState==1) 
            displayString(104,14,"S4",ST7735_WHITE); 
          else 
            displayString(104,14,"S4",ST7735_GREEN);
          break;
        case 5:
          if(newState==1) 
            displayString(68,14,"S2",ST7735_WHITE); 
          else 
            displayString(68,14,"S2",ST7735_GREEN);
          break;
        case 6:
          if(newState==1) 
            displayString(86,14,"S3",ST7735_WHITE); 
          else 
            displayString(86,14,"S3",ST7735_GREEN);
          break;
        case 7:
          if(newState==1) 
            displayString(50,14,"S1",ST7735_WHITE); 
          else 
            displayString(50,14,"S1",ST7735_GREEN);
          break;
      } 
    }
    // Store the new button state, so you can sense a difference later:
    buttonStates[thisButton] = newState;
  }
  joybuttonvalue = Esplora.readJoystickSwitch();
  if(joybuttonvalue) {  // Joystick depressed
   //  displayString(0,42,"Joystick:",ST7735_GREEN); use for screen switch?
  }
  else {
  }  
  slidervalue = Esplora.readSlider();
  if(slidervalue != oldslidervalue) {   // Slider changed
     displayInt(slidervalue,39,28,ST7735_WHITE,ST7735_BLACK);
     oldslidervalue=slidervalue;  // save for next time
  }
  lightvalue = Esplora.readLightSensor();
  if(lightvalue!=oldlightvalue)  {  // light sensor changed
     displayInt(lightvalue,110,28,ST7735_WHITE,ST7735_BLACK);
     oldlightvalue=lightvalue;
  }
  micvalue = Esplora.readMicrophone();
  displayInt2(micvalue,70,56,ST7735_WHITE,ST7735_BLACK);
  
  celsius = Esplora.readTemperature(DEGREES_C);
  fahrenheit = Esplora.readTemperature(DEGREES_F);
  if(celsius!=old_c) {
     displayInt(celsius,29,70,ST7735_WHITE,ST7735_BLACK);        // display only if changed
     displayString(62,70,"C",ST7735_WHITE);
  }
  if(fahrenheit!=old_f) {
     displayInt(fahrenheit,69,70,ST7735_WHITE,ST7735_BLACK);
     displayString(102,70,"F",ST7735_WHITE);
  }
  old_c=celsius; old_f=fahrenheit;
  
  x_axis = Esplora.readAccelerometer(X_AXIS);
  y_axis = Esplora.readAccelerometer(Y_AXIS);
  z_axis = Esplora.readAccelerometer(Z_AXIS);
  if(x_axis!=oldx) {
    displayInt2(x_axis,30,96,ST7735_WHITE,ST7735_BLACK);
  }
  if(y_axis!=oldy) {
    displayInt2(y_axis,70,96,ST7735_WHITE,ST7735_BLACK);
  }
  if(z_axis!=oldz) {
    displayInt2(z_axis,110,96,ST7735_WHITE,ST7735_BLACK);
  }
  oldx=x_axis; oldy=y_axis; oldz=z_axis;  // store values for comparison next refresh
  
  /*
    Wait a little bit (50ms) between one button check and another.
    When a mechanical switch is pressed or released, the
    contacts may bounce very rapidly. If the check is done too
    fast, these bounces may be confused as multiple presses and
    might lead to unexpected behavior.
   */
  delay(50);
  }
}

void displayString(byte x, byte y, char *text, uint16_t color) { // write string to LCD
  tft.setCursor(x,y);
  tft.setTextColor(color);
  tft.print(text);
}

void displayChar(byte x, byte y, char text, uint16_t color) {  // write character to LCD
  tft.setCursor(x,y);
  tft.setTextColor(color);
  tft.print(text);
}

// display an unsigned character  on LCD (if you have a signed value, use displayInt2)
void displayInt(unsigned int num, byte nx, byte ny, unsigned int color, unsigned int backcolor) {
  tft.fillRect(nx, ny, 29, 7, backcolor);
  displayChar(nx+24, ny, 48+(num%10), color);
  if(num>9) {
    displayChar(nx+18, ny, 48+(num%100)/10, color);
    if(num>99) {
      displayChar(nx+12, ny, 48+(num%1000)/100, color);
      if(num>999) {
        displayChar(nx+6, ny, 48+(num%10000)/1000, color);
        if(num>9999) {
          displayChar(nx, ny, 48+(num%100000)/10000, color);
        }
      }
    }
  }
}

void displayInt2(int num, byte nx, byte ny, unsigned int color, unsigned int backcolor) {
  tft.fillRect(nx, ny, 29, 7, backcolor); // clear old value
  tft.setCursor(nx,ny);
  tft.setTextColor(color);
  tft.print(num);  // print handles signs, leading space, etc. in this version
}
