// #include <Adafruit_NeoPixel.h>
#include "FastLED.h"
#define NUM_LEDS 16
#define LED_INPUT_PIN 3
#define LED_TYPE    WS2812
#define BRIGHTNESS 100



CRGB leds[NUM_LEDS];
//how to design leds
//effects: stacking, color increasement, comet 
//state: rainbow, dot moving 1 side, dot moving 2 side, filling left-right
//final combo: https://www.youtube.com/watch?v=seG5tip6wBw
//comet: https://www.youtube.com/watch?v=yM5dY7K2KHM

enum  direction { FORWARD, REVERSE };


// Pattern types supported:
// enum  pattern { NONE, RAINBOW_CYCLE, THEATER_CHASE, COLOR_WIPE, SCANNER, FADE };
class Pattern{
  public: 
  virtual void Update(){};
};



class Sweeper:public Pattern{
  int interval; 
  int lastTime; //last millis
  int current_pos; 
  direction dir; 

  public: 
  Sweeper(int _interval,direction dir){
  interval = _interval;
  lastTime = 0; 
  dir = dir;
  current_pos =0;
  }

  void Update(){
   if((millis()-lastTime) > interval){
     lastTime = millis();
       if(current_pos > NUM_LEDS){
           current_pos = 0;
       }
     leds[current_pos]+=CRGB::Blue;
     leds[current_pos-1]=CRGB::Black;
     current_pos++;
    //  move_on();
     FastLED.show();   
   }
    }
    //   void move_on(){
    //       if(dir == FORWARD){
    //         leds[current_pos-1]=CRGB::Black; 
    //        }else{
    //        leds[current_pos+1]=CRGB::Black; 
    //        }
    //                 current_pos += dir == FORWARD ? 1:-1; 
    //        }

  

};
// Patern directions supported:
class NeoPixelLed{
    public:
      Pattern *patterns[50];
   int current_active_pattern; 
   int num_patterns;
    NeoPixelLed(){
        current_active_pattern = 0; 
        num_patterns = 0; 
    }

   void Update(){
     for(int i = current_active_pattern; i < num_patterns; i++){
       patterns[i]->Update();
     } 
   } 
   void addPattern(Pattern *p,bool isStack=false){ 
patterns[num_patterns]=p;
     current_active_pattern = isStack ? current_active_pattern : num_patterns;
     num_patterns++;
   }
}; 


NeoPixelLed led1;
Sweeper *sweeper1 = new Sweeper(500,FORWARD);
// Sweeper sweeper2(100);


// Initialize everything and prepare to start
void setup()
{
  Serial.begin(9600);
  FastLED.addLeds<LED_TYPE, LED_INPUT_PIN>(leds, NUM_LEDS); 
  FastLED.setBrightness(BRIGHTNESS);
  
led1.addPattern(sweeper1);
}

// Main loop
void loop()
{ 
  led1.Update();
// sweeper1->Update(); 

}



