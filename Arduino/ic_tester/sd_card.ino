void SD_init()
{
  pinMode(SS, OUTPUT);

  if (!SD.begin(10, 11, 12, 13)) {
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("SD card initialized.");
}

void autoSearch(int pins)
{
  tft.fillScreen(BLACK);

  tft.setCursor(25, 1);
  tft.setTextColor(GREEN);  tft.setTextSize(5);
  tft.println("IC TESTER");

  tft.setCursor(60, 55);
  tft.setTextColor(YELLOW);  tft.setTextSize(3);
  tft.println("Auto Search");

  File dataFile = SD.open(fname);

  String newCase;
  IC newIC;
  int ICpins;
  boolean result;
  IC ans[10];

  if (dataFile)
  {
    int count = 0;
    pin = new int[pins];

    if (pins == 16)
      pin = pin16;
    else if (pins == 14)
      pin = pin14;


    while (dataFile.available())
    {
      searching(1);
      dataFile.readStringUntil('$');

      newIC.num = dataFile.readStringUntil('\n');
      newIC.name = dataFile.readStringUntil('\n');
      ICpins = dataFile.readStringUntil('\n').toInt();
      Serial.println(newIC.num);

      if (ICpins == pins)
      {
        result = true;

        Serial.println("Testing Now");

        while (dataFile.peek() != '$')
        {
          newCase = dataFile.readStringUntil('\n');
          newCase.trim();
          if (testCase(newCase, ICpins) == false)
          {
            result = false;
            break;
          }
        }

        if (result == true)
        {
          Serial.println("\nMatch Found !!\n");
          ans[count++] = newIC;
          //Serial.println(ans[count-1].num+"::"+ans[count-1].name);
        }

        Serial.println("Test Completed");
      }
      searching(3);
    }
    delete(pin);
    dataFile.close();

    if (count == 0)
    {
      Serial.println("No Match Found");
      autoSearchResult(0);
    }
    else
    {
      Serial.println("\nCount : " + String(count));
      
      /*for (int i = 0; i < count; i++)
      {
        Serial.println(String(i) + ". " + ans[i].num + " : " + ans[i].name);
      }*/

      while (count--)
      {
        Serial.println(String(count) + ". " + ans[count].num + " : " + ans[count].name);
        autoSearchResult(1, ans[count].num, ans[count].name, count);
      }
    }
  }

  else
  {
    Serial.print("error opening "); Serial.println(fname);
    dataFile.close();
    SD.begin(10, 11, 12, 13);

    tft.setCursor(5, 140);
    tft.setTextColor(RED);  tft.setTextSize(4);
    tft.print("ERROR:");
    tft.setTextColor(MAGENTA);
    tft.println("SD CARD");

    tft.setCursor(40, 220);
    tft.setTextColor(RED);  tft.setTextSize(2);
    tft.println("Touch for MAIN MENU");

    getTouch();
    resetFunc();
  }
}

void manualSearch(String number)
{
  tft.fillScreen(BLACK);

  tft.setCursor(25, 1);
  tft.setTextColor(GREEN);  tft.setTextSize(5);
  tft.println("IC TESTER");

  tft.setCursor(45, 55);
  tft.setTextColor(YELLOW);  tft.setTextSize(3);
  tft.println("Manual Search");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open(fname);

  String buffer;
  String newCase;
  IC newIC;
  int pins, send_status = -1;
  boolean result = true;

  // if the file is available, write to it:
  if (dataFile)
  {
    while (dataFile.available())
    {
      searching(1);
      result = true;
      dataFile.readStringUntil('$');

      buffer = dataFile.readStringUntil('\n');
      buffer.trim();

      Serial.println(buffer);

      if (number == buffer)
      {
        searching(2);
        newIC.num = buffer;
        newIC.name = dataFile.readStringUntil('\n');
        pins = dataFile.readStringUntil('\n').toInt();
        pin = new int[pins];

        if (pins == 16)
          pin = pin16;
        else if (pins == 14)
          pin = pin14;

        while (dataFile.peek() != '$')
        {
          newCase = dataFile.readStringUntil('\n');
          newCase.trim();
          if (testCase(newCase, pins) == false)
          {
            result = false;
          }
          delay(50);
          searching(3);
        }

        delete(pin);

        if (result == true)
        {
          Serial.println("IC is GOOD");
          send_status = 1;
        }
        else
        {
          Serial.println("IC is BAD");
          send_status = 0;
        }
        break;
      }
    }
    dataFile.close();
    if (send_status == -1)
      manualSearchResult(number, "", -1);
    else
      manualSearchResult(newIC.num, newIC.name, send_status);
  }
  else
  {
    Serial.print("error opening "); Serial.println(fname);
    dataFile.close();
    SD.begin(10, 11, 12, 13);

    tft.setCursor(5, 140);
    tft.setTextColor(RED);  tft.setTextSize(4);
    tft.print("ERROR:");
    tft.setTextColor(MAGENTA);
    tft.println("SD CARD");

    tft.setCursor(40, 220);
    tft.setTextColor(RED);  tft.setTextSize(2);
    tft.println("Touch for MAIN MENU");

    getTouch();
    resetFunc();
  }
}

