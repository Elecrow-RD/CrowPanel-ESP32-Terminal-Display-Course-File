#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <FS.h>

int sd_init_flag = 0;

#define SPI_MOSI 2 
#define SPI_MISO 41
#define SPI_SCK 42
#define SD_CS 1 

#define LCD_CS -1
#define LCD_BLK 46

#define I2C_SCL 39
#define I2C_SDA 38
class LGFX : public lgfx::LGFX_Device
{
    lgfx::Panel_ILI9488 _panel_instance;
    lgfx::Bus_Parallel16 _bus_instance;
  public:
   
    LGFX(void)
    {
      {
        auto cfg = _bus_instance.config();


        cfg.port = 0; 
        cfg.freq_write = 20000000; 
        cfg.pin_wr = 18; 
        cfg.pin_rd = 48; 
        cfg.pin_rs = 45; 

        cfg.pin_d0 = 47;
        cfg.pin_d1 = 21;
        cfg.pin_d2 = 14;
        cfg.pin_d3 = 13;
        cfg.pin_d4 = 12;
        cfg.pin_d5 = 11;
        cfg.pin_d6 = 10;
        cfg.pin_d7 = 9;
        cfg.pin_d8 = 3;
        cfg.pin_d9 = 8;
        cfg.pin_d10 = 16;
        cfg.pin_d11 = 15;
        cfg.pin_d12 = 7;
        cfg.pin_d13 = 6;
        cfg.pin_d14 = 5;
        cfg.pin_d15 = 4;

        _bus_instance.config(cfg);              
        _panel_instance.setBus(&_bus_instance);
      }

      { 
        auto cfg = _panel_instance.config(); 

        cfg.pin_cs = -1;
        cfg.pin_rst = -1; 
        cfg.pin_busy = -1;
       

        cfg.memory_width = 320;
        cfg.memory_height = 480;
        cfg.panel_width = 320; 
        cfg.panel_height = 480; 
        cfg.offset_x = 0; 
        cfg.offset_y = 0;
        cfg.offset_rotation = 0; 
        cfg.dummy_read_pixel = 8;
        cfg.dummy_read_bits = 1;
        cfg.readable = true; 
        cfg.invert = false; 
        cfg.rgb_order = false;
        cfg.dlen_16bit = true; 
        cfg.bus_shared = true;
        _panel_instance.config(cfg);
      }

      setPanel(&_panel_instance); 
    }
};


LGFX lcd;
SPIClass SD_SPI;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  lcd.init();
  //lcd.setSwapBytes(true);
  lcd.fillScreen(TFT_BLACK);
  delay(500);

  pinMode(LCD_BLK, OUTPUT);
  digitalWrite(LCD_BLK, HIGH);
  if (SD_init() == 0)
  {
    Serial.println("TF card initialization succeeded");
    lcd.fillScreen(TFT_BLACK);
    lcd.setCursor(115, 230);
    lcd.println("TF card successfully mounted");
  } else {
    Serial.println("TF card initialization failed");
    lcd.fillScreen(TFT_BLACK);
    lcd.setCursor(115, 230);
    lcd.println("TF card failed to mount");
  }
  lcd.printf("Setup done");
  lcd.setRotation(7);
  lcd.fillScreen(TFT_BLACK);

}
 
void loop() {

  print_img(SD, "/1.bmp", 480, 320);
  delay(5000);
  print_img(SD, "/2.bmp", 480, 320);
  delay(5000);
  print_img(SD, "/3.bmp", 480, 320);
  delay(5000);
  print_img(SD, "/4.bmp", 480, 320);
  delay(1000);

}

// Display image from file
int print_img(fs::FS & fs, String filename, int x, int y)
{

  File f = fs.open(filename, "r");
  if (!f)
  {
    Serial.println("Failed to open file for reading");
    f.close();
    return 0;
  }

  f.seek(54);
  int X = x;
  int Y = y;
  uint8_t RGB[3 * X];
  for (int row = 0; row < Y; row++)
  {
    f.seek(54 + 3 * X * row);
    f.read(RGB, 3 * X);

    lcd.pushImage(0, row, X, 1, (lgfx::rgb888_t *)RGB);
  }

  f.close();
  return 0;
}

int SD_init()
{
  SD_SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  if (!SD.begin(SD_CS, SD_SPI, 40000000))
  {
    Serial.println("Card Mount Failed");
    return 1;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE)
  {
    Serial.println("No TF card attached");
    return 1;
  }
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("TF Card Size: %lluMB\n", cardSize);
  listDir(SD, "/", 2);
  return 0;
}


// SD test
void listDir(fs::FS & fs, const char *dirname, uint8_t levels)
{
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root)
  {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory())
  {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file)
  {
    if (file.isDirectory())
    {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels)
      {
        listDir(fs, file.name(), levels - 1);
      }
    }
    else
    {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void createDir(fs::FS & fs, const char *path)
{
  Serial.printf("Creating Dir: %s\n", path);
  if (fs.mkdir(path))
  {
    Serial.println("Dir created");
  }
  else
  {
    Serial.println("mkdir failed");
  }
}

void removeDir(fs::FS & fs, const char *path)
{
  Serial.printf("Removing Dir: %s\n", path);
  if (fs.rmdir(path))
  {
    Serial.println("Dir removed");
  }
  else
  {
    Serial.println("rmdir failed");
  }
}

void readFile(fs::FS & fs, const char *path)
{
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if (!file)
  {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while (file.available())
  {
    Serial.write(file.read());
  }
  file.close();
}

void writeFile(fs::FS & fs, const char *path, const char *message)
{
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file)
  {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message))
  {
    Serial.println("File written");
  }
  else
  {
    Serial.println("Write failed");
  }
  file.close();
}

void appendFile(fs::FS & fs, const char *path, const char *message)
{
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file)
  {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message))
  {
    Serial.println("Message appended");
  }
  else
  {
    Serial.println("Append failed");
  }
  file.close();
}

void renameFile(fs::FS & fs, const char *path1, const char *path2)
{
  Serial.printf("Renaming file %s to %s\n", path1, path2);
  if (fs.rename(path1, path2))
  {
    Serial.println("File renamed");
  }
  else
  {
    Serial.println("Rename failed");
  }
}

void deleteFile(fs::FS & fs, const char *path)
{
  Serial.printf("Deleting file: %s\n", path);
  if (fs.remove(path))
  {
    Serial.println("File deleted");
  }
  else
  {
    Serial.println("Delete failed");
  }
}

void testFileIO(fs::FS & fs, const char *path)
{
  File file = fs.open(path);
  static uint8_t buf[512];
  size_t len = 0;
  uint32_t start = millis();
  uint32_t end = start;
  if (file)
  {
    len = file.size();
    size_t flen = len;
    start = millis();
    while (len)
    {
      size_t toRead = len;
      if (toRead > 512)
      {
        toRead = 512;
      }
      file.read(buf, toRead);
      len -= toRead;
    }
    end = millis() - start;
    Serial.printf("%u bytes read for %u ms\n", flen, end);
    file.close();
  }
  else
  {
    Serial.println("Failed to open file for reading");
  }

  file = fs.open(path, FILE_WRITE);
  if (!file)
  {
    Serial.println("Failed to open file for writing");
    return;
  }

  size_t i;
  start = millis();
  for (i = 0; i < 2048; i++)
  {
    file.write(buf, 512);
  }
  end = millis() - start;
  Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
  file.close();
}
