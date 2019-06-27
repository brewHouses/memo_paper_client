/**
  ******************************************************************************
    @file    epd.h
    @author  Waveshare Team
    @version V1.0.0
    @date    23-January-2018
    @brief   This file provides e-Paper driver functions
              void EPD_SendCommand(byte command);
              void EPD_SendData(byte data);
              void EPD_WaitUntilIdle();
              void EPD_Send_1(byte c, byte v1);
              void EPD_Send_2(byte c, byte v1, byte v2);
              void EPD_Send_3(byte c, byte v1, byte v2, byte v3);
              void EPD_Send_4(byte c, byte v1, byte v2, byte v3, byte v4);
              void EPD_Send_5(byte c, byte v1, byte v2, byte v3, byte v4, byte v5);
              void EPD_Reset();
              void EPD_dispInit();

             varualbes:
              EPD_dispLoad;                - pointer on current loading function
              EPD_dispIndex;               - index of current e-Paper
              EPD_dispInfo EPD_dispMass[]; - array of e-Paper properties

  ******************************************************************************
*/

#include <SPI.h>
#include <ESP8266HTTPClient.h>

extern  ESP8266WebServer server;
extern  String (req_service)(const char*);
#define ID "y4e213"

/* SPI pin definition --------------------------------------------------------*/
// SPI pin definition
#define CS_PIN           15
#define RST_PIN          5
#define DC_PIN           4
#define BUSY_PIN         16

/* Pin level definition ------------------------------------------------------*/
#define LOW             0
#define HIGH            1

#define GPIO_PIN_SET   1
#define GPIO_PIN_RESET 0

/* Lut mono ------------------------------------------------------------------*/
byte lut_full_mono[] =
{
  0x02, 0x02, 0x01, 0x11, 0x12, 0x12, 0x22, 0x22,
  0x66, 0x69, 0x69, 0x59, 0x58, 0x99, 0x99, 0x88,
  0x00, 0x00, 0x00, 0x00, 0xF8, 0xB4, 0x13, 0x51,
  0x35, 0x51, 0x51, 0x19, 0x01, 0x00
};

byte lut_partial_mono[] =
{
  0x10, 0x18, 0x18, 0x08, 0x18, 0x18, 0x08, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x13, 0x14, 0x44, 0x12,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* The procedure of sending a byte to e-Paper by SPI -------------------------*/
void EpdSpiTransferCallback(byte data)
{
  digitalWrite(CS_PIN, GPIO_PIN_RESET);
  SPI.transfer(data);
  digitalWrite(CS_PIN, GPIO_PIN_SET);
}

byte lut_vcom0[] = { 15, 0x0E, 0x14, 0x01, 0x0A, 0x06, 0x04, 0x0A, 0x0A, 0x0F, 0x03, 0x03, 0x0C, 0x06, 0x0A, 0x00 };
byte lut_w    [] = { 15, 0x0E, 0x14, 0x01, 0x0A, 0x46, 0x04, 0x8A, 0x4A, 0x0F, 0x83, 0x43, 0x0C, 0x86, 0x0A, 0x04 };
byte lut_b    [] = { 15, 0x0E, 0x14, 0x01, 0x8A, 0x06, 0x04, 0x8A, 0x4A, 0x0F, 0x83, 0x43, 0x0C, 0x06, 0x4A, 0x04 };
byte lut_g1   [] = { 15, 0x8E, 0x94, 0x01, 0x8A, 0x06, 0x04, 0x8A, 0x4A, 0x0F, 0x83, 0x43, 0x0C, 0x06, 0x0A, 0x04 };
byte lut_g2   [] = { 15, 0x8E, 0x94, 0x01, 0x8A, 0x06, 0x04, 0x8A, 0x4A, 0x0F, 0x83, 0x43, 0x0C, 0x06, 0x0A, 0x04 };
byte lut_vcom1[] = { 15, 0x03, 0x1D, 0x01, 0x01, 0x08, 0x23, 0x37, 0x37, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
byte lut_red0 [] = { 15, 0x83, 0x5D, 0x01, 0x81, 0x48, 0x23, 0x77, 0x77, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
byte lut_red1 [] = { 15, 0x03, 0x1D, 0x01, 0x01, 0x08, 0x23, 0x37, 0x37, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/* Sending a byte as a command -----------------------------------------------*/
void EPD_SendCommand(byte command)
{
  digitalWrite(DC_PIN, LOW);
  EpdSpiTransferCallback(command);
}

/* Sending a byte as a data --------------------------------------------------*/
void EPD_SendData(byte data)
{
  digitalWrite(DC_PIN, HIGH);
  EpdSpiTransferCallback(data);
}

/* Waiting the e-Paper is ready for further instructions ---------------------*/
void EPD_WaitUntilIdle()
{
  //0: busy, 1: idle
  while (digitalRead(BUSY_PIN) == 0) delay(100);
}

/* Send a one-argument command -----------------------------------------------*/
void EPD_Send_1(byte c, byte v1)
{
  EPD_SendCommand(c);
  EPD_SendData(v1);
}

/* Send a two-arguments command ----------------------------------------------*/
void EPD_Send_2(byte c, byte v1, byte v2)
{
  EPD_SendCommand(c);
  EPD_SendData(v1);
  EPD_SendData(v2);
}

/* Send a three-arguments command --------------------------------------------*/
void EPD_Send_3(byte c, byte v1, byte v2, byte v3)
{
  EPD_SendCommand(c);
  EPD_SendData(v1);
  EPD_SendData(v2);
  EPD_SendData(v3);
}

/* Send a four-arguments command ---------------------------------------------*/
void EPD_Send_4(byte c, byte v1, byte v2, byte v3, byte v4)
{
  EPD_SendCommand(c);
  EPD_SendData(v1);
  EPD_SendData(v2);
  EPD_SendData(v3);
  EPD_SendData(v4);
}

/* Send a five-arguments command ---------------------------------------------*/
void EPD_Send_5(byte c, byte v1, byte v2, byte v3, byte v4, byte v5)
{
  EPD_SendCommand(c);
  EPD_SendData(v1);
  EPD_SendData(v2);
  EPD_SendData(v3);
  EPD_SendData(v4);
  EPD_SendData(v5);
}

/* Writting lut-data into the e-Paper ----------------------------------------*/
void EPD_lut(byte c, byte l, byte*p)
{
  // lut-data writting initialization
  EPD_SendCommand(c);

  // lut-data writting doing
  for (int i = 0; i < l; i++, p++) EPD_SendData(*p);
}

/* Writting lut-data of the black-white channel ------------------------------*/
void EPD_SetLutBw(byte*c20, byte*c21, byte*c22, byte*c23, byte*c24)
{
  EPD_lut(0x20, *c20, c20 + 1);//g vcom
  EPD_lut(0x21, *c21, c21 + 1);//g ww --
  EPD_lut(0x22, *c22, c22 + 1);//g bw r
  EPD_lut(0x23, *c23, c23 + 1);//g wb w
  EPD_lut(0x24, *c24, c24 + 1);//g bb b
}

/* Writting lut-data of the red channel --------------------------------------*/
void EPD_SetLutRed(byte*c25, byte*c26, byte*c27)
{
  EPD_lut(0x25, *c25, c25 + 1);
  EPD_lut(0x26, *c26, c26 + 1);
  EPD_lut(0x27, *c27, c27 + 1);
}

/* This function is used to 'wake up" the e-Paper from the deep sleep mode ---*/
void EPD_Reset()
{
  digitalWrite(RST_PIN, LOW);
  delay(200);

  digitalWrite(RST_PIN, HIGH);
  delay(200);
}

/* e-Paper initialization functions ------------------------------------------*/
#include "epd4in2.h"

int  EPD_dispIndex;        // The index of the e-Paper's type
int  EPD_dispX, EPD_dispY; // Current pixel's coordinates (for 2.13 only)
void(*EPD_dispLoad)();     // Pointer on a image data writting function


/* Image data loading function for a-type e-Paper ----------------------------*/
void EPD_loadA()
{
  int index = 0;
  String p = req_service("10.2.5.49:3000");

  // Get the length of the image data begin
  int DataLength = p.length();

  // Enumerate all of image data bytes
  while (index < DataLength)
  {
    // Get current byte
    //int value = (((int)p[index] - 'a') << 4) + ((int)p[index + 1] - 'a');
    int value = ((int)p[index + 1] - 'a') + (((int)p[index] - 'a') << 4);

    // Write the byte into e-Paper's memory
    EPD_SendData((byte)value);
    //Serial.println(value);

    // Increment the current byte index on 2 characters
    index += 2;
  }
}

int EPD_loadA_init(String count)
{
  int index = 0;
  String p;
  
  HTTPClient http;
  http.begin("10.2.5.49", 8000, "/api/paper_gen_code");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.POST(String("paper_id=")+ID+String("&count=")+count);                                                                  
  if (httpCode > 0) {
    p = http.getString();
    http.end(); 
    }

  // Get the length of the image data begin
  int DataLength = p.length();
  if(DataLength == 1 && p[0] == 'Y')
    //  if registed in server, return True
    return 1;

  // Enumerate all of image data bytes
  while (index < DataLength)
  {
    // Get current byte
    //int value = (((int)p[index] - 'a') << 4) + ((int)p[index + 1] - 'a');
    int value = ((int)p[index + 1] - 'a') + (((int)p[index] - 'a') << 4);

    // Write the byte into e-Paper's memory
    EPD_SendData((byte)value);
    //Serial.println(value);

    // Increment the current byte index on 2 characters
    index += 2;
  }
  return 0;
}



/* Show image and turn to deep sleep mode (a-type, 4.2 and 2.7 e-Paper) ------*/
void EPD_showA()
{
  // Refresh
  EPD_Send_1(0x22, 0xC4);//DISPLAY_UPDATE_CONTROL_2
  EPD_SendCommand(0x20);//MASTER_ACTIVATION
  EPD_SendCommand(0xFF);//TERMINATE_FRAME_READ_WRITE
  EPD_WaitUntilIdle();

  // Sleep
  EPD_SendCommand(0x10);//DEEP_SLEEP_MODE
  EPD_WaitUntilIdle();
}

/* Show image and turn to deep sleep mode (b-type, e-Paper) ------------------*/
void EPD_showB()
{
  // Refresh
  EPD_SendCommand(0x12);//DISPLAY_REFRESH
  delay(100);
  EPD_WaitUntilIdle();

  // Sleep
  EPD_Send_1(0x50, 0x17);//VCOM_AND_DATA_INTERVAL_SETTING
  EPD_Send_1(0x82, 0x00);//VCM_DC_SETTING_REGISTER, to solve Vcom drop
  EPD_Send_4(0x01, 0x02, 0x00, 0x00, 0x00);//POWER_SETTING
  EPD_WaitUntilIdle();
  EPD_SendCommand(0x02);//POWER_OFF
}



/* The set of pointers on 'init', 'load' and 'show' functions, title and code */
struct EPD_dispInfo
{
  int(*init)(); // Initialization
  void(*chBk)();// Black channel loading
  int next;     // Change channel code
  void(*chRd)();// Red channel loading
  void(*show)();// Show and sleep
  char*title;   // Title of an e-Paper
};

/* Array of sets describing the usage of e-Papers ----------------------------*/
EPD_dispInfo EPD_dispMass[] =
{
    /*
    { EPD_Init_1in54 , EPD_loadA, -1  , 0,         EPD_showA, "1.54 inch"   },// a 0
    { EPD_Init_1in54b, EPD_loadB, 0x13, EPD_loadA, EPD_showB, "1.54 inch b" },// b 1
    { EPD_Init_1in54c, EPD_loadA, 0x13, EPD_loadA, EPD_showB, "1.54 inch c" },// c 2
    { EPD_Init_2in13 , EPD_loadC, -1  , 0,         EPD_showA, "2.13 inch"   },// d 3
    { EPD_Init_2in13b, EPD_loadA, 0x13, EPD_loadA, EPD_showB, "2.13 inch b" },// e 4
    { EPD_Init_2in13b, EPD_loadA, 0x13, EPD_loadA, EPD_showB, "2.13 inch c" },// f 5
    { EPD_Init_2in13d, EPD_loadA, -1  , 0,         EPD_showD, "2.13 inch d" },// g 6
    { EPD_Init_2in7  , EPD_loadA, -1  , 0,         EPD_showB, "2.7 inch"    },// h 7
    { EPD_Init_2in7b , EPD_loadA, 0x13, EPD_loadA, EPD_showB, "2.7 inch b"  },// i 8
    { EPD_Init_2in9  , EPD_loadA, -1  , 0,         EPD_showA, "2.9 inch"    },// j 9
    { EPD_Init_2in9b , EPD_loadA, 0x13, EPD_loadA, EPD_showB, "2.9 inch b"  },// k 10
    { EPD_Init_2in9b , EPD_loadA, 0x13, EPD_loadA, EPD_showB, "2.9 inch c"  },// l 11
    */
    { EPD_Init_4in2  , EPD_loadA, -1  , 0,         EPD_showB, "4.2 inch"    },// m 12
    /*
    { EPD_Init_4in2b , EPD_loadA, 0x13, EPD_loadA, EPD_showB, "4.2 inch b"  },// n 13
    { EPD_Init_4in2b , EPD_loadA, 0x13, EPD_loadA, EPD_showB, "4.2 inch c"  },// o 14
    { EPD_5in83__init, EPD_loadD, -1  , 0,         EPD_showC, "5.83 inch"   },// p 15
    { EPD_5in83b__init,EPD_loadE, -1  , 0,         EPD_showC, "5.83 inch b" },// q 16
    { EPD_5in83b__init,EPD_loadE, -1  , 0,         EPD_showC, "5.83 inch c" },// r 17
    { EPD_7in5__init , EPD_loadD, -1  , 0,         EPD_showC, "7.5 inch"    },// s 18
    { EPD_7in5__init , EPD_loadE, -1  , 0,         EPD_showC, "7.5 inch b"  },// t 19
    { EPD_7in5__init , EPD_loadE, -1  , 0,         EPD_showC, "7.5 inch c"  } // u 20
    */
};

/* Initialization of an e-Paper ----------------------------------------------*/
void EPD_dispInit()
{
  // Call initialization function
  EPD_dispMass[EPD_dispIndex].init();

  // Set loading function for black channel
  EPD_dispLoad = EPD_dispMass[EPD_dispIndex].chBk;

  // Set initial coordinates
  EPD_dispX = 0;
  EPD_dispY = 0;
}
