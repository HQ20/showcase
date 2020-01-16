/*********
  Thanks to:
  Rui Santos - Complete project details at https://RandomNerdTutorials.com/esp32-cam-take-photo-save-microsd-card
  stackoverflow - ESP32 send image file with HTTPClient at https://stackoverflow.com/q/53264373/3348623
  stackexchange - How to call a contract method using the eth_call JSON-RPC API athttps://ethereum.stackexchange.com/a/3520
*********/

#include "esp_camera.h"
#include "Arduino.h"
#include "FS.h"                // SD Card ESP32
#include "SD_MMC.h"            // SD Card ESP32
#include "soc/soc.h"           // Disable brownour problems
#include "soc/rtc_cntl_reg.h"  // Disable brownour problems
#include "driver/rtc_io.h"
#include <EEPROM.h>            // read and write from flash memory
#include <WiFi.h>
#include <ArduinoJson.h>
#include <HttpClient.h>


#define WIFI_SSID     "*"
#define WIFI_PASS     "*"
#define SERVER        "*"
#define BOUNDARY      "--------------------------133747188241686651551404"
#define TIMEOUT       20000

// define the number of bytes you want to access
#define EEPROM_SIZE 1

// Pin definition for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

int pictureNumber = 0;

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector

  Serial.begin(115200);
  //Serial.setDebugOutput(true);
  //Serial.println();

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // Init Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  unsigned char led_cnt = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //Serial.println("Starting SD Card");
  if (!SD_MMC.begin()) {
    Serial.println("SD Card Mount Failed");
    return;
  }

  uint8_t cardType = SD_MMC.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD Card attached");
    return;
  }

  camera_fb_t * fb = NULL;

  // Take Picture with Camera
  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  // initialize EEPROM with predefined size
  EEPROM.begin(EEPROM_SIZE);
  pictureNumber = EEPROM.read(0) + 1;

  // Path where new picture will be saved in SD Card
  String path = "/picture" + String(pictureNumber) + ".jpg";

  String res = sendImage(path, fb->buf, fb->len);
  Serial.println(res);

  fs::FS &fs = SD_MMC;
  Serial.printf("Picture file name: %s\n", path.c_str());

  File file = fs.open(path.c_str(), FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file in writing mode");
  }
  else {
    file.write(fb->buf, fb->len); // payload (image), payload length
    Serial.printf("Saved file to path: %s\n", path.c_str());
    EEPROM.write(0, pictureNumber);
    EEPROM.commit();
  }
  file.close();
  esp_camera_fb_return(fb);

  // Turns off the ESP32-CAM white on-board LED (flash) connected to GPIO 4
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  rtc_gpio_hold_en(GPIO_NUM_4);

  delay(2000);
  Serial.println("Going to sleep now");
  delay(2000);
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}

void loop() {

}

String sendTransaction(const char* hash) {
  StaticJsonDocument<552> doc;

  char json[552];
  strcpy (json, "{\"jsonrpc\":\"2.0\",\"method\":\"eth_sendTransaction\",\"params\":[{\"from\":\"0xe8a5e64E6EBb88F7DdCE7C91723e4d92b73B8FFc\",\"to\":\"0xf2Dee5975A808f16f93bf4Fd55aB5481a8B20497\",\"gas\":\"0x25f90\",\"data\":\"0x4abb7c000000000000000000000000000000000000000000000000000000000000016AA20000000000000000000000000000000000000000000000000000000000000040000000000000000000000000000000000000000000000000000000000000002e");
  strcat (json, hash);
  strcat (json, "000000000000000000000000000000000000\"}], \"id\":1}");

  deserializeJson(doc, json);

  WiFiClient client;
  if (!client.connect(SERVER, 8545)) {
    Serial.println("connection failed");
    return ("connection failed");
  }

  // Write response headers
  client.println(F("POST / HTTP/1.1"));
  client.println(F("cache-control: no-cache"));
  client.println(F("Content-Type: application/json"));
  client.println(F("Connection: keep-alive"));
  client.print(F("Content-Length: "));
  client.println(measureJsonPretty(doc));
  client.println();

  // Write JSON document
  serializeJsonPretty(doc, client);

  delay(20);
  long tOut = millis() + TIMEOUT;
  String serverRes = "";

  while (client.connected() && tOut > millis())
  {
    if (client.available())
    {
      String currentResult = client.readStringUntil('\r');
      serverRes += currentResult;
    }
  }
  return (serverRes);
}

String sendImage(String message, uint8_t *data_pic, size_t size_pic)
{
  String bodyPic =  body("imageFile", message);
  String bodyEnd =  String("--") + BOUNDARY + String("--\r\n");
  size_t allLen = bodyPic.length() + size_pic + bodyEnd.length();
  String headerTxt =  header(allLen);
  WiFiClient client;
  if (!client.connect(SERVER, 5001))
  {
    return ("connection failed");
  }

  client.print(headerTxt + bodyPic);
  client.write(data_pic, size_pic);
  client.print("\r\n" + bodyEnd);

  delay(20);
  long tOut = millis() + TIMEOUT;
  String serverRes = "";
  StaticJsonDocument<300> doc;
  JsonObject responseHash;

  while (client.connected() && tOut > millis())
  {
    if (client.available())
    {
      String currentResult = client.readStringUntil('\r');
      if (currentResult.indexOf("\"Hash\":\"Qm") > 0) {
        DeserializationError error = deserializeJson(doc, currentResult);
        // Test if parsing succeeds.
        if (error) {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.c_str());
        } else {
          serializeJson(doc, Serial);
          const char* ascii_str = doc["Hash"].as<char*>();
          //declare output string with double size of input string
          //because each character of input string will be converted
          //in 2 bytes
          int len = strlen(ascii_str);
          char hex_str[(len * 2) + 1];

          //converting ascii string to hex string
          string2hexString(ascii_str, hex_str);
          String tx = sendTransaction(hex_str);
          Serial.println(tx);
          Serial.println(hex_str);
        }
      }
      serverRes += currentResult;
    }
  }
  return (serverRes);
}
//function to convert ascii char[] to hex-string (char[])
void string2hexString(const char* input, char* output)
{
  int loop;
  int i;

  i = 0;
  loop = 0;

  while (input[loop] != '\0')
  {
    sprintf((char*)(output + i), "%02X", input[loop]);
    loop += 1;
    i += 2;
  }
  //insert NULL at the end of the output string
  output[i++] = '\0';
}
String header(size_t length)
{
  String  data;
  data =  F("POST /api/v0/add HTTP/1.1\r\n");
  data += F("cache-control: no-cache\r\n");
  data += F("Content-Type: multipart/form-data; boundary=");
  data += BOUNDARY;
  data += "\r\n";
  data += F("User-Agent: PostmanRuntime/6.4.1\r\n");
  data += F("Accept: */*\r\n");
  data += F("Host: ");
  data += SERVER;
  data += F("\r\n");
  data += F("accept-encoding: gzip, deflate\r\n");
  data += F("Connection: keep-alive\r\n");
  data += F("content-length: ");
  data += String(length);
  data += "\r\n";
  data += "\r\n";
  return (data);
}

String body(String content , String message)
{
  String data;
  data = "--";
  data += BOUNDARY;
  data += F("\r\n");
  data += F("Content-Disposition: form-data; name=\"imageFile\"; filename=\"");
  data += message;
  data += F("\"\r\n");
  data += F("Content-Type: image/jpeg\r\n");
  data += F("\r\n");
  return (data);
}