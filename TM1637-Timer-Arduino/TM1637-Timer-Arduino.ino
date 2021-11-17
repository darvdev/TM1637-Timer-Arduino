#include <SevenSegmentFun.h>

#define CLK 7
#define DIO 8

SevenSegmentFun display(CLK, DIO);

volatile long credits = 0;
unsigned long creditsPreviousMillis = 0;
const int creditsInterval = 1000;
const int coinpin = 2;
const int relaypin = 4;
const int spkpin = 5;
const int coin = 65;
const unsigned long startup = 60000;

uint8_t hours = 0;
uint8_t minutes =  0;
uint8_t seconds = 0;

void setup() {
  Serial.begin(9600);
  pinMode(coinpin, INPUT_PULLUP);
  pinMode(relaypin, OUTPUT);
  pinMode(spkpin, OUTPUT);
  digitalWrite(relaypin, HIGH);
  attachInterrupt(digitalPinToInterrupt(coinpin), coinISR, FALLING);
  
  display.init();
  display.clear();

  for (int i = 0 ; i <= (startup / 1000) ; i++ ){    
    display.snake();
    if (credits > 0){
      break;
    }
  }
  
  if (credits <= 0){
    digitalWrite(relaypin, LOW);
    display.clear();
    display.print("   0");
  }
  
}

void loop() {

  unsigned long currentMillis = millis();   
  if (currentMillis - creditsPreviousMillis >= creditsInterval){
      creditsPreviousMillis = currentMillis;
      if (credits <= 0){
        credits = 0;
        digitalWrite(relaypin, LOW);
      }else{
        credits -= 1;     
        Serial.println(credits);
        
        seconds = credits % 60;
        minutes = ((credits - seconds) / 60) % 60;
        hours =  ((credits - (seconds + (minutes * 60))) / 3600) % 60;


        if (credits == 60){
          tone(spkpin, 2100, 5000);
        }
        if (credits <= 10){
          if (credits != 0){
            tone(spkpin, 2100, 650);
          }else{
            tone(spkpin, 2100, 2000);
          }
        }
        
        display.clear();
        if (hours == 0){
          if (minutes == 0){
            if (credits > 9){
              display.setCursor(0,2);
            }else{
              display.setCursor(0,3);
            }
            display.print(credits);
          }else{
            display.printTime(minutes,seconds);
          }
        }else{
          display.printTime(hours, minutes);
          display.blink(1000,1);
        }
        
        if (digitalRead(relaypin) == LOW){
          digitalWrite(relaypin, HIGH);
        }
        
      }
  } 
}

void coinISR(){
  delay(1); 
  if (digitalRead(coinpin) == LOW){
    credits += coin;
  }
}
