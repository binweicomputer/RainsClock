#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <stdint.h>
#include <Adafruit_GFX.h>
#include <FastLED.h>
#include <FastLED_NeoMatrix.h>
#include <LittleFS.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266httpUpdate.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "SoftwareSerial.h"

enum MsgType
{
    MsgType_Wifi,
    MsgType_Host,
    MsgType_Temp,
    MsgType_Audio,
    MsgType_Gest,
    MsgType_LDR,
    MsgType_Other
};

String version = "0.43";
// 保存数据的标志
bool shouldSaveConfig = false;
bool USBConnection = false; // true = usb...
bool WIFIConnection = false;

// 更新固件
ESP8266WebServer server(80);
const char *serverIndex = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";

// 按键设置
int tasterPin[] = {D0, D4, D8};
int tasterCount = 3;
int pressedTaster = 0;
int menuePointer;
int minBrightness=10;
int maxBrightness=80;
bool tasterState[3];
int timeoutTaster[] = {0, 0, 0, 0};
int blockTimeTaster[] = {0, 0, 0, 0};
bool pushed[] = {false, false, false, false};
bool blockTaster[] = {false, false, false, false};
bool blockTaster2[] = {false, false, false, false};
bool allowTasterSendToServer = true;

// 实例化串口
#define BOARD_ID_B "ESP-01"
    SoftwareSerial mySoftwareSerial(D7, D5); // RX, TX
#define BAUD_RATE 115200
#define MY_BAUD_RATE 9600

// Matrix 参数
#define Display_WIDTH 64
#define Display_HEIGHT 16
#define Display_Single_WIDTH 16
#define Display_Single_HEIGHT 16
#define Display_Single_COUNT 4
CRGB leds[Display_WIDTH * Display_HEIGHT];
#define BRIGHTNESS 30 // 默认背光亮度
uint8_t offsetY = 4;  // 高度偏移量
FastLED_NeoMatrix *matrix;
int matrixType = 0;
int matrixTempCorrection = 0;
char awtrix_server[16] = "220.191.90.151";
char Port[6] = "7001"; // AWTRIX Host Port, default = 7001
// char city = "101210101";
bool ignoreServer = false;
int connectionTimout;
bool firstStart = true;

// USB 连接:
byte myBytes[1000];
int bufferpointer;

IPAddress Server;
WiFiClient espClient;
PubSubClient client(espClient);

WiFiManager wifiManager;

void initSerial();
void initScreen();
void initFileSystem();
void initWifi();

uint32_t Wheel(byte WheelPos, int pos);

void saveConfigCallback();
void configModeCallback(WiFiManager *myWiFiManager);
bool saveConfig();
void hardwareAnimatedCheck(MsgType typ, int x, int y);
void hardwareAnimatedSearch(int typ, int x, int y);
void hardwareAnimatedUncheck(int typ, int x, int y);
void flashProgress(unsigned int progress, unsigned int total);

void callback(char *topic, byte *payload, unsigned int length);
void updateMatrix(byte payload[], int length);
void showscrolling(String msg, int x , int y);
void showscrolling(String msg, int startX, int startY, int maxWidth, int maxHeight);

int checkTaster(int nr);
void sendToServer(String s);
#endif