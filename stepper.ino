#include <Stepper.h>

// 这里设置步进电机旋转一圈是多少步 Set the steps to rotate the stepper motor
#define STEPS 200

Stepper stepper(STEPS, 8, 9, 10, 11);

int total = 0;
int v1 = 0;
int prev = 0;
int absval = 0;
const byte trigPin = 2;
const int echoPin = 3;
unsigned long d;
bool gameState = false;
bool tempPause = true;
int endTime = 0;

void endGame()
{
    total = 0;
    v1 = 0;
    prev = 0;
    absval = 0;
    gameState = false;
    tempPause = true;
    endTime = 0;
}

unsigned long ping()
{
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(0);
    digitalWrite(trigPin, LOW);
    return pulseIn(echoPin, HIGH);
}


void setup()
{
  
    stepper.setSpeed(STEPS);

    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);


    Serial.begin(9600);

    total = 0;
    v1 = 0;
    prev = 0;
    absval = 0;
    
      


}

void loop()
{
    d = ping() / 58;
     Serial.println(d);
    
    delay(200);
    if(d <= 100)
    {
        v1 = d;
    }

    // 判断游戏状态 Judge state
    if(gameState == false && v1 != 0)
    {
        gameState = true;
        tempPause = false;
        endTime = 0;
        stepper.step(-8000);
        total = 8000;
    }

    if(tempPause == true && endTime <= 20000)
    {
        endTime = endTime + 100;
        delay(100);
    }
    else if( tempPause == true && endTime > 10000)
    {
        stepper.step(total);
        endGame();
    }
        // 游戏状态为真时开始后续流程 state=true, start the follow-up process
    if(gameState == true)
    {
        tempPause = true;
            // 设定电机上升的上限和下限 Set the upper and lower limits of the motor rise
        if(total < 11800 && total > 500)
        {
            if(d <= 100)
            {   // 移动前先检测移动后是否超出安全界限 Check if the safety limit is exceeded after moving
                if((total - absval * 30) >= 0 && (total + absval * 90) <= 11800)
                {   // 判断前一次的手部距离及上升下降状态 Determine the previous hand distance and ascending and descending status
                    if(prev > v1 && prev != 0)
                    {  
                        stepper.step(absval * 30);
                        int endTime = 0;
                        tempPause = false;
                        total = total - absval * 30;
                        

                    }
                    else if(prev < v1 && prev != 0)
                    {
                        stepper.step(-90 * absval);
                        int endTime = 0;
                        tempPause = false;
                        total = total + absval * 90;
                        
                    }
                }
            }

            absval = v1 - prev;


            if(absval < 0)
            {
                absval = -absval;
            }
            // 过滤轻微抖动带来的影响 Filter the effects of slight jitter
            if(absval >= 3)
            {
                prev = v1;
            }

        }
        else if(total <= 500)
        {

            if(d <= 100)
            {
                if(prev > v1 && prev != 0)
                {

                }
                else if(prev < v1 && prev != 0)
                {
                    stepper.step(-90 * absval);
                    int endTime = 0;
                    tempPause = false;
                    total = total + absval * 90;
                    Serial.println(-absval);
                }
            }

            absval = v1 - prev;


            if(absval < 0)
            {
                absval = -absval;
            }

            if(absval >= 3)
            {
                prev = v1;
            }


        }
        else if(total >= 11800)
        {

            if(d <= 100)
            {
                if(prev > v1 && prev != 0)
                {
                    stepper.step(absval * 30);
                    int endTime = 0;
                    tempPause = false;
                    total = total - absval * 30;
                    Serial.println(absval);

                }
                else if(prev < v1 && prev != 0)
                {
                }
            }

            absval = v1 - prev;


            if(absval < 0)
            {
                absval = -absval;
            }

            if(absval >= 3)
            {
                prev = v1;
            }

        }

    }

}
