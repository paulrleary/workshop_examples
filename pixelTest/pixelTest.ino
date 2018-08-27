#include <Adafruit_NeoPixel.h>
#define PIN 6


Adafruit_NeoPixel ring = Adafruit_NeoPixel(24, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  
  ring.begin();
  ring.show();

  Serial.println(ring.numPixels());

}

int i = 0;
int color = ring.Color(255,255,255);

void loop() {

  for(i=0;i<360;i++){

  int pixel = pixelAngle(i);
  Serial.printf("%d,%d\n\r",i, color);
//  ring.setPixelColor(pixel, color);
//  ring.show();
//  
//  delay(100);
//  
//  ring.setPixelColor(pixel, 0);
//  ring.show();

//delay(100);
//  i++;
//  
//  if(i>=24){
//    i=0;
//  }
 
  }
}

int pixelAngle(int angle){

  int pixel = angle*24/360;
  return pixel;
}

