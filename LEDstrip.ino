//MOTION SENSOR ROOM LED STRIP
//Author: Brendan Li
//Completed: 12/5/21


//LIBRARIES
#include <FastLED.h>
#include <elapsedMillis.h>
//declartions for LEDs
#define LED_PIN     5
#define NUM_LEDS    60
#define BRIGHTNESS  50
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
#define UPDATES_PER_SECOND 100
//Button variables
byte button;
byte oldbutton = 0;
byte buttonPin = 3;
byte state = 0;
//Sensor variables
int sensor = 4;              // the pin that the sensor is atteched to
int val = 0;                 // variable to store the sensor status (value)
//small LED (non strip)
#define LED 13  // The pin the LED is connected to
//blend/fade palettes
TBlendType    currentBlending;
CRGBPalette16 currentPalette;
//Creating red/purple palettes
CRGB purple = CHSV( HUE_PURPLE, 255, 255);
CRGB red  = CHSV( HUE_RED, 255, 255);
CRGBPalette16 nextP(red, red, red, red,
                     purple, purple, purple, purple,
                     red, red, red, red,
                     purple, purple, purple, purple );
CRGBPalette16 targetPalette(nextP);
//For the fade in effect,begin with all red...(I think it looks the best)
CRGBPalette16 allRed(CRGB::Red);


//SETUP METHOD
void setup() {
    delay( 3000 ); // power-up safety delay
    pinMode(LED, OUTPUT); // Declare the LED as an output
    pinMode(buttonPin, INPUT);
    pinMode(sensor, INPUT);    // initialize sensor as an input
    
    //Begin Serial communication at a baudrate of 9600:
    Serial.begin(9600);
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    //Clear LED strip
    FastLED.clear();
    FastLED.show(); 
    //set blending and palette
    currentPalette = allRed;
    currentBlending = LINEARBLEND;
}

//VOID LOOP METHOD
void loop()
{
  //READ BUTTON STATE
  // read the state of the pushbutton value:
  button = digitalRead(buttonPin);
  if(button && !oldbutton) // same as if(button == high && oldbutton == low)
  {
    //we have a new button press
    if(state == 0) // if the state is off, turn it on
    {
      //do stuff
      state = 1;
    }
    else // if the state is on, turn it off
    {
      //do stuff
      state = 0; 
    }
    oldbutton = 1;
  }
  else if(!button && oldbutton) // same as if(button == low && oldbutton == high)
  {
    // the button was released
    oldbutton = 0;
  }

  //GEN SETUP
  FastLED.setBrightness(40);
  elapsedMillis timeElapsed;
  unsigned int interval = 60000; //~60 secs, how long LED cycle will last
  val = digitalRead(sensor);   // read sensor value

  //IF BUTTON PUSHED | LIGHT MODE...will activate LED strip no matter how dark/light it is in the room
  if(state==1){
         digitalWrite(LED, HIGH); // Turn the LED on
         if (val == HIGH){
              for(int i = 0; i < NUM_LEDS; i++){
                  leds[i]=CRGB::Red;
                  FastLED.show();
                  delay(45);
              }
              while(timeElapsed < interval){
                 uint8_t maxChanges = 15; 
                 nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);
                 run();
              }
              currentPalette = allRed;
              targetPalette = nextP;
              for(int i = NUM_LEDS; i >= 0; i--){
                leds[i]=CRGB::Black;
                FastLED.show();
                delay(45);
            }
       }
        else{
           off();
       }
  }
  else{
         //DARK MODEEEEEE...will only activate LED strip if room is dark enough; reads photoresistor value
         //READ PHOTORESISTORY VALUE
         int photo = analogRead(A0); 
         Serial.println("Analog Value :");
         Serial.println(photo);  
         digitalWrite(LED, LOW); // Turn the LED on
         if (val == HIGH && photo<25){
               for(int i = 0; i < NUM_LEDS; i++){
                  leds[i]=CRGB::Red;
                  FastLED.show();
                  delay(45);
              }
              while(timeElapsed < interval){
                 uint8_t maxChanges = 15; 
                 nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);
                 run();
              }
              currentPalette = allRed;
              targetPalette = nextP;
              for(int i = NUM_LEDS; i >= 0; i--){
                leds[i]=CRGB::Black;
                FastLED.show();
                delay(45);
            }
       }
        else{
           off();
       }
  }
 
}

//method to shut off LEDs
void off(){
     FastLED.clear();
     FastLED.show(); 
}

//method to run color palette on LED strip
void run()
{
        //SetupTotallyRandomPalette();
        static uint8_t startIndex = 0;
        startIndex = startIndex + 1; /* motion speed */
        FillLEDsFromPaletteColors(startIndex);
        FastLED.show();
        FastLED.delay(2000 / UPDATES_PER_SECOND);
}

//preset FastLED method
void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    uint8_t brightness = 255;
    
    for( int i = 0; i < NUM_LEDS; ++i) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }
}
