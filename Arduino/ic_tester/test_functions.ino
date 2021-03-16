boolean testCase(String test, int pins)
{
  boolean result = true;
  int clkPin = -1;

  Serial.println("SignalIn : " + test);
  Serial.print("Response : ");

  //Setting Vcc, GND and INPUTS
  for (int i = 0; i < pins; i++)
  {
    switch (test[i])
    {
      case 'V' : pinMode(pin[i], OUTPUT); digitalWrite(pin[i], HIGH);
        break;
      case 'G' : pinMode(pin[i], OUTPUT); digitalWrite(pin[i], LOW);
        break;
      case 'L' : digitalWrite(pin[i], LOW); pinMode(pin[i], INPUT_PULLUP);
        break;
      case 'H' : digitalWrite(pin[i], LOW); pinMode(pin[i], INPUT_PULLUP);
        break;
    }
  }

  delay(5);

  //Setting Input Signals
  for (int i = 0; i < pins; i++)
  {
    switch (test[i])
    {
      case 'X' :
      case '0' : pinMode(pin[i], OUTPUT); digitalWrite(pin[i], LOW);
        break;
      case '1' : pinMode(pin[i], OUTPUT); digitalWrite(pin[i], HIGH);
        break;
      case 'C' : clkPin = pin[i]; pinMode(pin[i], OUTPUT); digitalWrite(pin[i], LOW);
        break;
    }
  }

  if (clkPin != -1)
  {
    //Clock Trigger
    digitalWrite(clkPin, HIGH);
    delay(10);
    digitalWrite(clkPin, LOW);
  }

  delay(5);

  //Reading Outputs
  for (int i = 0; i < pins; i++)
  {
    switch (test[i])
    {
      case 'H' : if (!digitalRead(pin[i])) {
          result = false;
          Serial.print('L');
        }
        else Serial.print(' ');
        break;
      case 'L' : if (digitalRead(pin[i])) {
          result = false;
          Serial.print('H');
        }
        else Serial.print(' ');
        break;
      default : Serial.print(' ');
    }
  }
  Serial.println(";");
  //Serial.println("\nCase Result : "+String(result));
  return result;
}

/*
  boolean testIC(String buffer, int pins)
  {
  int startIndex = 0, lastIndex = 0;
  boolean result=true;
  String newTest;
  buffer.trim();

  Serial.print(buffer);
  while (startIndex <= buffer.lastIndexOf('\n'))
  {
    lastIndex = buffer.indexOf('\n',startIndex);

    Serial.println("S: "+String(startIndex)+" L: "+String(lastIndex));

    newTest=buffer.substring(startIndex, lastIndex);
    newTest.trim();
    //Serial.println(newTest);
    Serial.println("------------------");
    startIndex = lastIndex + 1;

    if (testCase(newTest, pins)==false)
    {
      result = false;
      //break;
    }
  }
  //Serial.println("testIC() : "+String(result));
  return result;
  }
*/
