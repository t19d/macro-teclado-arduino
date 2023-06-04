#include <Keypad.h>
#include <Encoder.h>
#include <Bounce2.h>
#include "HID-Project.h"

//Keypad buttons
const int R1 = 14;
const int R2 = 16;
const int R3 = 10;
const int R4 = 9;
const int C1 = 8;
const int C2 = 7;
const int C3 = 6;
const byte ROWS = 4;
const byte COLS = 3;
char keys[COLS][ROWS] = {
  { 'X', '7', '4', '1' },
  { '*', '8', '5', '2' },
  { '-', '9', '6', '3' }
};
byte rowPins[ROWS] = { R1, R2, R3, R4 };
byte colPins[COLS] = { C1, C2, C3 };
Keypad kpd = Keypad(makeKeymap(keys), colPins, rowPins, COLS, ROWS);

//State LED pins
const int S1 = 2;
const int S2 = 3;
const int S3 = 4;
const int numStates = 3;
const int States[numStates] = { S1, S2, S3 };
int currentState = 0;

//Encoder
const int SW = 18;
const int DT = 15;
const int CLK = 5;
const int threshold = 2;
Encoder encoder(DT, CLK);
Bounce encoderButton = Bounce(SW, 25);
const int timeLimit = 500;
int oldPosition = 0;

// Others
const int delayDefault = 100;

// *****************************
//             SETUP
// *****************************
void setup() {
  Serial.begin(9600);

  for (int i = 0; i < numStates; i++) {
    pinMode(States[i], OUTPUT);
    digitalWrite(States[i], LOW);
  }

  pinMode(CLK, INPUT_PULLUP);
  delay(2000);

  Keyboard.begin();
  Consumer.begin();

  Serial.println("Ready");

  StartAnimation();
  digitalWrite(States[currentState], HIGH);
  delay(500);
  Serial.println("setup");
}

// *****************************
//             LOOP
// *****************************
void loop() {
  //check the key matrix first
  char key = kpd.getKey();
  if (key) {
    Serial.println(key);
    switch (key) {
      case '*':
        ChangeState();
        break;

      case '-':
        Consumer.write(CONSUMER_EMAIL_READER);
        delay(delayDefault);
        Keyboard.releaseAll();
        break;

      case '7':
        Consumer.write(MEDIA_PREV);
        delay(delayDefault);
        Keyboard.releaseAll();
        break;

      case '8':
        Consumer.write(MEDIA_PLAY_PAUSE);
        delay(delayDefault);
        Keyboard.releaseAll();
        break;

      case '9':
        Consumer.write(MEDIA_NEXT);
        delay(delayDefault);
        Keyboard.releaseAll();
        break;

      default:
        switch (currentState) {
          case 0:
            LayoutLocal(key);
            break;
          case 1:
            LayoutTrabajar(key);
            break;
          case 2:
            Layout3(key);
            break;
        }
    }
    delay(10);
    Keyboard.releaseAll();
  }

  //check the encoder button
  if (encoderButton.update()) {
    Serial.print("encoderButton.update");
    if (encoderButton.fallingEdge()) {
      int fall = millis();
      while (!encoderButton.update()) {}
      if (encoderButton.risingEdge()) {
        int rise = millis();
        //Serial.println(rise - fall);
        if (rise - fall > timeLimit) {
          Consumer.write(MEDIA_NEXT);
          Serial.print("Next");
        } else {
          Consumer.write(MEDIA_PLAY_PAUSE);
          Serial.print("Play/Pause");
        }
      }
      Keyboard.releaseAll();
    }
  }

  //check encoder rotation
  long newPosition = encoder.read();
  if (abs(newPosition - oldPosition) > threshold) {
    Serial.print("newPosition ");
    Serial.println(newPosition);
    Serial.print("oldPosition ");
    Serial.println(oldPosition);

    if ((newPosition - oldPosition) > 0) {
      //volumeup
      Serial.println("volume up");
      Consumer.write(MEDIA_VOLUME_UP);
    } else {
      //volumedown
      Serial.println("volume down");
      Consumer.write(MEDIA_VOLUME_DOWN);
    }
    oldPosition = newPosition;
    Keyboard.releaseAll();
    delay(delayDefault);  //a delay of 200 seems to be the sweet spot for this encoder.
  }
}

void StartAnimation() {
  int waitTime = 250;
  digitalWrite(S1, HIGH);
  delay(waitTime);
  digitalWrite(S2, HIGH);
  delay(waitTime);
  digitalWrite(S3, HIGH);
  delay(waitTime);
  digitalWrite(S3, LOW);
  delay(waitTime);
  digitalWrite(S2, LOW);
  delay(waitTime);
  return;
}

void ChangeState() {
  digitalWrite(States[currentState], LOW);
  currentState++;
  if (currentState == numStates) {
    currentState = 0;
  }
  digitalWrite(States[currentState], HIGH);
  delay(delayDefault);
  return;
}

void LayoutLocal(char button) {
  switch (button) {
    case '1':
      openWithWinR("chrome https://www.disneyplus.com/");
      break;
    case '2':
      openWithWinR("chrome https://play.hbomax.com/");
      break;
    case '3':
      openWithWinR("https://www.youtube.com");
      break;
    case '4':
      openWithWinR("https://chat.openai.com/");
      break;
    case '5':
      openWithWinR("chrome https://twitter.com/home");
      break;
    case '6':
      openWithWinR("chrome https://watch.pokemon.com/es-es/#/");
      break;
  };
}

void LayoutTrabajar(char button) {
  switch (button) {
    case '1':
      openWithWinR("chrome");
      break;
    case '2':
      openWithWinR("firefox");
      break;
    case '3':
      openWithWinR("edge");
      break;
    case '4':
      openWithWinR("code");
      openWithWinR("spotify");
      openWithWinR("chrome");
      break;
    case '5':
      openWithWinR("chrome -incognito");
      break;
    case '6':
      openWithWinR("powershell");
      break;
  };
}

void Layout3(char button) {
  switch (button) {
    case '1':
      openWithWinR("chrome");
      break;
    case '2':
      openWithWinR("firefox");
      break;
    case '3':
      openWithWinR("opera");
      break;
    case '4':
      openWithWinR("code");
      break;
    case '5':
      openWithWinR("powershell");
      break;
    case '6':
      openWithWinR("spotify");
      break;
  };
}

void openWithWinR(const char* exe) {
  Keyboard.press(KEY_LEFT_GUI);  // Presionar la tecla Windows
  Keyboard.press('r');           // Presionar la letra "r"
  delay(delayDefault);           // Esperar un momento
  Keyboard.releaseAll();         // Liberar todas las teclas
  delay(delayDefault);           // Esperar un segundo
  Keyboard.print(exe);           // Escribir `exe`
  delay(delayDefault);           // Esperar un segundo
  Keyboard.press(KEY_RETURN);    // Presionar la tecla Enter
  Keyboard.releaseAll();         // Liberar todas las teclas
}
