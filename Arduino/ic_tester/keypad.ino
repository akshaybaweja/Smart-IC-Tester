//UI details
#define BUTTON_X 34
#define BUTTON_Y 90
#define BUTTON_W 50
#define BUTTON_H 46
#define BUTTON_SPACING_X 13
#define BUTTON_SPACING_Y 13
#define BUTTON_TEXTSIZE 2

// text box where numbers go
#define TEXT_X 2
#define TEXT_Y 2
#define TEXT_W 317
#define TEXT_H 55
#define TEXT_TSIZE 4
#define TEXT_TCOLOR GREEN
#define TEXT_LEN 7

char buttonlabels[10][5] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "0"};

String getIC()
{
  String number;
  int i = 0;
  boolean status = false;
  tft.fillScreen(BLACK);
  // create buttons
  for (uint8_t row = 0; row < 2; row++) {
    for (uint8_t col = 0; col < 5; col++) {
      buttons[col + row * 5].initButton(&tft, BUTTON_X + col * (BUTTON_W + BUTTON_SPACING_X),
                                        BUTTON_Y + row * (BUTTON_H + BUTTON_SPACING_Y), // x, y, w, h, outline, fill, text
                                        BUTTON_W, BUTTON_H, WHITE, BLACK, WHITE,
                                        buttonlabels[col + row * 5], BUTTON_TEXTSIZE);
      buttons[col + row * 5].drawButton();
    }
  }
  buttons[10].initButton(&tft, 90, BUTTON_Y + 2 * (BUTTON_H + BUTTON_SPACING_Y), 140, BUTTON_H, WHITE, GREEN, WHITE, "OK", BUTTON_TEXTSIZE);
  buttons[10].drawButton();

  buttons[11].initButton(&tft, 238, BUTTON_Y + 2 * (BUTTON_H + BUTTON_SPACING_Y), 140, BUTTON_H, WHITE, RED, WHITE, "Clear", BUTTON_TEXTSIZE);
  buttons[11].drawButton();
  // create 'text field'
  tft.drawRect(TEXT_X, TEXT_Y, TEXT_W, TEXT_H, WHITE);


  while (!status)
  {
    digitalWrite(13, HIGH);
    TSPoint p = ts.getPoint();
    digitalWrite(13, LOW);

    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);

    bool pressed = false;

    if (p.z > MINPRESSURE && p.z < MAXPRESSURE)
    {
      // scale from 0->1023 to tft.width
      p.x = map(p.x, TS_MINX, TS_MAXX, tft.height(), 0);
      p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.width());

      //Swapping for Set Rotation 3
      p.x = p.x + p.y;
      p.y = p.x - p.y;
      p.x = p.x - p.y;

      pressed = true;
    }

    // go thru all the buttons, checking if they were pressed
    for (uint8_t b = 0; b < 12; b++)
    {
      if (buttons[b].contains(p.x - 8, p.y - 8) && pressed)
      {
        //Serial.print("Pressing: "); Serial.println(b);
        buttons[b].press(true);  // tell the button it is pressed
      }
      else
      {
        buttons[b].press(false);  // tell the button it is NOT pressed
      }
    }

    // now we can ask the buttons if their state has changed
    for (uint8_t b = 0; b < 12; b++)
    {
      if (buttons[b].justReleased())
      {
        //Serial.print("Released: "); Serial.println(b);
        buttons[b].drawButton();  // draw normal
      }

      if (buttons[b].justPressed())
      {
        buttons[b].drawButton(true);  // draw invert!

        // if a numberpad button, append the relevant # to the textfield
        if (b < 10) {
          if (i < TEXT_LEN)
          {
            number += buttonlabels[b][0];
            i++;
          }
        }

        // clr button! delete char
        if (b == 11) {
          tft.setCursor(TEXT_X + 2, TEXT_Y + 10);
          tft.setTextColor(BLACK);
          tft.setTextSize(TEXT_TSIZE);
          tft.print(number);
          number = "";
          i = 0;
        }

        // update the current text field
        //Serial.println(textfield);
        tft.setCursor(TEXT_X + 2, TEXT_Y + 10);
        tft.setTextColor(TEXT_TCOLOR, BLACK);
        tft.setTextSize(TEXT_TSIZE);
        tft.print(number);

        // we dont really check that the text field makes sense
        // just try to call
        if (b == 10) {
          Serial.print("IC no: "); Serial.println(number);
          status = true;
        }
        delay(100); // UI debouncing
      }
    }
  }
  return number;
}

