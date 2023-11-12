#include <dht11.h>
#include <Wire.h>
#include "rgb_lcd.h"
#define DHT11PIN 4
#include <Keypad.h>

const byte ROWS = 4;
const byte COLS = 4;
const int pin_fan = 13;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {10, 9, 8, 7};
byte colPins[COLS] = {6, 5, 3, 2};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

rgb_lcd lcd;

int colorR = 255;
int colorG = 255;
int colorB = 255;
String target_temp = "20";
const String pass = "1234";
String textNumber = "";
dht11 DHT11;
String passEntered = "default";


void  setup()
{
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  lcd.setRGB(colorR, colorG, colorB);
  Serial.begin(9600);
  lcd.clear();
  lcd.setCursor(0, 0);
  pinMode(pin_fan, OUTPUT);

}

String enter_pass(char keyPress) {
  textNumber = "";
  while (keyPress != '#' ) {              // test to see if the # key is pressed, if not add more characters
    keyPress = customKeypad.getKey();     // get new key press

    if (keyPress)
    {
      if (keyPress == '*')             // check if * has been pressed
      {
        //TODO delete for star
        lcd.setCursor(0, 0);
        textNumber = "";
        lcd.clear();
      }

      if (keyPress != 'D' && keyPress != 'A' && keyPress != 'B' && keyPress != 'C' && keyPress != '#' && keyPress != '*')  // check that only a number key has been pressed
      {
        textNumber += (char)keyPress;      // concatanate new character onto textNumber string

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(textNumber);
      }
    }
  }
  return textNumber;
}

bool checkPass(String passEntered) {
  if (passEntered == pass) {
    return true;
  }
  return false;
}

int setTemp() {
  char keyPress = '1';
  while (keyPress != '#' ) {              // test to see if the # key is pressed, if not add more characters
    keyPress = customKeypad.getKey();     // get new key press

    if (keyPress)
    {
      if (keyPress == '*')             // check if * has been pressed
      {
        //TODO delete for star
        lcd.setCursor(0, 0);
        target_temp = "";
        lcd.clear();
      }

      if (keyPress != 'D' && keyPress != 'A' && keyPress != 'B' && keyPress != 'C' && keyPress != '#' && keyPress != '*')  // check that only a number key has been pressed
      {
        if (target_temp.length() < 2){
          target_temp += (char)keyPress;      // concatanate new character onto textNumber string
        }

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Target Temp: ");
        lcd.print(target_temp);
      }
    }
  }
  lcd.clear();
}

void correct() {
  colorR = 0;
  colorG = 255;
  colorB = 0;
  lcd.setRGB(colorR, colorG, colorB);
  lcd.clear();
  lcd.print("Correct!");
  delay(1000);
  lcd.clear();
  setTemp();
  passEntered = "default";
}

void normal() {
  colorR = 255;
  colorG = 255;
  colorB = 255;
  lcd.setRGB(colorR, colorG, colorB);
}

void incorrect() {
  colorR = 255;
  colorG = 0;
  colorB = 0;
  lcd.setRGB(colorR, colorG, colorB);
  lcd.clear();
  lcd.print("Wrong pass...");
  delay(1000);
  lcd.clear();
  passEntered = "default";
}


void loop()
{
  char keyPress = customKeypad.getKey();
  //Serial.println(customKey);
  if (keyPress) {
    passEntered = enter_pass(keyPress);
    keyPress = NO_KEY;
  }

  bool rightPass = checkPass(passEntered);
  if (rightPass) {
    correct();
  } else if (passEntered == "default") {
    normal();
  } else if (!rightPass) {
    incorrect();
  }

  Serial.print(DHT11.temperature >= target_temp.toInt());
  if (DHT11.temperature >= target_temp.toInt()) {
    analogWrite(pin_fan, 100);
  } else if(DHT11.temperature <= target_temp.toInt()){
    analogWrite(pin_fan, 0);
  }
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:

  delay(100);

  int chk = DHT11.read(DHT11PIN);

  lcd.print("Target Temp: ");
  lcd.print(target_temp);
  //Serial.print("Humidity(%): ");
  //Serial.print(DHT11.humidity);

  lcd.setCursor(0, 0);
  lcd.print("Temp(C): ");
  lcd.print(DHT11.temperature);

}