//LED
#include <FastLED.h>

#define LED_PIN     46
#define NUM_LEDS    2
#define BRIGHTNESS  200
#define LED_TYPE    WS2812B

#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

//TFT
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h> //Touch Screen Library

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // LCD RESET goes to Analog 4

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940

#define MINPRESSURE 10
#define MAXPRESSURE 1000

// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

//TFT initialization
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

Adafruit_GFX_Button buttons[12];

//SD CARD
#include <SPI.h>
#include <SD.h>

#define chipSelect 10

//Pin Definitions
int pin14[14] = {30, 32, 34, 36, 38, 40, 42, 43, 41, 39, 37, 35, 33, 31};
int pin16[16] = {30, 32, 34, 36, 38, 40, 42, 44, 45, 43, 41, 39, 37, 35, 33, 31};

//Database File name
#define fname "database.txt"

//Structure Definiton for IC
typedef struct {
  String num;
  String name;
} IC;

//Function Headers
boolean testIC(String buffer, int pins);
void autoSearchResult(int mode, String number = "", String name = "", int count = 0);
void manualSearchResult(String number, String name, int status);

int screenStatus = 0, lastStatus = 0;
int *pin, pinCount = 0;

void(* resetFunc) (void) = 0;

void setup() {
  Serial.begin(9600);

  delay( 1000 ); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );

  //TFT setup
  tft_init();

  //SD Card Setup
  SD_init();

}

void loop() {

  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE)
  {
    //Serial.println("Z: " + String(p.z) + " X: " + String(p.x) + " Y: " + String(p.y));
    // scale from 0->1023 to tft.width
    p.x = map(p.x, TS_MINX, TS_MAXX, tft.height(), 0);
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.width());

    //Swapping for Set Rotation 3
    p.x = p.x + p.y;
    p.y = p.x - p.y;
    p.x = p.x - p.y;

    delay(10);
    Serial.println("(" +  String(p.x) + " , " + String(p.y) + " , " + String(p.z) + ")");

    if (screenStatus == 0)
      screenStatus = 1;

    else if (screenStatus == 1)
    {
      if (buttons[0].contains(p.x, p.y))
      {
        Serial.println("AUTO MODE");
        screenStatus = 2;
      }
      else if (buttons[1].contains(p.x, p.y))
      {
        Serial.println("MANUAL MODE");
        screenStatus = 3;
      }
    }
    else if (screenStatus == 2)
    {
      if (buttons[0].contains(p.x, p.y))
      {
        Serial.println("14 Pin Selected");
        screenStatus = 21;
        pinCount = 14;
      }
      else if (buttons[1].contains(p.x, p.y))
      {
        Serial.println("16 Pin Selected");
        screenStatus = 21;
        pinCount = 16;
      }
    }
    else if (screenStatus == 3)
    {
      if (buttons[0].contains(p.x, p.y))
      {
        Serial.println("Entering Keypad Mode");
        screenStatus = 30;
      }
    }
  }

  pinMode(XM, OUTPUT);
  digitalWrite(XM, LOW);
  pinMode(YP, OUTPUT);
  digitalWrite(YP, HIGH);

  if (screenStatus != lastStatus)
  {
    switch (screenStatus)
    {
      case 0: startScreen();
        break;
      case 1: modeScreen();
        break;
      case 2: autoScreen();
        break;
      case 21: autoSearch(pinCount);
        break;
      case 3: manualScreen();
        break;
      case 30: String number = getIC();
        manualSearch(number);
        break;
    }
    lastStatus = screenStatus;
  }
  delay(5);
}

void getTouch()
{
  boolean status = false;
  while (1)
  {
    digitalWrite(13, HIGH);
    TSPoint q = ts.getPoint();
    digitalWrite(13, LOW);
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);

    if (q.z > MINPRESSURE && q.z < MAXPRESSURE)
    {
      status = true;
      break;
    }
    delay(10);
  }

  pinMode(XM, OUTPUT);
  digitalWrite(XM, LOW);
  pinMode(YP, OUTPUT);
  digitalWrite(YP, HIGH);
}

