#include <Wire.h>
#include <DS3231.h>
#include <SPI.h>          // f.k. for Arduino-1.5.2
#include "Adafruit_GFX.h"
#include <MCUFRIEND_kbv.h>

// Assign human-readable names to some common 16-bit color values:
#define BLACK           0x0000
#define NAVY            0x000F
#define DARKGREEN       0x03E0
#define DARKCYAN        0x03EF
#define MAROON          0x7800
#define PURPLE          0x780F
#define OLIVE           0x7BE0
#define LIGHTGREY       0xC618
#define DARKGREY        0x7BEF
#define BLUE            0x001F
#define GREEN           0x07E0
#define CYAN            0x07FF
#define RED             0xF800
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF
#define ORANGE          0xFD20
#define GREENYELLOW     0xAFE5
#define PINK            0xF81F
#define HOLIDAY_COLOR   0xF884

#define FONTSIZE 5

#define HIGH_CONTRAST 1

#if defined(HIGH_CONTRAST)
  #define BACKGROUND_COLOR BLACK
  #define TEXT_COLOR WHITE
#else
  #define BACKGROUND_COLOR WHITE
  #define TEXT_COLOR BLACK
#endif

uint16_t g_identifier;
MCUFRIEND_kbv tft;
DS3231 clock;
RTCDateTime rtc;

unsigned long ping = millis();
const int buttonPinOne = 11;
const int buttonPinTwo = 12;
boolean summerTime = false;

void setup(void);
void loop(void);
void printData(void);
bool ifBefore(RTCDateTime currentTime, uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);

void setup() {
  delay(100);
  Serial.begin(9600);

  g_identifier = tft.readID(); // 0x9481
  Serial.print("ID = 0x");
  Serial.println(g_identifier, HEX);
  if (g_identifier == 0x00D3 || g_identifier == 0xD3D3) g_identifier = 0x9481; // write-only shield
  if (g_identifier == 0xFFFF) g_identifier = 0x9341; // serial
  tft.begin(g_identifier);
  tft.setRotation(0); // 320x480 (49mm x 74mm) ok 65px/cm
  tft.fillScreen(BACKGROUND_COLOR);

  // Initialize DS3231
  Serial.println("Initialize DS3231");;
  clock.begin();
  // Set sketch compiling time
  // clock.setDateTime(__DATE__, __TIME__);

  pinMode(buttonPinOne, INPUT);
  pinMode(buttonPinTwo, INPUT);
  
  printData();
}

void loop() {
  // Togle 32kHz output
  //clock.enable32kHz(state);
  //state = !state;
//  buttonState = digitalRead(buttonPinOne);
//  if (buttonStateOne == HIGH) {
//    // turn LED on:
//    tft.invertDisplay(false);
//  } else {
//    // turn LED off:
//    tft.invertDisplay(true);
//  }

  if (millis() - ping >= 1000) {
    ping = millis();
    //if(ledState == LOW) ledState = HIGH;
    //else ledState = LOW;
    //digitalWrite(ledPin, ledState);
    printData();
    Serial.print("loop time: ");
    Serial.println(millis() - ping);
    Serial.print("total time: ");
    Serial.println(rtc.unixtime);
    Serial.print(rtc.hour);
    Serial.print(" : ");
    Serial.print(rtc.minute);
    Serial.print(" : ");
    Serial.println(rtc.second);
    Serial.print("button 1 : ");
    Serial.println(digitalRead(buttonPinOne));
    Serial.print("button 2 : ");
    Serial.println(digitalRead(buttonPinTwo));
  }
}

void printData() {
  rtc = clock.getDateTime();
  int textSize = 1;
  int numberOfSigns = 1;

  if ((rtc.hour == 0) && (rtc.minute == 0) && (rtc.second == 0)) tft.fillScreen(BACKGROUND_COLOR);

  textSize = 10;
  tft.setTextSize(textSize);
  tft.setTextColor(TEXT_COLOR, BACKGROUND_COLOR); // text color, text backgroud color

  // positioning
  if (((rtc.hour) / 10) == 0) tft.setCursor(45, 50); // 3 digits + :
  else tft.setCursor(15, 50); // 4 digits + :

  tft.print(clock.dateFormat("G", rtc)); // hour withoul leading 0
  if (((rtc.second) % 2) > 0)  tft.print(":");
  else tft.print(" ");
  tft.println(clock.dateFormat("i", rtc)); // minutes with leading 0

  // if Sunday change color to light red and back to black
  if (atoi(clock.dateFormat("N", rtc)) == 7) tft.setTextColor(HOLIDAY_COLOR, BACKGROUND_COLOR);

  textSize = 4; //sobota
  tft.setTextSize(textSize);
  numberOfSigns = strlen(clock.dateFormat("l", rtc));
  tft.setCursor((320 - (textSize * numberOfSigns * FONTSIZE + textSize * (numberOfSigns - 1))) / 2, 160);
  tft.println(clock.dateFormat("l", rtc));   //day of week

  textSize = 8; //27
  tft.setTextSize(textSize);
  if (rtc.day > 9) tft.setCursor((320 - (textSize * 2 * FONTSIZE + textSize)) / 2, 240);
  else tft.setCursor((320 - (textSize * FONTSIZE)) / 2, 240);
  tft.println(rtc.day);
  tft.println();

  // if Sunday change color to dark red and back to black
  if (atoi(clock.dateFormat("N", rtc)) == 7) tft.setTextColor(TEXT_COLOR, BACKGROUND_COLOR);

  textSize = 4; //sierpnia
  tft.setTextSize(textSize);
  numberOfSigns = strlen(clock.dateFormat("F", rtc));
  tft.setCursor((320 - (textSize * numberOfSigns * FONTSIZE + textSize * (numberOfSigns - 1))) / 2, 340);
  tft.println(clock.dateFormat("F", rtc));  //month
  tft.println();

}

bool ifBefore(RTCDateTime currentTime, uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second){
  if(year < currentTime.year) return true;
  else if ((year == currentTime.year) && ( month < currentTime.month) )return true;
  else if ((year == currentTime.year) && ( month == currentTime.month) && ( day == currentTime.day))return true;
  else return false;
}

