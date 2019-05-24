#include <SPI.h>
#include "mcp_can.h"
#include "RGBdriver.h"

const int SPI_CS_PIN = 9;

MCP_CAN CAN(SPI_CS_PIN);



//PID: Parameter ID
//CAN: Controller Area Network
//MIL: Malfunction Indicator Light
//DTC: Diagnostic Trouble Code
//
//
//This sketch relies on standard OBDII protocols over a CAN bus.
//It will work with any vehicle with a CAN bus.
//As far as I know, no proprietary PIDs are used.


#define PID_ENGINE_RPM       0x0C
#define PID_MIL         0x01

#define CAN_ID_PID          0x7DF

#define CLK 8//pins definitions for the LED driver        
#define DIO 9

RGBdriver Driver(CLK, DIO);

#define REDLINERPM 5500
#define IDLERPM 750

boolean MIL = false;

void set_mask_filt() //initializes masks and filters for CAN data
{
  /*
     set mask, set both the mask to 0x3ff
  */
  CAN.init_Mask(0, 0, 0x7FF);
  CAN.init_Mask(1, 0, 0x7FF);
  /*
     set filter, we can receive id from 0x04 ~ 0x09
  */
  CAN.init_Filt(0, 0, 0x7E8);
  CAN.init_Filt(1, 0, 0x7E8);
  CAN.init_Filt(2, 0, 0x7E8);
  CAN.init_Filt(3, 0, 0x7E8);
  CAN.init_Filt(4, 0, 0x7E8);
  CAN.init_Filt(5, 0, 0x7E8);
}

void sendPid(unsigned char __pid) //Sends pid on CAN bus with no data
{
  unsigned char tmp[8] = {0x02, 0x01, __pid, 0, 0, 0, 0, 0};
  //Serial.print("SEND PID: 0x");
  //Serial.println(__pid, HEX);
  CAN.sendMsgBuf(CAN_ID_PID, 0, 8, tmp);
}
double getBrightness() {
  //TODO: reads voltage from interior light dimmer switch to determine brightness level
  return 0.65;
}
void setup() //establshes CAN communication
{
  Serial.begin(115200);
  while (CAN_OK != CAN.begin(CAN_500KBPS))    // init can bus : baudrate = 500k
  {
    Serial.println("CAN BUS Shield init fail");
    Serial.println(" Init CAN BUS Shield again");
    delay(100);
  }
  Serial.println("CAN BUS Shield init ok!");
  set_mask_filt();
}

void checkMIL() //TODO: reads DTC PID to determine if MIL is on
{
  sendPid(PID_MIL);
  unsigned char len = 0;
  unsigned char buf[8];
  if (CAN_MSGAVAIL == CAN.checkReceive())                  // check if get data
  {
    CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf
    int id = CAN.getCanId();
    //if (id == ?? ?? ) {
    //process mil data
    //}
  }
}
void loop()
{
  //checkMIL();
  sendPid(PID_ENGINE_RPM);
  boolean flash = false;
  unsigned char len = 0;
  unsigned char buf[8];
  double num = 0.0;
  int red, green, blue = 0;
  double brightness = 1.0;

  if (!MIL && (CAN_MSGAVAIL == CAN.checkReceive()))                // check if get data
  {
    CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf
    int id = CAN.getCanId();
    if (id == 2024) { //Engine RPM
      int a = buf[3];
      int b = buf[4];
      double rpm = ((256 * a) + b) / 4;
      if (rpm > REDLINERPM - 200) flash = true;
      num = rpm / REDLINERPM;
      //num=((IDLERPM*num)/((REDLINERPM-IDLERPM)/IDLERPM))-(REDLINERPM/((REDLINERPM-IDLERPM)/IDLERPM));
    }

    if (num > 1) num = 1;
    brightness = (flash ? 1 : getBrightness());
    red = 255 * brightness;
    green = (255 - (num * 255)) * brightness;
    blue = (255 - (2 * num * 255)) * brightness;
    Driver.begin();
    Driver.SetColor(red, blue, green);
    Driver.end();

    if (flash) {
      //for (int i = 0; i < 10; i++) {
      delay(50);
      Driver.begin();
      Driver.SetColor(0, 0, 0);
      Driver.end();
      delay(50);

    }
    else if (MIL) {
      red = 245;
      green = 173;
      blue = 47;
      Driver.begin();
      Driver.SetColor(red, blue, green);
      Driver.end();
    }
  }
}
