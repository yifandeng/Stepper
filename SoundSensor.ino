
#include <FastLED.h>

/** BASIC CONFIGURATION  **/

//灯的数量 Number of lights
#define NUM_LEDS 150
//PIN口
#define LED_PIN 6
//声音传感器的A口 Sound sensor
#define ANALOG_READ 0

//确定声音传感器接受的最大值和最小值 Determine the maximum and minimum values accepted by the sound sensor
#define MIC_LOW 0.0
#define MIC_HIGH 1000.0
/** 111111111 */
//传感数据对平均值的影响
#define AVGLEN 5
//声音传感器的最高值时对应的传感量 The corresponding sensor amount when the highest value of the sound sensor
#define LONG_SECTOR 20

//对模式的标记位置
#define HIGH 3
#define NORMAL 2

//在重新开始测量之前，对保持当前平均声音所需要时间 The time required to maintain the current average sound before restarting the measurement
#define MSECS 30 * 1000
#define CYCLES MSECS / DELAY

/*22222222222 **/
#define DEV_THRESH 0.8

//延时 Delay
#define DELAY 1

float fscale( float originalMin, float originalMax, float newBegin, float newEnd, float inputValue, float curve);
void insert(int val, int *avgs, int len);
int compute_average(int *avgs, int len);
void visualize_music();

//定义数量
int curshow = NUM_LEDS;

/*Not really used yet. Thought to be able to switch between sound reactive
mode, and general gradient梯度 pulsing/static color*/
int mode = 0;

//对应模式展示的颜色
int songmode = NORMAL;


unsigned long song_avg;


int iter = 0;

//T在 传感器没有读取时灯带灭 The light is off when the sensor is not reading.
float fade_scale = 1.2;

//Led array
CRGB leds[NUM_LEDS];

/*33333333333333333333333 */
int avgs[AVGLEN] = {-1};


int long_avg[LONG_SECTOR] = {-1};

//到达一定模式时的平均时间 Average time when a certain mode is reached
struct time_keeping {
  unsigned long times_start;
  short times;
};

//每个周期减少或者增加的rgb的量  RGB reduced or increased per cycle
struct color {
  int r;
  int g;
  int b;
};

struct time_keeping high;
struct color Color; 

//1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111

void setup() {
  Serial.begin(9600);
  
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  for (int i = 0; i < NUM_LEDS; i++) 
    leds[i] = CRGB(255, 0, 0);
  FastLED.show(); 
  delay(1000);  

  
  for (int i = 0; i < AVGLEN; i++) {  
    insert(250, avgs, AVGLEN);
  }

  
  high.times = 0;
  high.times_start = millis();
  Color.r = 0;  
  Color.g = 0;
  Color.b = 1;
}

//2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222

void loop() {
  switch(mode) {
    case 0:
      visualize_music();
      break;
    default:
      break;
  }
    delay(DELAY);      
}



void check_high(int avg) {
  if (avg > (song_avg/iter * 1.1))  {
    if (high.times != 0) {
      if (millis() - high.times_start > 200.0) {
        high.times = 0;
        songmode = NORMAL;
      } else {
        high.times_start = millis();  
        high.times++; 
      }
    } else {
      high.times++;
      high.times_start = millis();

    }
  }
  if (high.times > 30 && millis() - high.times_start < 50.0)
    songmode = HIGH;
  else if (millis() - high.times_start > 200) {
    high.times = 0;
    songmode = NORMAL;
  }
}


void visualize_music() {
  int sensor_value, mapped, avg, longavg;
  
  
  sensor_value = analogRead(ANALOG_READ);
  
  
  if (sensor_value == 0)
    return;

  //偏离太远则更新平均值 Update the average when the deviation is too far
  mapped = (float)fscale(MIC_LOW, MIC_HIGH, MIC_LOW, (float)MIC_HIGH, (float)sensor_value, 2.0);
  avg = compute_average(avgs, AVGLEN);

  if (((avg - mapped) > avg*DEV_THRESH)) //|| ((avg - mapped) < -avg*DEV_THRESH))
    return;
  
  
  insert(mapped, avgs, AVGLEN); 
  insert(avg, long_avg, LONG_SECTOR); 

  //计算传感器的声音平均值 Calculate the sound average of the sensor
  song_avg += avg;
  iter++;
  if (iter > CYCLES) {  
    song_avg = song_avg / iter;
    iter = 1;
  }
    
  longavg = compute_average(long_avg, LONG_SECTOR);

  //确定是否进入HIGH模式 Determine whether to enter the HIGH mode
  check_high(longavg);  

  if (songmode == HIGH) {
    fade_scale = 3;
    Color.r = 5;
    Color.g = 3;
    Color.b = -1;
  }
  else if (songmode == NORMAL) {
    fade_scale = 2;
    Color.r = -1;
    Color.b = 2;
    Color.g = 1;
  }

  //确定点亮的灯数 Determine the number of lights that are lit
  curshow = fscale(MIC_LOW, MIC_HIGH, 0.0, (float)NUM_LEDS, (float)avg, -1);

  
  for (int i = 0; i < NUM_LEDS; i++) 
    
    if (i < curshow) {
      if (leds[i].r + Color.r > 255)
        leds[i].r = 255;
      else if (leds[i].r + Color.r < 0)
        leds[i].r = 0;
      else
        leds[i].r = leds[i].r + Color.r;
          
      if (leds[i].g + Color.g > 255)
        leds[i].g = 255;
      else if (leds[i].g + Color.g < 0)
        leds[i].g = 0;
      else 
        leds[i].g = leds[i].g + Color.g;

      if (leds[i].b + Color.b > 255)
        leds[i].b = 255;
      else if (leds[i].b + Color.b < 0)
        leds[i].b = 0;
      else 
        leds[i].b = leds[i].b + Color.b;  
      
    
    } else {
      leds[i] = CRGB(leds[i].r/fade_scale, leds[i].g/fade_scale, leds[i].b/fade_scale);
    }
  FastLED.show(); 
}

int compute_average(int *avgs, int len) {
  int sum = 0;
  for (int i = 0; i < len; i++)
    sum += avgs[i];

  return (int)(sum / len);

}


void insert(int val, int *avgs, int len) {
  for (int i = 0; i < len; i++) {
    if (avgs[i] == -1) {
      avgs[i] = val;
      return;
    }  
  }

  for (int i = 1; i < len; i++) {
    avgs[i - 1] = avgs[i];
  }
  avgs[len - 1] = val;
}


float fscale( float originalMin, float originalMax, float newBegin, float
    newEnd, float inputValue, float curve){

  float OriginalRange = 0;
  float NewRange = 0;
  float zeroRefCurVal = 0;
  float normalizedCurVal = 0;
  float rangedValue = 0;
  boolean invFlag = 0;


  
  if (curve > 10) curve = 10;
  if (curve < -10) curve = -10;

  curve = (curve * -.1) ; 
  curve = pow(10, curve); 
  
  if (inputValue < originalMin) {
    inputValue = originalMin;
  }
  if (inputValue > originalMax) {
    inputValue = originalMax;
  }

  
  OriginalRange = originalMax - originalMin;

  if (newEnd > newBegin){ 
    NewRange = newEnd - newBegin;
  }
  else
  {
    NewRange = newBegin - newEnd; 
    invFlag = 1;
  }

  zeroRefCurVal = inputValue - originalMin;
  normalizedCurVal  =  zeroRefCurVal / OriginalRange;   // 整理为0-1浮点数

  
  if (originalMin > originalMax ) {
    return 0;
  }

  if (invFlag == 0){
    rangedValue =  (pow(normalizedCurVal, curve) * NewRange) + newBegin;

  }
  else     
  {   
    rangedValue =  newBegin - (pow(normalizedCurVal, curve) * NewRange); 
  }

  return rangedValue;
}
