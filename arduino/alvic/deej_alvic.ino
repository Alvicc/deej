#include <RTClib.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ezButton.h>

// SERIAL COMMAND IDs
// Must match the command ids in the client (serial.go)
#define CMD_DEBUG       0
#define CMD_TIME_SYNC   1
#define CMD_VOL_SND     2
#define CMD_REQ_TIME    3
#define CMD_UPDATE_SONG 4
#define CMD_DEBUG_MSG   5


#define SCREEN_WIDTH    128 // OLED display width, in pixels
#define SCREEN_HEIGHT   32 // OLED display height, in pixels

// #define DEBUG

// Splah screen duration in milliseconds
#ifdef DEBUG 
  #define SPLASH_DELAY    0  
#else
  #define SPLASH_DELAY    1500  
#endif

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LOGO_WIDTH  128
#define LOGO_HEIGHT 32

// Generated with lcd-image-converter : https://github.com/riuson/lcd-image-converter
static const unsigned char PROGMEM logo_bmp[] =
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x6f, 0xa0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x7f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x07, 0xc0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x7f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x38, 0x00, 0x60, 0x00, 0x00, 
0x00, 0xf0, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x3c, 0x00, 0xe0, 0x00, 0x00, 
0x00, 0x70, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0xf0, 0x00, 0x00, 
0x00, 0xf0, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0xf0, 0x00, 0x00, 
0x00, 0x70, 0x0f, 0x00, 0x3a, 0x00, 0x3a, 0x02, 0x80, 0x00, 0x00, 0x38, 0x01, 0xf0, 0x00, 0x00, 
0x00, 0xf0, 0x07, 0x80, 0xff, 0x00, 0xff, 0x07, 0x80, 0x00, 0x00, 0x3c, 0x00, 0xe0, 0x00, 0x00, 
0x00, 0xf0, 0x07, 0x83, 0xff, 0x83, 0xff, 0x87, 0x00, 0x00, 0x00, 0x38, 0x01, 0xf0, 0x00, 0x00, 
0x00, 0xe0, 0x07, 0x87, 0x83, 0x87, 0x83, 0x87, 0x80, 0x00, 0x00, 0x18, 0x00, 0xc0, 0x00, 0x00, 
0x00, 0xf0, 0x07, 0x07, 0x03, 0xc7, 0x03, 0xc7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0xe0, 0x07, 0x8f, 0x03, 0x8f, 0x03, 0x8f, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x60, 0x00, 
0x00, 0xe0, 0x0f, 0x0e, 0x0f, 0x8e, 0x0f, 0x87, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0xe0, 0x00, 
0x00, 0xf0, 0x0f, 0x1f, 0xff, 0x1f, 0xff, 0x0f, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0xc0, 0x00, 
0x01, 0xe0, 0x0f, 0x1f, 0xfc, 0x1f, 0xfc, 0x07, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0xf0, 0x00, 
0x00, 0xe0, 0x1e, 0x1f, 0xa0, 0x1f, 0xd0, 0x0f, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0xe0, 0x00, 
0x01, 0xe0, 0x1c, 0x0e, 0x00, 0x0e, 0x00, 0x0e, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x01, 0xe0, 0x00, 
0x01, 0xe0, 0x3c, 0x0e, 0x00, 0x1e, 0x00, 0x0f, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x03, 0xc0, 0x00, 
0x01, 0xe0, 0xf8, 0x0f, 0x03, 0x0f, 0x03, 0x0e, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x07, 0x80, 0x00, 
0x01, 0xff, 0xf0, 0x0f, 0xff, 0x07, 0xff, 0x0e, 0x00, 0x00, 0x0f, 0x80, 0x00, 0x0f, 0x00, 0x00, 
0x01, 0xff, 0xc0, 0x03, 0xfe, 0x07, 0xfe, 0x0e, 0x00, 0x00, 0x03, 0xe8, 0x00, 0x7e, 0x00, 0x00, 
0x01, 0xfe, 0x00, 0x01, 0xe8, 0x00, 0xe8, 0x1e, 0x00, 0x00, 0x03, 0xfa, 0x05, 0xfc, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0xff, 0xff, 0xf0, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x1f, 0xff, 0x80, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x05, 0xfd, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


// Deej
const int NUM_SLIDERS = 4;
const int NUM_SAMPLES = 15;
const int ANALOG_IMPUTS[NUM_SLIDERS] = {A0, A1, A2, A3};
const int DIGITAL_INPUTS[NUM_SLIDERS] = {9, 10, 11, 12};
const float MEASURED_MAX_VALUE = 1000.0; // My potentionmeters never got me to 1023, so i cheat to be abble to display 100% volume
const unsigned long TIME_REQUEST_INTERVAL = 1000UL*60UL*15UL; // Sync internal clock every 15 minutes
const unsigned long SCROLL_PAUSE = 1000; // Pause for n ms at the start and end of the scrolling animation
const int SCROLL_TIME_INTERVAL = 250; // Scroll the current song info by n ms

unsigned long lastTimeRequest = 0;

int analogSliderValues[NUM_SLIDERS];

// SCROLL
unsigned long scrollTimer = 0;
bool scrollPaused = false;
unsigned long scrollPause = 0;
int charOffset = 0;

String currentSong;

struct channel {
  int analogSliderValues[NUM_SAMPLES]; // Raw values from the potentiometers ( [0] is the most recent )
  int averagedValue;                   // Averaged value, to get more stable GFX
  bool isMuted;
  ezButton * button;
};

struct channel channels[NUM_SLIDERS];



void setup() {
  
  Serial.begin(9600);
  
  // Setup Deej
  for (int i = 0; i < NUM_SLIDERS; i++) {
    pinMode(ANALOG_IMPUTS[i], INPUT);
    // pinMode(DIGITAL_INPUTS[i], INPUT_PULLUP);
    channels[i].isMuted = false;
    channels[i].button = new ezButton(DIGITAL_INPUTS[i]);
  }

  currentSong = "undefined";

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }


  // Splash screen
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.drawBitmap(
    (display.width()  - LOGO_WIDTH ) / 2,
    (display.height() - LOGO_HEIGHT) / 2,
    logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
    
  display.display();

  //DateTime date = DateTime(1605319321);
  RTC_Millis::begin(DateTime());

  scrollTimer = millis();

  delay(SPLASH_DELAY);
  display.clearDisplay();
  requestTime();
}


/**
 * Main Loop
 */
void loop() {
  updateSliderValues();
  sendSliderValues(); // Actually send data (all the time)
  renderToDisplay();
  
  // Periodically sync internal clock
  unsigned long now = millis();
  if (now - lastTimeRequest >= TIME_REQUEST_INTERVAL) {
    lastTimeRequest = now;
    requestTime();
  }
}


/**
 * Get the latest value from the potentionmter and compute the average from past samples
 */
void updateSliderValues() {
  // Get Latest slider data and update the mutted state
  for (int i = 0; i < NUM_SLIDERS; i++) {

    // Slider value
     channels[i].analogSliderValues[0] = analogRead(ANALOG_IMPUTS[i]);
     for(int j = NUM_SAMPLES ; j > 0 ; j--){
      channels[i].analogSliderValues[j] = channels[i].analogSliderValues[j-1];
     }

      // Mutted state
      channels[i].button->loop();
    
     if ( channels[i].button->isPressed() ) {
       if( channels[i].isMuted ) {
         channels[i].isMuted = false;
       } else {
         channels[i].isMuted = true;
       }
     }

     // Shift the value record
     for(int j = NUM_SAMPLES ; j > 0 ; j--){
      channels[i].analogSliderValues[j] = channels[i].analogSliderValues[j-1];
     }


    // Compute the mean average value
    int sum = 0;
    for(int j = 0 ; j < NUM_SAMPLES ; j++){
      sum += channels[i].analogSliderValues[j];
    }

    channels[i].averagedValue = (sum/NUM_SAMPLES);
  }
}
  





/**
 * Send the potentiometers value to the Deej cient
 */
void sendSliderValues() {
  String builtString = String("");

  for (int i = 0; i < NUM_SLIDERS; i++) {
    builtString += channels[i].isMuted ? String((int)0) : String((int)channels[i].averagedValue); 
    // if(channels[i].isMuted){
    //   builtString += String((int)0);
    // } else {
    //   builtString += String((int)channels[i].averagedValue);
    // }
    
    if (i < NUM_SLIDERS - 1) {
      builtString += String("|");
    }
  }
  String formattedCommand = "<" + String(CMD_VOL_SND) + ":" + builtString + ">";
  sendCmd(CMD_VOL_SND, builtString);
}



/**
 * Render on Display
 */
void renderToDisplay(){
    
  display.clearDisplay();
  
  //=================================================================
  // DISPLAY TIME
  //=================================================================
  display.setTextSize(2);
  DateTime now = RTC_Millis::now();
  String hours = now.hour() < 10 ? "0"+String(now.hour()) : now.hour();
  String minutes = now.minute() < 10 ? "0" + String( now.minute() ) : now.minute();
  // String seconds = now.second() < 10 ? "0" + String( now.second() ) : now.second();
  String timeString = hours + ":" + minutes;
  
  int16_t textBoundaryX = 0; 
  int16_t textBoundaryY = 0; 
  uint16_t textWidth = 0;
  uint16_t textHeight = 0;

  display.getTextBounds(timeString, 0, 0, &textBoundaryX, &textBoundaryY, &textWidth, &textHeight);
  
  int16_t cx = (SCREEN_WIDTH/2)-(textWidth/2)  + 12;
  int16_t cy = (SCREEN_HEIGHT/2)-(textHeight/2) -5;
  display.setCursor(cx, cy);
  display.print(timeString);

  
  //=================================================================
  // DISPLAY CURRENTLY PLAYING SONG
  //=================================================================
  display.setTextSize(1);
  display.setCursor(4*(5+1) + 5 , SCREEN_HEIGHT-8);
  
  // Figure out by how many character we need to scroll
  int spaceToEdge = (SCREEN_WIDTH)- display.getCursorX();
  int charWidth = 7; // 6px per char + 1px of padding
  int songTextWidth = strlen(currentSong.c_str()) * charWidth;
  int toScroll = songTextWidth - spaceToEdge;
  int offsetBy = 0;
  
  if (toScroll > 0) {
    offsetBy = int(toScroll / (charWidth));
  }
  
  // Pause the scrolling for
  unsigned long pauseElapsed = millis()- scrollPause;
  if (pauseElapsed >= SCROLL_PAUSE) scrollPaused = false;
  
  unsigned long elapsed = millis()- scrollTimer;

  if ( elapsed >= SCROLL_TIME_INTERVAL && !scrollPaused) {
    // Reset animation timer
    scrollTimer = millis();

    // Move text by 1 character
    charOffset ++;

    // Pause at the start of scrolling
    if (charOffset == offsetBy-1) {
      scrollPaused = true;
      scrollPause = millis();
    }

    // Pause at the end of scrolling, and reset animation
    if (charOffset >= offsetBy) {
      charOffset = 0;
      scrollPaused = true;
      scrollPause = millis();
    }
  }

  // Print the right portion of the current song info
  for (int i = charOffset ; i < ( strlen(currentSong.c_str()) ) ; i++) {
    display.print(currentSong[i]);
  }



  //=================================================================
  // DISPLAY VOLUME LEVELS
  //=================================================================
  for (int i = 0; i < NUM_SLIDERS; i++) {
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    float normalized = min(channels[i].averagedValue / MEASURED_MAX_VALUE , 1.0);
    
    int x, y, w, h;
    h = normalized * SCREEN_HEIGHT;
    w = 5;
    x = 0 + (w+1) * i;
    y = SCREEN_HEIGHT-1-h;

    if(channels[i].isMuted){
      display.drawRect(x, y, w, h, SSD1306_WHITE);  
      // display.print(">");
    } else {
      display.fillRect(x, y, w, h, SSD1306_WHITE);  
    }
    
    display.drawLine(x,     0,                x+w,    0,                SSD1306_WHITE); // Little bar at the top
    display.drawLine(x,     SCREEN_HEIGHT/2,  x+w,    SCREEN_HEIGHT/2,  SSD1306_WHITE); // Little bar in the middle
    display.drawLine(x,     SCREEN_HEIGHT-1,  x+w,    SCREEN_HEIGHT-1,  SSD1306_WHITE); // Little bar at the bottom
    display.drawLine(x+w/2, 0,                x+w/2,  SCREEN_HEIGHT-1,  SSD1306_WHITE);  // Long ligne
  }


  // Flip to screen
  display.display();
}



/**
 * Debug print
 */
void printSliderValues() {
  
  for (int i = 0; i < NUM_SLIDERS; i++) {
    String val = digitalRead(DIGITAL_INPUTS[i]) == HIGH ? "HIGH" : "LOW";
    Serial.write(val.c_str());
    Serial.write("|");
    String printedString = String("Slider #") + String(i + 1) + String(": ") + String(channels[i].analogSliderValues[0]) + String(" mV")+ String(" Mutted :") + String(channels[i].isMuted);
    Serial.write(printedString.c_str());

    if (i < NUM_SLIDERS - 1) {
      Serial.write(" | ");
    } else {
      Serial.write("\n");
    }
  }

}

// Read serial input
void serialEvent(){
  // Serial commands start with '<' and end with '>'
  if ( Serial.find('<') ) {
    int cmd_type = Serial.readStringUntil(':').toInt();
    String data = Serial.readStringUntil('>');
    #ifdef DEBUG
      sendCmd(CMD_DEBUG_MSG, data);
    #endif
    parseCommand(cmd_type, data);
  } else {
    Serial.flush();
  }
  

} 

// Parse and dispatch serial commands
void parseCommand(int cmd_type, String data){

  //printDebug(String(cmd_type) + " => " + data);
  switch(cmd_type) {
    case CMD_DEBUG :
      printDebug(String(cmd_type) + " => " + data);
      break;
    case CMD_TIME_SYNC:
    {
      long time = data.toInt();
      setClock(time);
      break;
    }
    case CMD_UPDATE_SONG:
    {
      currentSong = data;
      break;
    }
    default:
      Serial.println("Commande Type unknown : "  + String(cmd_type));
      Serial.println("Message : "  + String(cmd_type) + " => " + data);
      break;
  }
}

// Print a debug messagr to on the dislay
void printDebug(String str) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor( 3, 3);
  display.print("DEBUG : " + str);
  display.display();
}

void setClock(long unix_time){
  RTC_Millis::adjust(DateTime(unix_time));
}

void requestTime(){
  sendCmd(CMD_REQ_TIME, "");
}

void sendCmd(int cmdID, String data) {
  Serial.println("<" + String(cmdID) + ":" + data +">");
}