#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN            7

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      4

// initialise NeoPixels
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_RGB + NEO_KHZ800);


// Define the RGB values for the 4 pots:
int r1, g1, b1;
int r2, g2, b2;
int r3, g3, b3;
int r4, g4, b4;

// intensity level (brightness) for LED 2 [drift probability]
float intensity2;

// variables for the control inputs
int center;
int gravity;
int stretch_abs;
int taste;

// metaparameter names
float drift_size;
float probability;

// raw potentiometer readings variables
int pot_1, pot_2, pot_3, pot_4;

// smoothing rate
float smooth_rate = 0.07;

void setup() {
  pixels.begin(); // This initializes the NeoPixel library.
  Serial.begin(9600);
  startup(200);     // startup lighting function. 
}

void loop() {

  // read analog pin values for each pot.
  pot_1  = analogRead(A0);
  pot_2 = analogRead(A1);
  pot_3 = analogRead(A2);
  pot_4 = analogRead(A3);
 
  

  // average the values, to clean the input a bit, constrain
  center += (pot_1 - center) * smooth_rate;
  center = constrain(center, 0, 677);
  gravity += (pot_2 - gravity) * smooth_rate;
  gravity = constrain(gravity, 0, 668);
  stretch_abs += (pot_3 - stretch_abs) * smooth_rate;
  stretch_abs = constrain(stretch_abs, 0, 690);
  taste += (pot_4 - taste) * smooth_rate;
  taste = constrain(taste, 0, 690);


  

  
  

  //functions for determining the r,g,b values for each pot.
  // and mapping for meta-parameters
  
  int mapping_stretch_abs = map(stretch_abs, 0, 690, 0, 255);
  float stretch_abs_scaled = mapIntToFloat(mapping_stretch_abs, 0, 255, -0.25, 0.25);
  stretch_abs_scaled = abs(stretch_abs_scaled);

  float grav_mag = abs(mapIntToFloat(gravity, 0, 668, -100.0, 100.0));
 
  drift_size = (0.002726405204711015*grav_mag) + 
               (0.00011211730008719635*pow(grav_mag, 2)) + 
               (0.7474992616888515*stretch_abs_scaled) - 
               (0.25274739199860224*pow(stretch_abs_scaled, 2)) - 
               (5.100864445054601*pow(stretch_abs_scaled, 3)) - 0.001773 + 
               (0.0000005408160449449254*pow(grav_mag, 3));
  drift_size = constrain(mapFloat(drift_size, -0.001595, 2.024234, 0.0, 1.0), 0.0, 1.0);
  

  // LED_1 
  // if grav negative, drift size gives more red than green
  if (gravity <= 334) {
      r1 = mapFloatToInt(drift_size, 0.0, 1.0, 0, 240);
      r1 = constrain(r1, 0, 240);
      g1 = mapFloatToInt(drift_size, 0.0, 1.0, 0, 60);
               
  }

  // if grav positive, drift size gives more green than red
  else {
      g1 = mapFloatToInt(drift_size, 0.0, 1.0, 0, 240);
      constrain(g1, 0, 240);
      r1 = mapFloatToInt(drift_size, 0.0, 1.0, 0, 60);
  }

  // blue is always mapped to center
  b1 = map(center, 0, 668, 20, 200);

   
  // LED_2
  // level of knob 2 and 3 determine probability. Prob is mapped to intensity of LED at 
  // knob 2. The mix of G, R, is based on probability, higher prob = more green, brighter
  // less probability = more red, less bright.
  
  probability = 1.076606679135022*grav_mag - 
                0.006708752151172304*pow(grav_mag, 2) + 
                0.00007127543436653731*pow(grav_mag, 3) + 
                2.4981759755230417*stretch_abs_scaled - 1358.1795672353812*pow(stretch_abs_scaled, 2) + 
                3603.3932449045255*pow(stretch_abs_scaled, 3) + 0.264784;
  probability = mapFloat(probability, 0.265302, 112.113884, 0.0, 100.0);

  intensity2 = constrain((probability / 100.0), 0.0, 1.0);

  float red_mix = constrain(mapFloat(stretch_abs_scaled, 0, 0.25, 0, 0.6), 0, 0.6);
  float other_mix = 1.0 - red_mix;

  float blue2 = 240 * intensity2 * other_mix;
  
  float red2;
  float thresh = 0.4;
  if (intensity2 >= thresh) {
    red2 = 240 * intensity2;
  }
  else if (intensity2 < thresh) {
    red2 = mapFloat(stretch_abs_scaled, 0, thresh, 0, 100.0);
  }
  
  float green2 = 240 * intensity2 * other_mix;
  
  b2  = int(blue2);    
  g2 = int(green2);
  r2 = int(red2);

 
  // LED_3

  r3 = mapFloatToInt(stretch_abs_scaled, 0.0, 0.25, 20, 100);

  // b3
  if (taste == 0){
    b3 = 20;
    g3 = 10;  
  }
  else if (taste >= 1 && taste < 76){
    b3 = map(taste, 0, 160, 20, 180);
    g3 = 0;
  }
  else if (taste >= 76 && taste < 316){
    g3 = map(taste, 161, 294, 20, 180);
    b3 = 0;
  }
  else if (taste >= 316 && taste < 553){
    b3 = map(taste, 295, 427, 20, 180);
    g3 = 0;
  }
  else if (taste >= 553 && taste < 794){
    g3 = map(taste, 428, 559, 20, 180);
    b3 = 0;
  }
  else {
    b3 = map(taste, 560, 690, 20, 180);
    g3 = 0;
  }

  //b4
  r4 = map(taste, 0, 690, 20, 235);
  g4 = map(taste, 0, 690, 190, 60);
  b4 = map(taste, 0, 690, 80, 10);


  
  // pixles are constantly lit, with changing r,g,b values: 
  pixels.setPixelColor(0, pixels.Color(r1, g1, b1));
  pixels.setPixelColor(1, pixels.Color(r2, g2, b2));
  pixels.setPixelColor(2, pixels.Color(r3, g3, b3));
  pixels.setPixelColor(3, pixels.Color(r4, g4, b4)); 
  pixels.show();

  
}


// functions
void startup(int brightness) {
  for(int j = 0; j<brightness; j+= 3){
    for(int i = 0; i<NUMPIXELS; i++){
      pixels.setPixelColor(i, pixels.Color(j, j, j));
      pixels.show();
      delay(10);
    }
  }
  for(int j = brightness; j>0; j-= 10){
    for(int i = 0; i<NUMPIXELS; i++){
      pixels.setPixelColor(i, pixels.Color(j, j, j));
      pixels.show();
      delay(10);
    }
  }
}

// modified from:  https://forum.arduino.cc/index.php?topic=3922.msg30006#msg30006
float mapIntToFloat(long x, long in_min, long in_max, float out_min, float out_max)
{
 return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max)
{
 return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}

int mapFloatToInt(float x, float in_min, float in_max, int out_min, int out_max)
{
 return (int)((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}

