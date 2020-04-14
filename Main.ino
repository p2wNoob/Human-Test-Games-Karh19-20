#include "LCD.h"
#include "LiquidCrystal_I2C.h"

class NonBlockDelay {  
    unsigned long iTimeout;  
  public:  
    void Delay (unsigned long t){  
      iTimeout = millis() + t;  
      return;  
  };  
    bool Timeout (void){  
      return (iTimeout < millis());  
  }  
    unsigned long Time(void){
      return iTimeout;
    }
}; 

NonBlockDelay d;

class Button {
    private:
        bool _state;
        uint8_t _pin;

    public:
        Button(uint8_t pin) : _pin(pin) {}

        void begin() {
            pinMode(_pin, INPUT_PULLUP);
            _state = digitalRead(_pin);
        }

        bool isReleased() {
            bool v = digitalRead(_pin);
            if (v != _state) {
                _state = v;
                if (_state) {
                    return true;
                }
            }
            return false;
        }
};

void led_flashing(int pin, int sec){ 
    digitalWrite(pin,HIGH);
    delay(1000*sec);
    digitalWrite(pin,LOW);
}

Button b4(6);
Button b3(5);
Button b2(4);
Button b1(3);
int led_yellow = 12;
int led_red = 11;
int led_white = 10;
int led_blue = 9;
int gameOver = 0;
int gameOver2 = 0;
int score = 0;
int on_time;
int currentLed = 0;
int prevLed = 0;
int playerInput = 0;
int colorsCount = 0;
int Colors[100];


LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7);

void setup()
{
  lcd.begin(20,4);
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.setCursor(0,0);
  lcd.print("Welcome!");
  lcd.setCursor(0,1);
  lcd.print("Simon Says Game");
  lcd.setCursor(0,2);
  lcd.print("Will Start in 5 secs");
  delay(5000);
  lcd.clear();
  b4.begin();
  b3.begin();
  b2.begin();
  b1.begin();
  pinMode(led_yellow, OUTPUT);
  pinMode(led_red, OUTPUT);
  pinMode(led_white, OUTPUT);
  pinMode(led_blue, OUTPUT);
  Serial.begin(9600);
  randomSeed(analogRead(0));
}

void loop()
{  
  if(!gameOver){
    //adding a new color to the sequence
    Colors[colorsCount++] = random(9,13);
    //going through all the colors
    delay(600);
    for(int i = 0; i < colorsCount; ++i){
      if(i > 0){
        if(Colors[i] == Colors[i-1]){
          delay(300); 
        }
      }
      led_flashing(Colors[i], 1);
    }
    //checking the input of the user
    for(int i = 0; i < colorsCount;){
      playerInput = 0;
      if (b4.isReleased()) {
        playerInput = led_yellow;
      }
      if (b3.isReleased()) {
        playerInput = led_red;
      }
      if (b2.isReleased()) {
        playerInput = led_white;
      }
      if (b1.isReleased()) {
        playerInput = led_blue;
      }
      if(Colors[i] != playerInput && playerInput != 0){
        lcd.clear();
        lcd.print("Game 1 Over!");
        lcd.setCursor(0,1);
        lcd.print("Score: ");
        lcd.print(colorsCount);
        Serial.println("\nGame 1 over!");
            Serial.print("score: ");
            colorsCount--;
            Serial.print(colorsCount);
            gameOver = 1; 
          break;
      }
      else if(playerInput != 0){
        i++;  
      }
    }
  }
  else if(!gameOver2){
    if(currentLed == 0)
    prevLed = currentLed;
    currentLed = random(9, 13);
    if(prevLed == currentLed){
      delay(300); 
    }
    led_flashing(currentLed, 1);
    unsigned long start_time = millis();
    on_time = 0;
    unsigned long elapsed_time = 0;
    while(elapsed_time < 3000){
        unsigned long current_time = millis();
        elapsed_time = current_time - start_time;
      playerInput = 0;
      if (b4.isReleased()) {
        playerInput = led_yellow;
      }
      if (b3.isReleased()) {
        playerInput = led_red;
      }
      if (b2.isReleased()) {
        playerInput = led_white;
      }
      if (b1.isReleased()) {
        playerInput = led_blue;
      }
      if(currentLed != playerInput && playerInput != 0){
        lcd.clear();
        lcd.print("Game 2 Over");
        lcd.setCursor(0,1);
        lcd.print("Score: ");
        lcd.print(score);  
        Serial.println("\nGame 2 over!");
            Serial.print("score: ");
            Serial.print(score);
            gameOver2 = 1; 
          break;
      }
      else if(playerInput != 0){
        score++;
            on_time = 1;
            break;
      }
    }
    if(!on_time && gameOver2 != 1){
      Serial.println("\nGame 2 over!");
      Serial.print("score: ");
        Serial.print(score);
        gameOver2 = 1;
    }
    
  } 
  delay(10); 
}
