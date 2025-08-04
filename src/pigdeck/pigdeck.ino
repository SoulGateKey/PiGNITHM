#include <Wire.h>
#include <wiring_private.h>
#include "Adafruit_TinyUSB.h"
#include "report.h"
#include "sensor.h"
#include <Adafruit_NeoPixel.h>
#define NUM_LEDS 31
#define NUM_AIR 6
#define DATA_PIN 12
#define LRGB_PIN 13
#define RRGB_PIN 8

Adafruit_NeoPixel panelRGB(NUM_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel LRGB(NUM_AIR, LRGB_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel RRGB(NUM_AIR, RRGB_PIN, NEO_GRB + NEO_KHZ800);
 
Adafruit_USBD_HID usb_hid(desc_hid_report, sizeof(desc_hid_report), HID_ITF_PROTOCOL_NONE, 2, true);

void setup() {
  touchInit();
  buttonInit();
  
  TinyUSBDevice.setSerialDescriptor("SGK"); //设定USB设备序列号
  TinyUSBDevice.setID(0x1973,0x2001); //设定USB设备vid，pid
  TinyUSBDevice.setProductDescriptor("PiGNITHM"); //设定USB设备产品名
  TinyUSBDevice.setManufacturerDescriptor("PigQwQ.moe"); //设定USB设备制造商名
  usb_hid.setPollInterval(1); //设定hid报文间隔为1ms，即最大1000hz回报率
  usb_hid.setReportCallback(get_report_callback, set_report_callback); //当电脑向手台发送数据时会调用set_report_callback进行处理
  usb_hid.begin();
  while (!TinyUSBDevice.mounted()) delay(1);  //如果没插入则等待 wait till plugged
  while (!usb_hid.ready()) delay(1);  
}
struct inputdata data_tx;
struct usb_output_data_1 data_rx_1;
struct usb_output_data_2 data_rx_2;
void loop() {
  updateSensorValue(); //从触摸芯片获取值
  updateTouchData(); //将值处理后放入TouchData数组
  
  updateButton();
  data_tx.Buttons = getButtonVal();
  data_tx.IRValue = getAirVal();
  for (int i = 0; i < 32; i++)
  {
    data_tx.TouchValue[i] = getTouchData(i); //获取触摸数值
  }
  
  usb_hid.sendReport(0, &data_tx, sizeof(data_tx));
}

uint16_t get_report_callback (uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  // not used in this example
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;
  return 0;
}
void set_report_callback(uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  // This example doesn't use multiple report and report ID
  (void) report_id;
  (void) report_type;

  //buffer即为从电脑收到的数据，首先判断第1byte的数值，来选择解析为data_rx_1还是data_rx_2 (see definition in report.h)
  if (buffer[0] == 0)
  {
    memcpy(&data_rx_1, buffer, bufsize);
  }
  else
  {
    memcpy(&data_rx_2, buffer, bufsize);
  }
}

// void RGBset() {
//   FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
//   //FastLED.addLeds<WS2812B, air_right_pin, GRB>(AIR_RIGHT, air_right_num);
//   //FastLED.addLeds<WS2812B, air_left_pin, GRB>(AIR_LEFT, air_left_num);
// }

void setup1(){
  airInit();
  panelRGB.begin();
  RRGB.begin();
  LRGB.begin();
  panelRGB.setBrightness(255);
  RRGB.setBrightness(255);
  LRGB.setBrightness(255);
}

void loop1() {
  updateAir();
  for (int i = 0; i<20; i++){
    panelRGB.setPixelColor(i, data_rx_1.TouchArea[i].R, data_rx_1.TouchArea[i].G, data_rx_1.TouchArea[i].B);
  }
  for (int i = 0; i<11; i++){
    panelRGB.setPixelColor(i+20, data_rx_2.TouchArea[i].R, data_rx_2.TouchArea[i].G, data_rx_2.TouchArea[i].B);
  }
  for (int i = 0; i<6; i++){
    LRGB.setPixelColor(i, data_rx_2.LeftAir.R, data_rx_2.LeftAir.G, data_rx_2.LeftAir.B);
  }
  for (int i = 0; i<6; i++){
    RRGB.setPixelColor(i, data_rx_2.Rightair.R, data_rx_2.Rightair.G, data_rx_2.Rightair.B);
  }
  panelRGB.show();
  LRGB.show();
  RRGB.show();
}