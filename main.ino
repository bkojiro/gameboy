//UPDATE THIS USING NEW MAIN FILE

#include <Adafruit_GFX.h>
#include <Adafruit_TFTLCD.h>

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0

#define LCD_RESET A4

#define BLACK   0x0000
#define GREY    0xAAAA
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
 
#define WIDTH 20
#define NORTH 1
#define EAST 2
#define SOUTH 3
#define WEST 4

//position stats
int xPos;
int yPos;
int moveSpeed = 1;
int facing = 2;

//player stats
int lives = 2;

void setup() {
  Serial.begin(9600);
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.fillScreen(BLACK);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  pinMode(12, INPUT);
  xPos = 230;
  yPos = 150;
  tft.setRotation(3);
 
  SidebarRender();
}


void loop() {
  Movement();
  if (digitalRead(12) == 1) {
    if (facing == EAST) {
      //swipe to the right
     
    } else if (facing == WEST) {
      //swipe to left


    }
  }
}


void Movement() {
  int xMove = map(analogRead(A5), 50, 300, 1, -1);
  int yMove = map(analogRead(A4), 50, 300, 1, -1);
  if (xMove !=0 || yMove != 0) {
    if (xMove < 0) { //moving left, replace right
      facing = WEST;
      tft.fillRect(xPos + WIDTH, yPos - 1, moveSpeed, WIDTH + 2, BLACK);
    } else if (xMove > 0) { //moving right, replace left
      facing = EAST;
      tft.fillRect(xPos - moveSpeed, yPos - 1, moveSpeed, WIDTH + 2, BLACK);
    }
    xPos += moveSpeed * xMove;
    if (xPos <= 80 || xPos >= 460) xPos -= moveSpeed * xMove;
    if (yMove < 0) { //moving down, replace up
      tft.fillRect(xPos - 1, yPos + WIDTH, WIDTH + 2, moveSpeed, BLACK);
    } else if (yMove > 0) { //moving up, replace down
      tft.fillRect(xPos - 1, yPos, WIDTH + 2, moveSpeed, BLACK);
    }
    yPos += moveSpeed * yMove;
    if (yPos <= 0 || yPos >= 300) yPos -= moveSpeed * yMove;
    tft.fillRect(xPos, yPos, WIDTH, WIDTH, WHITE);
  }
}


void SidebarRender() {
  tft.fillRect(0, 0, 80, 320, WHITE);
  //health display
  tft.setCursor(22, 10);
  tft.setTextSize(1);
  tft.setTextColor(BLACK);
  tft.println("HEALTH");
  tft.setCursor(10, 20);
  tft.setTextSize(2);
  tft.setTextColor(RED);
  tft.println("20/20"); //change this to actual code later
  //mana display
  tft.setCursor(26, 45);
  tft.setTextSize(1);
  tft.setTextColor(BLACK);
  tft.println("MANA");
  tft.setCursor(10, 55);
  tft.setTextSize(2);
  tft.setTextColor(BLUE);
  tft.println("20/20");
  //weapon display
  tft.setCursor(20, 70);
  tft.setTextSize(1);
  tft.setTextColor(BLACK);
  tft.println("Weapon");
  tft.setCursor(20, 80);
  tft.println("Sword");
}
