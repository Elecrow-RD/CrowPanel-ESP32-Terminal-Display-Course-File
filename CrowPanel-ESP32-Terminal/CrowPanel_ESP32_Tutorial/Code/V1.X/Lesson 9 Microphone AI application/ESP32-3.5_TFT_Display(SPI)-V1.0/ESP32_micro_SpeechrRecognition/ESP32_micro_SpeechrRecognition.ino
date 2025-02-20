#include <driver/i2s.h>   //An I2S (Inter-IC Sound) driver library for ESP32 development boards. I2S is a digital audio interface commonly used for communication between audio devices and microcontrollers. This library provides configuration and operation functions related to I2S and can be used for audio input and output.
#include <SPIFFS.h>       //A file system library for ESP32 that allows you to read and write files as you would on an SD card.
#include <WiFi.h>         
#include <HTTPClient.h>   //The HTTPClient library is the HTTP client library for ESP32. It allows you to send HTTP requests and receive responses, which can be used to interact with Web servers, such as getting web content, sending data to the server, and so on
#include <ArduinoJson.h>
#include <base64.h>       //A library for Base64 encoding and decoding. Base64 is a binary-to-text encoding method commonly used to transmit binary data in text format.
#include <Arduino.h>
#include "FT6236.h"       //A library for the FT6236 touch screen controller. FT6236 is a common touch screen controller, this library provides functions to communicate with the controller and read the touch screen data.
#include <Bounce2.h>      //A library for anti-jitter. It can help you deal with the jitter problem of mechanical buttons and switches and ensure a stable reading of the button state. This library is useful for projects that require stable button input.
#include <s3servo.h>      //A library for servo motor control. It provides functions that control the Angle and speed of the servo motor. This library is useful for projects that require precise control of servo motors.
#include "rgbdisplay.h"

s3servo servo;
int h = 320;
int w = 480;

const int i2c_touch_addr = TOUCH_I2C_ADD;

#define SDA_FT6236 2
#define SCL_FT6236 1

#define I2S_WS 38
#define I2S_SD 41
#define I2S_SCK 39
#define I2S_PORT I2S_NUM_0
#define I2S_SAMPLE_RATE (16000)
#define I2S_SAMPLE_BITS (16)
#define I2S_READ_LEN (16 * 1024)
#define RECORD_TIME (10)  //Recording seconds
#define I2S_CHANNEL_NUM (1)
#define FLASH_RECORD_SIZE (I2S_CHANNEL_NUM * I2S_SAMPLE_RATE * I2S_SAMPLE_BITS / 8 * RECORD_TIME)

File file;
const char filename[] = "/recording.pcm";
const char* ssid = "jinitaim";
const char* password = "11183u491";
bool adc_start_flag = false;
HTTPClient http_client;

/**************************************************************************************
* @file base64Encode
* @brief base64 transcoding of binary audio data 
* @param const char* data  Recording data 
* @param size_t length     length of Recording data 
* @return String encoded
* @date 
***************************************************************************************/
String base64Encode(const char* data, size_t length) 
{
  String encoded = base64::encode((uint8_t*)data, length);
  return encoded;
}

void touch_init() 
{
  Wire.begin(SDA_FT6236, SCL_FT6236);// I2C init
  byte error, address;

  Wire.beginTransmission(i2c_touch_addr);
  error = Wire.endTransmission();

  if (error == 0) 
  {
    Serial.print("I2C device found at address 0x");
    Serial.print(i2c_touch_addr, HEX);
    Serial.println("  !");
  } else if (error == 4) 
  {
    Serial.print("Unknown error at address 0x");
    Serial.println(i2c_touch_addr, HEX);
  }
}

void setup() 
{
  Serial.begin(115200);
  servo.attach(40);  // Connect the servo signal wires to GPIO 40.
  //screen initialization
  pinMode(LCD_CS, OUTPUT);
  pinMode(LCD_BLK, OUTPUT);
  digitalWrite(LCD_CS, LOW);
  digitalWrite(LCD_BLK, HIGH);
  touch_init();
  lcd.init();
  lcd.setRotation(3);    //Set display rotation
  lcd.fillScreen(TFT_WHITE);    //Fill the whole screen with white
  //Set the ui display of the recording button
  lcd.fillRect(190, 260, 100, 60, TFT_YELLOW);
  lcd.setTextSize(2);
  lcd.setTextColor(TFT_BLACK, TFT_YELLOW);
  lcd.setCursor(215, 280);
  lcd.println("start");
  //File system initialization
  SPIFFSInit();
  //Microphone initialization
  i2sInit();
  //WIFI initialization
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print(".");
    delay(200);
  }
  Serial.printf("\r\n-- wifi connect success! --\r\n");
}

void loop() 
{
  int pos[2];       // Define an integer array to store the X and Y coordinates.
  ft6236_pos(pos);  // Call the ft6236_pos function.
  int touchX, touchY;

  touchX = pos[1];
  touchY = pos[0];
  // Read touch screen coordinates
  if (touchX != -1 && touchY != -1 && touchX != 0 && touchY != 0) 
  {
    Serial.print("X: ");
    Serial.print(pos[1]);
    Serial.print(", Y: ");
    Serial.println(pos[0]);
    // Determine whether the touch position is within the key area
    if (touchX > 190 && touchX < 290 && touchY > 260 && touchY < 320) 
    {
      SPIFFS.remove(filename);
      // Opens the file to write new recording data
      file = SPIFFS.open(filename, FILE_WRITE);
      if (!file) 
      {
        Serial.println("File is not available!");
        vTaskDelete(NULL);
        return;
      }
      lcd.fillRect(190, 260, 100, 60, TFT_PINK);
      lcd.setTextSize(2);
      lcd.setTextColor(TFT_YELLOW, TFT_PINK);
      lcd.setCursor(200, 280);
      lcd.println("record");
      Serial.printf("Start recognition\r\n\r\n");
      adc_start_flag = true;
      if (xTaskCreate(i2s_adc, "i2s_adc", 1024 * 8, NULL, 1, NULL) != pdPASS) 
      {
        Serial.println("Failed to create i2s_adc task");
        adc_start_flag = false;
      }
      // Wait for recording to finish
      while (adc_start_flag) 
      {
        vTaskDelay(10);  // Short delay to prevent high CPU usage
      }
      File file = SPIFFS.open(filename, FILE_READ);
      if (!file) 
      {
        Serial.println("Failed to open file for reading");
        return;
      }
      // Get the size of the file
      size_t fileSize = file.size();
      // Allocate a buffer to hold the file content
      char* fileContent = (char*)malloc(fileSize);
      if (!fileContent) 
      {
        Serial.println("Failed to allocate memory for file content");
        file.close();
        return;
      }
      // Read the file content into the buffer
      size_t bytesRead = file.readBytes(fileContent, fileSize);
      file.close();
      // Base64 encode the file content,the length of the audio data becomes 4/3 after base64 encoding
      String base64Encoded = base64Encode(fileContent, bytesRead);
      // Free the buffer as it's no longer needed
      free(fileContent);
      // If you use static allocation arrays, you can replace them with dynamic allocation
      char* data_json = (char*)malloc(427000);  // Allocate memory on the heap

      if (data_json == nullptr) 
      {
        Serial.println("Memory allocation failed for data_json");
        return;  // If the assignment fails, exit
      }
      //Data format of the api interface
      memset(data_json, '\0', strlen(data_json));
      strcat(data_json, "{");
      strcat(data_json, "\"format\":\"pcm\",");
      strcat(data_json, "\"rate\":16000,");
      strcat(data_json, "\"channel\":1,");
      strcat(data_json, "\"cuid\":\"123456PYTHON\",");
      strcat(data_json, "\"token\":\"24.212e692a0414fd8089d2bb5770ba4d30.2592000.1735869824.282335-47362397\",");
      strcat(data_json, "\"dev_pid\":1737,");
      // strcat(data_json, "\"lm_id\":1737,");
      strcat(data_json, "\"speech\":\"");
      strcat(data_json, base64Encoded.c_str());
      strcat(data_json, "\",");
      strcat(data_json, "\"len\":");                
      strcat(data_json, String(fileSize).c_str());  // Append the fileSize as a string
      strcat(data_json, "}");
      int httpCode;
      http_client.setTimeout(5000);
      http_client.begin("http://vop.baidu.com/server_api");
      http_client.addHeader("Content-Type:", "application/json");

      httpCode = http_client.POST(data_json);

      if (httpCode == 200) {
        String response = http_client.getString();
        http_client.end();
        Serial.println(response);
        DynamicJsonDocument jsonDoc(512);
        deserializeJson(jsonDoc, response);
        String outputText01 ="";
        String outputText = jsonDoc["result"][0];
        outputText01 = outputText;
        //Print recognized speech text
        lcd.setTextSize(3);
        lcd.setCursor(0, 0);
        lcd.setTextColor(TFT_BLACK, TFT_WHITE);
        lcd.println(outputText01);
        //Determine if the door is open and closed
        if (outputText == "open the door") 
        {
          servo.write(90);  // Turn the steering gear to 90 degrees
        }
        if (outputText == "close the door") 
        {
          servo.write(0);  // Turn the steering gear to 0 degrees
        }
        Serial.println(outputText);
      } 
      else 
      {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http_client.errorToString(httpCode).c_str());
      }
      free(data_json);     //Free memory
      Serial.printf("Recognition complete\r\n");
      lcd.fillRect(190, 260, 100, 60, TFT_YELLOW);
      lcd.setTextSize(2);
      lcd.setTextColor(TFT_BLACK, TFT_YELLOW);
      lcd.setCursor(215, 280);
      lcd.println("start");
    }
  }
  vTaskDelay(1);
}

void SPIFFSInit() 
{
  if (!SPIFFS.begin(true)) 
  {
    Serial.println("SPIFFS initialisation failed!");
    while (1) yield();
  }
}

void i2sInit() 
{
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = I2S_SAMPLE_RATE,
    .bits_per_sample = i2s_bits_per_sample_t(I2S_SAMPLE_BITS),
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = 0,
    .dma_buf_count = 64,
    .dma_buf_len = 1024,  // 1024 samples per buffer
    .use_apll = 1         // use APLL-CLK, frequency 16MHZ-128MHZ, it's for audio
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);

  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = -1,
    .data_in_num = I2S_SD
  };

  i2s_set_pin(I2S_PORT, &pin_config);
}

/**************************************************************************************
* @file i2s_adc
* @brief The microphone performs recording tasks 
* @param void* arg 
* @return null
* @date 
***************************************************************************************/
void i2s_adc(void* arg) 
{
  int i2s_read_len = I2S_READ_LEN;
  int flash_wr_size = 0;
  size_t bytes_read;
  char* i2s_read_buff = (char*)calloc(i2s_read_len, sizeof(char));
  Serial.println(" *** Recording Start *** ");
  while (adc_start_flag && flash_wr_size < FLASH_RECORD_SIZE) 
  {
    i2s_read(I2S_PORT, (void*)i2s_read_buff, i2s_read_len, &bytes_read, portMAX_DELAY);   //The i2s reads the recording data
    if (bytes_read > 0) 
    {
      if ((flash_wr_size + bytes_read) > FLASH_RECORD_SIZE) 
      {
        Serial.println("Buffer overflow, reducing read size");
        bytes_read = FLASH_RECORD_SIZE - flash_wr_size;  // Prevent overflow
      }
      file.write((const byte*)i2s_read_buff, bytes_read);   //Recording data is written to the esp32 file system
      flash_wr_size += bytes_read;
      ets_printf("Sound recording %u%%\n", flash_wr_size * 100 / FLASH_RECORD_SIZE);    //The recording progress is displayed on the serial port
    } 
    else 
    {
      Serial.println("No data read from I2S");
      break;
    }
  }
  free(i2s_read_buff);
  i2s_read_buff = NULL;
  file.close();
  listSPIFFS();   //Print all files in the file system and their sizes in the serial port
  adc_start_flag = false;  // Mark end of recording
  vTaskDelete(NULL);
}

/**************************************************************************************
* @file listSPIFFS
* @brief TPrint all files in the file system and their sizes in the serial port
* @param null
* @return null
* @date 
***************************************************************************************/
void listSPIFFS(void) 
{
  Serial.println(F("\r\nListing SPIFFS files:"));
  static const char line[] PROGMEM = "=================================================";
  Serial.println(FPSTR(line));
  Serial.println(F("  File name                              Size"));
  Serial.println(FPSTR(line));

  fs::File root = SPIFFS.open("/");
  if (!root) 
  {
    Serial.println(F("Failed to open directory"));
    return;
  }
  if (!root.isDirectory()) 
  {
    Serial.println(F("Not a directory"));
    return;
  }

  fs::File file = root.openNextFile();
  while (file) 
  {
    if (file.isDirectory()) 
    {
      Serial.print("DIR : ");
      String fileName = file.name();
      Serial.print(fileName);
    } 
    else 
    {
      String fileName = file.name();
      Serial.print("  " + fileName);
      int spaces = 33 - fileName.length();  // Tabulate nicely
      if (spaces < 1) spaces = 1;
      while (spaces--) Serial.print(" ");
      String fileSize = (String)file.size();
      spaces = 10 - fileSize.length();  // Tabulate nicely
      if (spaces < 1) spaces = 1;
      while (spaces--) Serial.print(" ");
      Serial.println(fileSize + " bytes");
    }
    file = root.openNextFile();
  }

  Serial.println(FPSTR(line));
  Serial.println();
  delay(1000);
}