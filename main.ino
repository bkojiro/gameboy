#include <Adafruit_GFX.h>
#include <Adafruit_TFTLCD.h>

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0

#define LCD_RESET A4

#define	BLACK   0x0000
#define GREY    0xAAAA
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
 
//dimensions of screen: 320 x 480 
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

struct Weapon {
  int dmg = 0;
  char* wpnName;
  bool hasSpecial = false;
  int specialStat = 0; //if weapon has a special ability, store here
};

//player stats
int health = 20;
int maxHealth = 20;
int mana = 20;
int maxMana = 20;
int atk = 1;
int def = 1;
Weapon* wielding;

void setup() {
  Serial.begin(9600);
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.fillScreen(BLACK);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  pinMode(12, INPUT);
  xPos = 0;
  yPos = 100;
  tft.setRotation(3);
  tft.fillRect(xPos, yPos, WIDTH, WIDTH, WHITE);
  SidebarRender();

  Weapon* sword = new Weapon();
  sword->dmg = 1;
  sword->wpnName = "Sword";
}

void loop() {
  Movement();
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
    if (xPos <= 0 || xPos >= 460) xPos -= moveSpeed * xMove;
    if (yMove < 0) { //moving down, replace up
      tft.fillRect(xPos - 1, yPos + WIDTH, WIDTH + 2, moveSpeed, BLACK);
    } else if (yMove > 0) { //moving up, replace down
      tft.fillRect(xPos - 1, yPos, WIDTH + 2, moveSpeed, BLACK);
    }
    yPos += moveSpeed * yMove;
    if (yPos <= 0 || yPos >= 220) yPos -= moveSpeed * yMove;
    tft.fillRect(xPos, yPos, WIDTH, WIDTH, WHITE);
  }
}

void SidebarRender() {
  tft.fillRect(0, 240, 200, 80, WHITE);
  tft.fillRect(200, 240, 280, 80, GREY);
  //health display
  tft.setCursor(22, 250);
  tft.setTextSize(1);
  tft.setTextColor(BLACK);
  tft.println("HEALTH");
  tft.setCursor(10, 260);
  tft.setTextSize(2);
  tft.setTextColor(RED);
  tft.println("20/20"); //change this to actual code later
  //mana display
  tft.setCursor(26, 285);
  tft.setTextSize(1);
  tft.setTextColor(BLACK);
  tft.println("MANA");
  tft.setCursor(10, 295);
  tft.setTextSize(2);
  tft.setTextColor(BLUE);
  tft.println("20/20");
  //stat display
  tft.setCursor(85, 250);
  tft.setTextColor(BLACK);
  tft.println("ATK:");
  tft.setCursor(135, 250);
  tft.println(atk);
  tft.setCursor(85, 275);
  tft.println("DEF:");
  tft.setCursor(135, 275);
  tft.println(def);
  //weapon display
  tft.setCursor(85, 300);
  tft.println("WPN:");
  tft.setTextSize(1);
  tft.setCursor(135, 300);
  if (wielding != NULL) {
    tft.println(wielding->wpnName);
  } else {
    tft.println("NONE");
  }  
}
