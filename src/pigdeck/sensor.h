#include <Wire.h>
#include "data.h"

//Touch
#define CLAMP(val, lo, hi) (val < lo ? lo : (val > hi ? hi : val))
#define touch_addr0 0x08 //第一个（左边）芯片的I2C地址
#define touch_addr1 0x09

struct touchval touchval0;
struct touchval touchval1;
uint8_t touchdata[32];

const uint8_t sensorPinMap0[16] = {30,31,28,29,26,27,24,25,22,23,20,21,18,19,16,17}; //第一个触摸芯片引脚到实际区域的映射数组
const uint8_t sensorPinMap1[16] = {15,14,13,12,11,10,9,8,7,6,5,4,3,2,1};//第二个触摸芯片引脚到实际区域的映射数组
const int airPinMap[6]= {28, 29, 26, 27, 14, 15};

void touchInit()
{
  Wire1.setSDA(2);
  Wire1.setSCL(3);
  Wire1.setClock(1000000);
  Wire1.begin();
}

void updateSensorValue()
{
  uint8_t buffer[64];
  //获取第一个PSoC的数据
  Wire1.requestFrom(touch_addr0, 64);
  uint8_t c = 0;
  while (Wire1.available()) {
    buffer[c] = Wire1.read();
    c ++;
  }
  memcpy(&touchval0, buffer, 64);
  //获取第二个PSoC的数据
  Wire1.requestFrom(touch_addr1, 64);
  c = 0;
  while (Wire1.available()) {
    buffer[c] = Wire1.read();
    c ++;
  }
  memcpy(&touchval1, buffer, 64);
}

void updateTouchData()
{
  int temp;
  for (int i=0; i<16; i++)
  {
    temp = touchval0.raw[i] - touchval0.baseline[i];
    temp = temp / 3 - 15; //这部分的具体处理要随实际情况而定
    touchdata[sensorPinMap0[i]] = CLAMP(temp, 0, 255);
  }
  for (int i=0; i<16; i++)
  {
    temp = touchval1.raw[i] - touchval1.baseline[i];
    temp = temp / 3 - 15;
    touchdata[sensorPinMap1[i]] = CLAMP(temp, 0, 255);
  }
}

uint8_t getTouchData(uint8_t sensorNum)
{
  return touchdata[sensorNum];
}

//Button
#define service 0
#define test 1

uint8_t buttonVal = 0;

void buttonInit()
{
  pinMode(service,INPUT);
  pinMode(test,INPUT);
}

void updateButton()
{
  buttonVal = 0;
  if (digitalRead(service) == HIGH)
  {
    buttonVal += 0x02;
  }
  if (digitalRead(test) == HIGH)
  {
    buttonVal += 0x01;
  }
}

uint8_t getButtonVal()
{
  return buttonVal;
}

void airInit()
{
  for(int i=0;i<6;i++){
    pinMode(airPinMap[i], INPUT_PULLUP);
  }
}
uint8_t airVal = 0;
void updateAir()
{
  airVal = 0;
  for(int i = 0; i < 6; i++) {
    if(digitalRead(airPinMap[i]) == LOW) {  // 反过来了
      airVal |= (1 << i);                   // LOW 表示按下
    } else {
      airVal &= ~(1 << i);
    }
  }
}
uint8_t getAirVal(){
  return airVal;
}