#include "rgbdisplay.h"
#include "Latin_Hiragana_24.h"
#include "NotoSansBold15.h"
#include "NotoSansMonoSCB20.h"

#define latin Latin_Hiragana_24
#define small NotoSansBold15
#define digits NotoSansMonoSCB20

#define c1 0xE73C //white
#define c2 0x18C3  //gray
#define c3 0x9986 //red
#define c4 0x2CAB  //green
#define c5 0xBDEF //gold
#define c6 0xFFFAFA //snow

void setup() {

  pinMode(LCD_CS, OUTPUT);
  pinMode(LCD_BLK, OUTPUT);

  digitalWrite(LCD_CS, LOW);
  digitalWrite(LCD_BLK, HIGH);

  lcd.init();
  lcd.setRotation(1);
  lcd.fillScreen(TFT_WHITE);
  
  sprite.createSprite(480,320);
}

void loop()
{
  sprite.fillRect(80,61,310,50,0x9986);//Draw a rectangle
  sprite.drawLine(80,164,80,240,0x2CAB);//Draw a line
  sprite.loadFont(small);
  sprite.setTextColor(c4,c3);
  sprite.drawString("Hello elecrow!",190,230);//Write a paragraph
  sprite.drawCircle(240, 160, 30, c5);//Draw a circle
  sprite.pushSprite(0,0);
}
