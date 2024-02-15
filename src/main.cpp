#include <Arduino.h>
#include "config.h"
#include "Fonts/TomThumb.h"
#include "MenueControl/MenueControl.h"

MenueControl myMenue;

/**
 * @brief 根据轮子位置和颜色位置计算颜色
 *
 * @param WheelPos 轮子位置
 * @param pos 颜色位置
 * @return uint32_t 颜色值
 */
uint32_t Wheel(byte WheelPos, int pos)
{
  // 如果轮子位置小于85
  if (WheelPos < 85)
  {
    // 返回颜色值
    return matrix->Color((WheelPos * 3) - pos, (255 - WheelPos * 3) - pos, 0);
  }
  // 如果轮子位置小于170
  else if (WheelPos < 170)
  {
    // 减去85得到新的轮子位置
    WheelPos -= 85;
    // 返回颜色值
    return matrix->Color((255 - WheelPos * 3) - pos, 0, (WheelPos * 3) - pos);
  }
  // 如果轮子位置大于等于170
  else
  {
    // 减去170得到新的轮子位置
    WheelPos -= 170;
    // 返回颜色值
    return matrix->Color(0, (WheelPos * 3) - pos, (255 - WheelPos * 3) - pos);
  }
}

void setup() {
  delay(2000);
  // 按键初使化
  for (int i = 0; i < tasterCount; i++)
  {
    pinMode(tasterPin[i], INPUT_PULLUP);
  }

  initSerial();
  initFileSystem();
  initScreen();
  initWifi();
  
  for (int x = 32; x >= -100; x--)
  {
    matrix->clear();
    matrix->setCursor(x, 6 + offsetY);
    matrix->print("Host-IP: " + String(awtrix_server) + ":" + String(Port));
    matrix->setTextColor(matrix->Color(0, 255, 50));
    matrix->show();
    delay(40);
  }

  client.setServer(awtrix_server, atoi(Port));
  client.setCallback(callback);

  ignoreServer = false;

  connectionTimout = millis();
}

void loop() {
  server.handleClient();

  checkTaster(0);
  checkTaster(1);
  checkTaster(2);
  // 菜单所必需的...
  if (ignoreServer)
  {
    if (pressedTaster > 0)
    {
      matrix->clear();
      matrix->setCursor(0, 6 + offsetY);
      matrix->setTextColor(matrix->Color(0, 255, 50));
      matrix->print(myMenue.getMenueString(&menuePointer, &pressedTaster, &minBrightness, &maxBrightness));
      matrix->show();
    }

    // 获取数据并忽略
    if (Serial.available() > 0)
    {
      Serial.read();
    }
  }
}

/**
 * @brief 初始化串口
 *
 * @details 初始化串口，设置接收缓冲区大小，开始串口通信，并打印串口准备就绪信息。
 *
 * @param None
 * @return None
 */
void initSerial()
{
  delay(2000);                                     // 延迟2秒
  Serial.setRxBufferSize(1024);                    // 设置接收缓冲区大小为1024
  Serial.begin(BAUD_RATE);                         // 开始串口通信
  Serial.println("串口准备就绪...");               // 打印串口准备就绪信息
  mySoftwareSerial.begin(MY_BAUD_RATE);            // 初始化软串口
  mySoftwareSerial.println("软串口已经准备好..."); // 打印软串口准备就绪信息
}

/**
 * @brief 初始化屏幕，根据矩阵类型和温度校正参数进行配置
 */
void initScreen()
{
  switch (matrixType)
  {
  case 0:
    matrix = new FastLED_NeoMatrix(leds, Display_WIDTH, Display_HEIGHT, NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG);
    break;
  case 1:
    matrix = new FastLED_NeoMatrix(leds, Display_Single_WIDTH, Display_Single_HEIGHT, Display_Single_COUNT, 1, NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE);
    break;
  case 2:
    matrix = new FastLED_NeoMatrix(leds, Display_WIDTH, Display_HEIGHT, NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG);
    break;
  default:
    matrix = new FastLED_NeoMatrix(leds, Display_WIDTH, Display_HEIGHT, NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG);
    break;
  }

  switch (matrixTempCorrection)
  {
  case 0:
    FastLED.addLeds<NEOPIXEL, D2>(leds, Display_WIDTH * Display_HEIGHT).setCorrection(TypicalLEDStrip);
    break;
  case 1:
    FastLED.addLeds<NEOPIXEL, D2>(leds, Display_WIDTH * Display_HEIGHT).setTemperature(Candle);
    break;
  case 2:
    FastLED.addLeds<NEOPIXEL, D2>(leds, Display_WIDTH * Display_HEIGHT).setTemperature(Tungsten40W);
    break;
  case 3:
    FastLED.addLeds<NEOPIXEL, D2>(leds, Display_WIDTH * Display_HEIGHT).setTemperature(Tungsten100W);
    break;
  case 4:
    FastLED.addLeds<NEOPIXEL, D2>(leds, Display_WIDTH * Display_HEIGHT).setTemperature(Halogen);
    break;
  case 5:
    FastLED.addLeds<NEOPIXEL, D2>(leds, Display_WIDTH * Display_HEIGHT).setTemperature(CarbonArc);
    break;
  case 6:
    FastLED.addLeds<NEOPIXEL, D2>(leds, Display_WIDTH * Display_HEIGHT).setTemperature(HighNoonSun);
    break;
  case 7:
    FastLED.addLeds<NEOPIXEL, D2>(leds, Display_WIDTH * Display_HEIGHT).setTemperature(DirectSunlight);
    break;
  case 8:
    FastLED.addLeds<NEOPIXEL, D2>(leds, Display_WIDTH * Display_HEIGHT).setTemperature(OvercastSky);
    break;
  case 9:
    FastLED.addLeds<NEOPIXEL, D2>(leds, Display_WIDTH * Display_HEIGHT).setTemperature(ClearBlueSky);
    break;
  case 10:
    FastLED.addLeds<NEOPIXEL, D2>(leds, Display_WIDTH * Display_HEIGHT).setTemperature(WarmFluorescent);
    break;
  case 11:
    FastLED.addLeds<NEOPIXEL, D2>(leds, Display_WIDTH * Display_HEIGHT).setTemperature(StandardFluorescent);
    break;
  case 12:
    FastLED.addLeds<NEOPIXEL, D2>(leds, Display_WIDTH * Display_HEIGHT).setTemperature(CoolWhiteFluorescent);
    break;
  case 13:
    FastLED.addLeds<NEOPIXEL, D2>(leds, Display_WIDTH * Display_HEIGHT).setTemperature(FullSpectrumFluorescent);
    break;
  case 14:
    FastLED.addLeds<NEOPIXEL, D2>(leds, Display_WIDTH * Display_HEIGHT).setTemperature(GrowLightFluorescent);
    break;
  case 15:
    FastLED.addLeds<NEOPIXEL, D2>(leds, Display_WIDTH * Display_HEIGHT).setTemperature(BlackLightFluorescent);
    break;
  case 16:
    FastLED.addLeds<NEOPIXEL, D2>(leds, Display_WIDTH * Display_HEIGHT).setTemperature(MercuryVapor);
    break;
  case 17:
    FastLED.addLeds<NEOPIXEL, D2>(leds, Display_WIDTH * Display_HEIGHT).setTemperature(SodiumVapor);
    break;
  case 18:
    FastLED.addLeds<NEOPIXEL, D2>(leds, Display_WIDTH * Display_HEIGHT).setTemperature(MetalHalide);
    break;
  case 19:
    FastLED.addLeds<NEOPIXEL, D2>(leds, Display_WIDTH * Display_HEIGHT).setTemperature(HighPressureSodium);
    break;
  case 20:
    FastLED.addLeds<NEOPIXEL, D2>(leds, Display_WIDTH * Display_HEIGHT).setTemperature(UncorrectedTemperature);
    break;
  default:
    FastLED.addLeds<NEOPIXEL, D2>(leds, Display_WIDTH * Display_HEIGHT).setCorrection(TypicalLEDStrip);
    break;
  }
  // 开始矩阵操作，设置不自动换行、亮度以及字体
  matrix->begin();
  matrix->setTextWrap(false);
  matrix->setBrightness(BRIGHTNESS);
  matrix->setFont(&TomThumb);
}

/**
 * @brief 读取配置文件
 */
void initFileSystem()
{
  if (LittleFS.begin())
  {
    // 如果awtrix.json文件不存在，则创建一个新文件并写入一些默认内容（此处省略）
    if (!(LittleFS.exists("/awtrix.json")))
    {
      LittleFS.open("/awtrix.json", "w+");
    }

    File configFile = LittleFS.open("/awtrix.json", "r");
    Serial.println("读取始化文件...");
    if (configFile)
    {
      size_t size = configFile.size();
      if (size > 0)
      {
        // 分配一个足够大的缓冲区来存储文件内容
        std::unique_ptr<char[]> buf(new char[size + 1]); // 注意：这里通常需要多分配一个字节以添加终止符'\0'
        configFile.readBytes(buf.get(), size);
        buf.get()[size] = '\0'; // 添加终止符，确保字符串正确结束

        DynamicJsonBuffer jsonBuffer;
        // 尝试解析JSON
        JsonObject &json = jsonBuffer.parseObject(buf.get());
        if (!json.success())
        {
          Serial.println("解析JSON失败!");
          Serial.println(buf.get()); // 输出读取到的文件内容以便于调试
        }
        else
        {
          strcpy(awtrix_server, json["awtrix_server"]);
          if (json.containsKey("matrixType"))
          {
            matrixType = json["matrixType"].as<int>();
          }
          matrixTempCorrection = json["matrixCorrection"].as<int>();

          if (json.containsKey("Port"))
          {
            strcpy(Port, json["Port"]);
          }
          Serial.println(buf.get());
        }
      }
      else
      {
        Serial.println("配置文件为空!");
      }
    }
    else
    {
      Serial.println("未能打开配置文件!");
    }
    configFile.close();
  }
  else
  {
    Serial.println("LittleFS初始化失败...");
  }
  Serial.println("matrixType");
  Serial.println(matrixType);
}

/**
 * @brief 初始化网络连接
 *
 * 该函数负责设置静态IP配置，添加WiFi参数，并设置保存配置回调和接入点回调。
 * 然后尝试连接WiFi网络并设置服务器以处理HTTP请求。
 * 最后保存配置信息并重启ESP32。
 */
void initWifi()
{
  // 设置静态IP配置
  wifiManager.setAPStaticIPConfig(IPAddress(172, 217, 28, 1), IPAddress(172, 217, 28, 1), IPAddress(255, 255, 255, 0));
  WiFiManagerParameter custom_awtrix_server("server", "AWTRIX Host", awtrix_server, 16);
  WiFiManagerParameter custom_port("Port", "Matrix Port", Port, 6);
  WiFiManagerParameter custom_matrix_type("matrixType", "MatrixType", "0", 1);
  // Just a quick hint
  WiFiManagerParameter host_hint("<small>AWTRIX Host IP (without Port)<br></small><br><br>");
  WiFiManagerParameter port_hint("<small>Communication Port (default: 7001)<br></small><br><br>");
  WiFiManagerParameter matrix_hint("<small>0: Columns; 1: Tiles; 2: Rows <br></small><br><br>");
  WiFiManagerParameter p_lineBreak_notext("<p></p>");

  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.setAPCallback(configModeCallback);

  wifiManager.addParameter(&p_lineBreak_notext);
  wifiManager.addParameter(&host_hint);
  wifiManager.addParameter(&custom_awtrix_server);
  wifiManager.addParameter(&port_hint);
  wifiManager.addParameter(&custom_port);
  wifiManager.addParameter(&matrix_hint);
  wifiManager.addParameter(&custom_matrix_type);
  wifiManager.addParameter(&p_lineBreak_notext);

  wifiManager.setCustomHeadElement("<style>html{ background-color: #607D8B;}</style>");

  for (int i = 0; i < 5; i++)
  {
    Serial.print(".");
    hardwareAnimatedSearch(0, 24, 0);
    delay(10);
  }
  
  if (!wifiManager.autoConnect("BinweiClock", ""))
  {
    // 重置并重试，或者让它进入深度睡眠
    ESP.reset();
    delay(5000);
  }

  // 开启热点!
  WiFi.mode(WIFI_STA);

  server.on("/", HTTP_GET, []()
            {
		server.sendHeader("Connection", "close");
		server.send(200, "text/html", serverIndex); });

  server.on("/reset", HTTP_GET, []()
            {
		server.send(200, "text/html", serverIndex);
		wifiManager.resetSettings();
		ESP.reset(); });

  server.on(
      "/update", HTTP_POST, []()
      {
      server.sendHeader("Connection", "close");
      server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
      ESP.restart(); },
      []()
      {
      HTTPUpload& upload = server.upload();

      if (upload.status == UPLOAD_FILE_START) {
        Serial.setDebugOutput(true);

        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
        if (!Update.begin(maxSketchSpace)) { //start with max available size
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_WRITE) {
		  matrix->clear();
		  flashProgress((int)upload.currentSize,(int)upload.buf);
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) { //true to set the size to the current progress
		  server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");


        } else {
          Update.printError(Serial);
        }
        Serial.setDebugOutput(false);
      }
      yield(); });

  server.begin();

  if (shouldSaveConfig)
  {

    strcpy(awtrix_server, custom_awtrix_server.getValue());
    matrixType = atoi(custom_matrix_type.getValue());
    strcpy(Port, custom_port.getValue());
    saveConfig();
    ESP.reset();
  }

  hardwareAnimatedCheck(MsgType_Wifi, 27, 2);
  IPAddress ip = WiFi.localIP();
  String ipAddressStr = String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) + "." + String(ip[3]);
  showscrolling(ipAddressStr, 64, 11);

  delay(100); 
}

/**
 * @brief 配置保存回调函数
 *
 * 当需要保存配置时，调用此函数。
 * 如果USB连接未建立，则打印一条消息表示应保存配置。
 * 并将标志`shouldSaveConfig`设置为true。
 */
void saveConfigCallback()
{
  if (!USBConnection)
  {
    Serial.println("Should save config");
  }
  shouldSaveConfig = true;
}

/**
 * @brief WiFi配置模式回调函数
 *
 * 当WiFi需要进行配置时，该函数会被调用。它会检查USB连接是否建立，
 * 并在串口打印软AP的IP地址以及配置门户的SSID，
 * 同时在矩阵显示屏上显示热点消息。
 *
 * @param myWiFiManager 指向WiFiManager对象的指针
 */
void configModeCallback(WiFiManager *myWiFiManager)
{
  if (!USBConnection)
  {
    Serial.println("已进入配置模式");
    Serial.println(WiFi.softAPIP());
    Serial.println(myWiFiManager->getConfigPortalSSID());
  }
  matrix->clear();                                 // 清除矩阵显示屏内容
  matrix->setCursor(3, 6 + offsetY);               // 设置光标位置
  matrix->setTextColor(matrix->Color(0, 255, 50)); // 设置字体颜色为绿色
  matrix->print("Hotspot");                        // 显示“Hotspot”文本
  matrix->show();                                  // 更新并显示矩阵显示屏内容
}


/**
 * @brief 保存配置信息到文件
 *
 * @return bool 保存成功返回true，否则返回false
 */
bool saveConfig()
{
  DynamicJsonBuffer jsonBuffer;
  JsonObject &json = jsonBuffer.createObject();
  json["awtrix_server"] = awtrix_server;
  json["matrixType"] = matrixType;
  json["matrixCorrection"] = matrixTempCorrection;
  json["Port"] = Port;

  // json["temp"] = tempState;
  // json["usbWifi"] = USBConnection;
  // json["ldr"] = ldrState;
  // json["gesture"] = gestureState;
  // json["audio"] = audioState;

  File configFile = LittleFS.open("/awtrix.json", "w");
  if (!configFile)
  {
    if (!USBConnection)
    {
      Serial.println("failed to open config file for writing");
    }

    return false;
  }

  json.printTo(configFile);
  configFile.close();
  // end save
  return true;
}

/**
 * @brief 硬件动画检查函数.
 *
 * 此函数用于检查硬件状态并在矩阵显示屏上显示动画消息.
 *
 * @param typ 要显示的消息类型.
 * @param x 待绘制像素的x坐标.
 * @param y 待绘制像素的y坐标.
 */
void hardwareAnimatedCheck(MsgType typ, int x, int y)
{
  int wifiCheckTime = millis();
  int wifiCheckPoints = 0;

  // 每隔2秒检查一次WiFi状态
  while (millis() - wifiCheckTime < 2000)
  {
    // 清除矩阵显示屏内容
    matrix->clear();

    // 根据消息类型显示相应消息
    switch (typ)
    {
    case MsgType_Wifi:
      matrix->setCursor(7, 6 + offsetY);
      matrix->print("WiFi");
      break;
    case MsgType_Host:
      matrix->setCursor(5, 6 + offsetY);
      matrix->print("Host");
      break;
    case MsgType_Temp:
      matrix->setCursor(7, 6 + offsetY);
      matrix->print("Temp");
      break;
    case MsgType_Audio:
      matrix->setCursor(3, 6 + offsetY);
      matrix->print("Audio");
      break;
    case MsgType_Gest:
      matrix->setCursor(3, 6 + offsetY);
      matrix->print("Gest.");
      break;
    case MsgType_LDR:
      matrix->setCursor(7, 6 + offsetY);
      matrix->print("LDR");
      break;
    }

    // 绘制动画消息  √
    switch (wifiCheckPoints)
    {
    case 6:
      matrix->drawPixel(63, 9, 0xff00);
      matrix->drawPixel(x, y + offsetY, 0x07E0);
    case 5:
      matrix->drawPixel(63, 10, 0xff00);
      matrix->drawPixel(x - 1, y + offsetY + 1, 0x07E0);
    case 4:
      matrix->drawPixel(63, 11, 0xff00);
      matrix->drawPixel(x - 2, y + offsetY + 2, 0x07E0);
    case 3:
      matrix->drawPixel(63, 12, 0xff00);
      matrix->drawPixel(x - 3, y + offsetY + 3, 0x07E0);
    case 2:
      matrix->drawPixel(63, 13, 0xff00);
      matrix->drawPixel(x - 4, y + offsetY + 4, 0x07E0);
    case 1:
      matrix->drawPixel(63, 14, 0xff00);
      matrix->drawPixel(x - 5, y + offsetY + 3, 0x07E0);
    case 0:
      matrix->drawPixel(63, 15, 0xff00);
      matrix->drawPixel(x - 6, y + offsetY + 2, 0x07E0);
      break;
    }

    // 显示矩阵显示屏并延迟100毫秒
    wifiCheckPoints++;
    matrix->show();
    delay(200);
  }
}

/**
 * @brief 硬件动画搜索函数 ->>动画
 *
 * 此函数在硬件显示上执行动画搜索。
 *
 * @param typ 搜索类型（0表示WiFi，1表示Host）
 * @param x 搜索区域的x坐标
 * @param y 搜索区域的y坐标
 */
void hardwareAnimatedSearch(int typ, int x, int y)
{
  for (int i = 0; i < 4; i++)
  {
    matrix->clear();              // 清除显示屏内容
    matrix->setTextColor(0xFFFF); // 设置文本颜色为白色
    if (typ == 0)
    {
      matrix->setCursor(7, 6 + offsetY); // 设置光标位置以显示"WiFi"文本
      matrix->print("WiFi");             // 显示"WiFi"文本
    }
    else if (typ == 1)
    {
      matrix->setCursor(5, 6 + offsetY); // 设置光标位置以显示"Host"文本
      matrix->print("Host");             // 显示"Host"文本
    }
    //->>动画
    switch (i)
    {
    case 3:
      matrix->drawPixel(60, 0, 0x00ff);
      matrix->drawPixel(x, y + offsetY, 0x22ff);         // 在(x, y)位置绘制像素点
      matrix->drawPixel(x + 1, y + offsetY + 1, 0x22ff); // 在(x+1, y+1)位置绘制像素点
      matrix->drawPixel(x + 2, y + offsetY + 2, 0x22ff); // 在(x+2, y+2)位置绘制像素点
      matrix->drawPixel(x + 3, y + offsetY + 3, 0x22ff); // 在(x+3, y+3)位置绘制像素点
      matrix->drawPixel(x + 2, y + offsetY + 4, 0x22ff); // 在(x+2, y+4)位置绘制像素点
      matrix->drawPixel(x + 1, y + offsetY + 5, 0x22ff); // 在(x+1, y+5)位置绘制像素点
      matrix->drawPixel(x, y + offsetY + 6, 0x22ff);     // 在(x, y+6)位置绘制像素点
      break;
    case 2:
      matrix->drawPixel(61, 0, 0x00ff);
      matrix->drawPixel(x - 1, y + offsetY + 2, 0x22ff); // 在(x-1, y+2)位置绘制像素点
      matrix->drawPixel(x, y + offsetY + 3, 0x22ff);     // 在(x, y+3)位置绘制像素点
      matrix->drawPixel(x - 1, y + offsetY + 4, 0x22ff); // 在(x-1, y+4)位置绘制像素点
      break;
    case 1:
      matrix->drawPixel(62, 0, 0x00ff);
      matrix->drawPixel(x - 3, y + offsetY + 3, 0x22ff); // 在(x-3, y+3)位置绘制像素点
      break;
    case 0:
      matrix->drawPixel(63, 0, 0x00ff);
      break;
    }
    matrix->show(); // 显示当前矩阵内容
    delay(100);     // 延迟100毫秒
  }
}

/**
 * @brief 动画模拟取消选择硬件组件 打X 动画。
 *
 * @param typ 待取消选择的硬件组件类型。
 * @param x 硬件组件的x坐标。
 * @param y 硬件组件的y坐标。
 */
void hardwareAnimatedUncheck(int typ, int x, int y)
{
  int wifiCheckTime = millis(); // 获取当前系统运行时间（毫秒）
  int wifiCheckPoints = 0;      // 初始化动画步骤计数器

  // 在2秒内循环执行动画
  while (millis() - wifiCheckTime < 2000)
  {
    // 按照10步进行动画绘制
    while (wifiCheckPoints < 10)
    {
      matrix->clear(); // 清除显示矩阵内容

      // 根据硬件组件类型设置并显示文本
      switch (typ)
      {
      case 0:
        matrix->setCursor(7, 6 + offsetY);
        matrix->print("WiFi");
        break;
      case 1:
        matrix->setCursor(1, 6 + offsetY);
        matrix->print("Server");
        break;
      case 2:
        matrix->setCursor(7, 6 + offsetY);
        matrix->print("Temp");
        break;
      case 4:
        matrix->setCursor(3, 6 + offsetY);
        matrix->print("Gest.");
        break;
      }

      // 绘制逐个消隐的像素点，打 X 动画效果
      switch (wifiCheckPoints)
      {
      case 9:
        matrix->drawPixel(63, 6, 0xff00);
        matrix->drawPixel(x, y + offsetY + 4, 0xF800);
      case 8:
        matrix->drawPixel(63, 7, 0xff00);
        matrix->drawPixel(x - 1, y + offsetY + 3, 0xF800);
      case 7:
        matrix->drawPixel(63, 8, 0xff00);
        matrix->drawPixel(x - 2, y + offsetY + 2, 0xF800);
      case 6:
        matrix->drawPixel(63, 9, 0xff00);
        matrix->drawPixel(x - 3, y + offsetY + 1, 0xF800);
      case 5:
        matrix->drawPixel(63, 10, 0xff00);
        matrix->drawPixel(x - 4, y + offsetY, 0xF800);
      case 4:
        matrix->drawPixel(63, 11, 0xff00);
        matrix->drawPixel(x - 4, y + offsetY + 4, 0xF800);
      case 3:
        matrix->drawPixel(63, 12, 0xff00);
        matrix->drawPixel(x - 3, y + offsetY + 3, 0xF800);
      case 2:
        matrix->drawPixel(63, 13, 0xff00);
        matrix->drawPixel(x - 2, y + offsetY + 2, 0xF800);
      case 1:
        matrix->drawPixel(63, 14, 0xff00);
        matrix->drawPixel(x - 1, y + offsetY + 1, 0xF800);
      case 0:
        matrix->drawPixel(63, 15, 0xff00);
        matrix->drawPixel(x, y + offsetY, 0xF800);
        break;
      }

      wifiCheckPoints++; // 增加动画步骤计数
      matrix->show();    // 更新显示矩阵内容
      delay(100);        // 延迟100毫秒以实现动画效果
    }
  }
}

/**
 * @brief 在矩阵显示屏上显示进度条。
 *
 * @param progress 当前进度值。
 * @param total 总进度值。
 */
void flashProgress(unsigned int progress, unsigned int total)
{
  matrix->setBrightness(BRIGHTNESS);                            // 设置矩阵显示屏的亮度
  long num = Display_WIDTH * Display_HEIGHT * progress / total; // 计算需要显示的像素点数量
  for (unsigned char y = 0; y < Display_HEIGHT; y++)            // 遍历矩阵显示屏每一行
  {
    for (unsigned char x = 0; x < Display_WIDTH; x++) // 遍历矩阵显示屏每一列
    {
      if (num-- > 0)                                                              // 检查是否还有像素点需要显示
        matrix->drawPixel(x, Display_HEIGHT - y - 1, Wheel((num * 16) & 255, 0)); // 在矩阵显示屏上绘制像素点
    }
  }
  matrix->setCursor(1, 6 + offsetY);                  // 设置矩阵显示屏光标位置
  matrix->setTextColor(matrix->Color(200, 200, 200)); // 设置矩阵显示屏文本颜色
  matrix->print("FLASHING");                          // 在矩阵显示屏上打印文本"FLASHING"
  matrix->show();                                     // 显示矩阵显示屏内容
}

/**
 * @brief MQTT消息接收回调函数
 *
 * 当接收到指定MQTT主题的消息时，调用此函数进行处理。
 * 它将WIFIConnection标志设为true，并调用updateMatrix函数来处理接收到的负载数据。
 *
 * @param topic 接收到消息的MQTT主题
 * @param payload 接收到的负载数据
 * @param length 接收负载的长度
 */
void callback(char *topic, byte *payload, unsigned int length)
{
  // 将WIFIConnection标志设置为true
  WIFIConnection = true;

  // 调用updateMatrix函数来处理接收到的负载内容
  updateMatrix(payload, length);
}

/**
 * @brief 更新矩阵
 *
 * @param payload 矩阵数据
 * @param length 数据长度
 */
void updateMatrix(byte payload[], int length)
{
  if (!ignoreServer)
  {
    int y_offset = 5;
    // 如果是首次启动，执行初始化操作并标记为非首次
    if (firstStart)
    {
      hardwareAnimatedCheck(MsgType_Audio, 30, 2);
      firstStart = false;
    }

    connectionTimout = millis();
  }
}


//滚动显示
void showscrolling(String msg, int x, int y)
{
  int length = msg.length();
  while (x >= -length*4)
  {
    matrix->clear();
    matrix->setCursor(x, y);
    matrix->print(msg);
    matrix->setTextColor(matrix->Color(0, 255, 50));
    matrix->show();
    delay(100);
    x--;
  } 
}

void showscrolling(String msg, int startX, int startY, int maxWidth, int maxHeight)
{
  Serial.println(msg);
  int length = msg.length();
  int x = startX;
  while (x >= -(length * 4) )
  {
    matrix->clear();

    // 计算并限制当前文本打印的起始列位置，确保在指定范围内
    int textX = max(0, min(maxWidth - (length * 4), x));

    matrix->setCursor(textX, startY);

    // 只打印在指定高度内的文本行
    for (int i = 0; i < msg.length(); ++i)
    {
      matrix->print(msg[i]);
      Serial.println(x);
      Serial.print(i);
      if (i != msg.length() - 1) // 非最后一个字符时添加空格以模拟滚动效果
        matrix->print(' ');
    }

    matrix->setTextColor(matrix->Color(0, 255, 50));
    matrix->show();
    delay(100);
    x--;
  }
}

/**
 * @brief 检查taster的状态，并根据状态执行相应的操作
 *
 * @param nr taster的编号
 * @return int 返回值为0表示正常，返回值为1表示发送按钮消息给服务器，返回值为2表示长按按钮
 */
int checkTaster(int nr)
{
  tasterState[0] = !digitalRead(tasterPin[0]);
  tasterState[1] = digitalRead(tasterPin[1]);
  tasterState[2] = !digitalRead(tasterPin[2]);

  switch (nr)
  {
  case 0:
    if (tasterState[0] == LOW && !pushed[nr] && !blockTaster2[nr] && tasterState[1] && tasterState[2])
    {
      pushed[nr] = true;
      timeoutTaster[nr] = millis();
    }
    break;
  case 1:
    if (tasterState[1] == LOW && !pushed[nr] && !blockTaster2[nr] && tasterState[0] && tasterState[2])
    {
      pushed[nr] = true;
      timeoutTaster[nr] = millis();
    }
    break;
  case 2:
    if (tasterState[2] == LOW && !pushed[nr] && !blockTaster2[nr] && tasterState[0] && tasterState[1])
    {
      pushed[nr] = true;
      timeoutTaster[nr] = millis();
    }
    break;
  case 3:
    if (tasterState[0] == LOW && tasterState[2] == LOW && !pushed[nr] && !blockTaster2[nr] && tasterState[1])
    {
      pushed[nr] = true;
      timeoutTaster[nr] = millis();
    }
    break;
  }

  if (pushed[nr] && (millis() - timeoutTaster[nr] < 2000) && tasterState[nr] == HIGH)
  {
    if (!blockTaster2[nr])
    {
      StaticJsonBuffer<400> jsonBuffer;
      JsonObject &root = jsonBuffer.createObject();
      root["type"] = "button";

      switch (nr)
      {
      case 0:
        root["left"] = "short";
        pressedTaster = 1;
        Serial.println("LEFT: normaler Tastendruck");
        break;
      case 1:
        root["middle"] = "short";
        pressedTaster = 2;
        Serial.println("MID: normaler Tastendruck");
        break;
      case 2:
        root["right"] = "short";
        pressedTaster = 3;
        Serial.println("RIGHT: normaler Tastendruck");
        break;
      }

      String JS;
      root.printTo(JS);
      if (allowTasterSendToServer)
      {
        sendToServer(JS);
      }
      pushed[nr] = false;
      return 1;
    }
  }

  if (pushed[nr] && (millis() - timeoutTaster[nr] > 2000))
  {
    if (!blockTaster2[nr])
    {
      StaticJsonBuffer<400> jsonBuffer;
      JsonObject &root = jsonBuffer.createObject();
      root["type"] = "button";
      switch (nr)
      {
      case 0:
        root["left"] = "long";
        Serial.println("LEFT: langer Tastendruck");
        break;
      case 1:
        root["middle"] = "long";
        Serial.println("MID: langer Tastendruck");
        break;
      case 2:
        root["right"] = "long";
        Serial.println("RIGHT: langer Tastendruck");
        break;
      case 3:
        if (allowTasterSendToServer)
        {
          allowTasterSendToServer = false;
          ignoreServer = true;
        }
        else
        {
          allowTasterSendToServer = true;
          ignoreServer = false;
          menuePointer = 0;
        }
        break;
      }
      String JS;
      root.printTo(JS);
      if (allowTasterSendToServer)
      {
        sendToServer(JS);
      }

      blockTaster[nr] = true;
      blockTaster2[nr] = true;
      pushed[nr] = false;
      return 2;
    }
  }
  if (nr == 3)
  {
    if (blockTaster[nr] && tasterState[0] == HIGH && tasterState[2] == HIGH)
    {
      blockTaster[nr] = false;
      blockTimeTaster[nr] = millis();
    }
  }
  else
  {
    if (blockTaster[nr] && tasterState[nr] == HIGH)
    {
      blockTaster[nr] = false;
      blockTimeTaster[nr] = millis();
    }
  }

  if (!blockTaster[nr] && (millis() - blockTimeTaster[nr] > 500))
  {
    blockTaster2[nr] = false;
  }
  return 0;
}

/**
 * @brief 向服务器发送字符串
 *
 * @param s 需要发送的字符串
 */
void sendToServer(String s)
{
  if (USBConnection) // 如果USB连接已建立
  {
    // 计算字符串长度并转换为uint32_t类型
    uint32_t laenge = s.length();

    // 将字符串长度以四位ASCII字符的形式打印，高位在前
    Serial.printf("%c%c%c%c%s", (laenge & 0xFF000000) >> 24, (laenge & 0x00FF0000) >> 16, (laenge & 0x0000FF00) >> 8, (laenge & 0x000000FF), s.c_str());
  }
  else // 若未使用USB连接
  {
    // 使用MQTT客户端将字符串发布到名为"matrixClient"的主题上
    client.publish("matrixClient", s.c_str());
  }
}
