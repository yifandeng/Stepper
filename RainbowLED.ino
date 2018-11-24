#include <Adafruit_NeoPixel.h>
#define PIN 6
// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number
// Parameter 3 = pixel type flags, add together as needed:
// NEO_KHZ800 800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(100, PIN, NEO_GRB + NEO_KHZ800);
void setup() {
   strip.begin();
   strip.show(); // 初始化所有的灯
}
void loop() {
   //依次亮 RGB light uo in turn
   colorWipe(strip.Color(255, 0, 0), 50); // Red
   colorWipe(strip.Color(0, 255, 0), 50); // Green
   colorWipe(strip.Color(0, 0, 255), 50); // Blue
   
   theaterChase(strip.Color(127, 127, 127), 50); // White
   theaterChase(strip.Color(127, 0, 0), 50); // Red
   theaterChase(strip.Color( 0, 0, 127), 50); // Blue
   rainbow(20);
   rainbowCycle(20);
   theaterChaseRainbow(50);
}

void colorWipe(uint32_t c, uint8_t wait) {
   for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
   }
}
//彩虹灯 rainbow light
void rainbow(uint8_t wait) {
   uint16_t i, j;
   for(j=0; j<256; j++) {
      for(i=0; i<strip.numPixels(); i++) {
         strip.setPixelColor(i, Wheel((i+j) & 255));
      }
      strip.show();
      delay(wait);
   }
}

void rainbowCycle(uint8_t wait) {
   uint16_t i, j;
   for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
      for(i=0; i< strip.numPixels(); i++) {
         strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
      }
      strip.show();
      delay(wait);
   }
}

void theaterChase(uint32_t c, uint8_t wait) {
   for (int j=0; j<10; j++) { //do 10 cycles of chasing
      for (int q=0; q < 3; q++) {
         for (int i=0; i < strip.numPixels(); i=i+3) {
             strip.setPixelColor(i+q, c); //turn every third pixel on
          }
       strip.show();
       delay(wait);
       for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, 0); //turn every third pixel off
       }
       }
   }
}

void theaterChaseRainbow(uint8_t wait) {
   for (int j=0; j < 256; j++) { // cycle all 256 colors in the wheel
      for (int q=0; q < 3; q++) {
         for (int i=0; i < strip.numPixels(); i=i+3) {
             strip.setPixelColor(i+q, Wheel( (i+j) % 255)); //turn every third pixel on
          }
          strip.show();
          delay(wait);
          for (int i=0; i < strip.numPixels(); i=i+3) {
             strip.setPixelColor(i+q, 0); //turn every third pixel off
          }
       }
   }
}

// 全彩呼吸灯 breathing light
uint32_t Wheel(byte WheelPos) {
   if(WheelPos < 85) {
       return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
    } else if(WheelPos < 170) {
       WheelPos -= 85;
       return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    } else {
       WheelPos -= 170;
       return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
}
