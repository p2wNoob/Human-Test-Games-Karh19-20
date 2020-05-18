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

void led_flashing(int pin, float sec){ 
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

void printMainMenu()
{
  lcd.setCursor(0,0);
  lcd.print("Button 1: Simon Says");
  lcd.setCursor(0,1);
  lcd.print("Button 2: Guitar Hero");
  lcd.setCursor(0,2);
  lcd.print("Button 3: Reaction Test");
  lcd.setCursor(0,3);
  lcd.print("Button 4: Quit");
}

void GameStartSequence() 
{
  for(int i = 3; i > 0; i--)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Game starts in:");
    lcd.print(i);
    delay(1000);
  }
}

void printCurrentScore(int score)
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Current Score:");
  lcd.print(score);
}


void SimonSays()
{
  gameOver = 0;
  colorsCount = 0;
  GameStartSequence();
  while(!gameOver){
    printCurrentScore(colorsCount);
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
        lcd.print("Game Over!");
        lcd.setCursor(0,1);
        lcd.print("Final score: ");
        lcd.print(colorsCount-1);
        gameOver = 1; 
        break;
      }
      else if(playerInput != 0){
        i++;  
      }
    }
  }
  lcd.setCursor(0,2);
  lcd.print("Press any button");
  lcd.setCursor(0,3);
  lcd.print("to continue");
  delay(1000); //wait so user doenst accindetaly skip score
  while(!b1.isReleased() && !b2.isReleased() && !b3.isReleased() && !b4.isReleased())
  {
    //wait until button press
  }
  return;
}

void GuitarHero()
{
 gameOver = 0;
 score = 0;
 GameStartSequence(); 
  
 while(!gameOver){
    lcd.clear();
    printCurrentScore(score);  
    if(currentLed == 0)
    prevLed = currentLed;
    currentLed = random(9, 13);
    if(prevLed == currentLed){
      delay(300); 
    }
    led_flashing(currentLed, 0.3);
    unsigned long start_time = millis();
    on_time = 0;
    unsigned long elapsed_time = 0;
    while(elapsed_time < 300){
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
        lcd.print("Game Over!");
        lcd.setCursor(0,1);
        lcd.print("Final Score: ");
        lcd.print(score);  
        gameOver = 1; 
        break;
      }
      else if(playerInput != 0){
        score++;
            on_time = 1;
            break;
      }
    }
    if(!on_time && gameOver != 1){
        lcd.clear();
        lcd.print("Game Over!");
        lcd.setCursor(0,1);
        lcd.print("Final Score: ");
        lcd.print(score);
        gameOver = 1;
    }
  }
  lcd.setCursor(0,2);
  lcd.print("Press any button");
  lcd.setCursor(0,3);
  lcd.print("to continue");
   delay(1000); //wait so user doenst accindetaly skip score
  while(!b1.isReleased() && !b2.isReleased() && !b3.isReleased() && !b4.isReleased())
  {
    //wait until button press
  }
  return; 
}

void ReactionTest()
{
  unsigned long startTime = 0;
  unsigned long endTime = 0;
  unsigned long duration = 0;
  byte timerRunning = 0;
  float wait = random(1, 500) / 100.0;
  byte led = random(9,13); 
  byte button = led - 6; //gets button pin

  GameStartSequence();
  delay(wait);
  led_flashing(led, 0.5);
  while(1)
  {
    if (timerRunning == 0 && digitalRead(button) == LOW){ // button pressed & timer not running already
    startTime = millis();
    timerRunning = 1;
    }
    if (timerRunning == 1 && digitalRead(button) == HIGH){ // timer running, button released
    endTime = millis();
    timerRunning = 0;
    duration = endTime - startTime;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Score: ");
    lcd.print(duration);
    break;
    }
  }
  lcd.setCursor(0,2);
  lcd.print("Press any button");
  lcd.setCursor(0,3);
  lcd.print("to continue");
  delay(500); //wait so user doenst accindetaly skip score
  while(!b1.isReleased() && !b2.isReleased() && !b3.isReleased() && !b4.isReleased())
  {
    //wait until button press
  }
  return; 
}

void loop()
{
  while(1)
  {
      printMainMenu();
      if (b1.isReleased()) {
        SimonSays();
      }
      else if (b2.isReleased()) {
        GuitarHero();
      }
      else if (b3.isReleased()) {
        ReactionTest();
      }
      else if (b4.isReleased()) {
        lcd.clear();
        while(1){}
      }
  }
}
