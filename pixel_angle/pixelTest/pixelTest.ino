#include <Adafruit_NeoPixel.h>
#define PIN 6


Adafruit_NeoPixel ring = Adafruit_NeoPixel(24, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  
  ring.begin();
  ring.show();

  Serial.println(ring.numPixels());

}

int i = 0;
int color = ring.Color(0,50,0);
int pixel = 0;

void loop() {
    int angle = 180;
  
//    ring.setPixelColor(pixelAngle(angle),color);
//    ring.show();
  for(i=-180;i<180;i++){
  int prev_pixel = pixel;
  
  ring.setPixelColor(prev_pixel, 0);
//  ring.show();
  
  pixel = pixelAngle(i);
//  Serial.printf("%d,%d\n\r",i, color);
  ring.setPixelColor(pixel, color);
  ring.show();
  
//  delay(5);
 
  }
}

int pixelAngle(int angle){

  int pixel = angle*24/360;
  pixel+=12;
  return pixel;
}

