#include <Adafruit_GFX.h>
#include <Adafruit_TFTLCD.h>
#include <Vector.h>

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

#define BLACK   0x0000
#define GREY    0xBDF7
#define DARKGREY 0x632C
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define PURPLE  0xF810
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define WIDTH 20

using namespace std;

//screen dimensions: 320 x 480
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

class Item {
 public:
  Item(char* name, int DMG, int val, uint16_t color, int width) { //constructor: Item("name", DMG, specialVal, width);
    itemName = name; DMG = DMG; specialVal = val;
    itemX = 0; itemY = 0; itemColor = color; itemWidth = width;
  }
  ~Item() {
    delete itemName;
  }
  char* getName() {
    return itemName;
  }
  int getDMG() {
    return DMG;
  }
  int getSpecial() {
    return specialVal;
  }
  void setX(int x) {
    itemX = x;
  }
  void setY(int y) {
    itemY = y;
  }
  int getX() {
    return itemX;
  }
  int getY() {
    return itemY;
  }
 private:
  char* itemName = new char[30];
  int DMG; //for weapons
  int specialVal;
  int itemX;
  int itemY;
  uint16_t itemColor;
  int itemWidth;
};

Item* Potion = new Item("Potion", 0, 10, RED, 4);

class Room {
 public:
  Room() {
    north = NULL;
    east = NULL;
    south = NULL;
    west = NULL;
  }
  void setNorth(Room* n) {
    north = n;
  }
  void setEast(Room* e) {
    east = e;
  }
  void setSouth(Room* s) {
    south = s;
  }
  void setWest(Room* w) {
    west = w;
  }
  Room* getNorth() {
    return north;
  }
  Room* getEast() {
    return east;
  }
  Room* getSouth() {
    return south;
  }
  Room* getWest() {
    return west;
  }
  void addItem(Item* it) {
    item = it;
  }
  Item* getItem() {
    return item;
  }
 private:
  Room* north;
  Room* east;
  Room* south;
  Room* west;
  Item* item;
};

//position stats
int xPos;
int yPos;
int moveSpeed = 1;

//player stats
int health = 20;
int maxHealth = 20;
int mana = 20;
int maxMana = 20;
int atk = 1;
int def = 1;
Item* wielding;
Room* current;

void setup() {
  Serial.begin(9600);
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.fillScreen(GREY);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  pinMode(12, INPUT);
  tft.setRotation(3);

  xPos = 60;
  yPos = 110;
  tft.fillRect(xPos, yPos, WIDTH, WIDTH, WHITE);
  SidebarRender();

  Item* sword = new Item("Sword", 1, 0, BLUE, 10);

  Room* entrance = new Room();
  Room* R1 = new Room();
  
  entrance->setEast(R1);
  entrance->addItem(sword);
  
  R1->setWest(entrance);

  current = entrance;
  RoomRender(current);
}

void loop() {
  Movement();
  ChangeRooms();
}

void ChangeRooms() {
  if (xPos >= 220 && yPos <= 10) {//go thru north exit
    current = current->getNorth();
    tft.fillRect(0, 0, 480, 240, GREY);
    RoomRender(current);
    xPos = 230;
    yPos = 180;
    tft.fillRect(xPos, yPos, WIDTH, WIDTH, WHITE);
  }
  if (xPos >= 450 && yPos >= 100) {//go thru east exit
    current = current->getEast();
    tft.fillRect(0, 0, 480, 240, GREY);
    RoomRender(current);
    xPos = 40;
    yPos = 110;
    tft.fillRect(xPos, yPos, WIDTH, WIDTH, WHITE);
  }
  if (xPos >= 220 && yPos >= 210) {//go thru north exit
    current = current->getSouth();
    tft.fillRect(0, 0, 480, 240, GREY);
    RoomRender(current);
    xPos = 230;
    yPos = 40;
    tft.fillRect(xPos, yPos, WIDTH, WIDTH, WHITE);
  }
  if (xPos <= 10 && yPos >= 100) {//go thru west exit
    current = current->getWest();
    tft.fillRect(0, 0, 480, 240, GREY);
    RoomRender(current);
    xPos = 420;
    yPos = 110;
    tft.fillRect(xPos, yPos, WIDTH, WIDTH, WHITE);
  }
}

void Movement() {
  int xMove = map(analogRead(A5), 50, 300, 1, -1);
  int yMove = map(analogRead(A4), 50, 300, 1, -1);
  if (xMove !=0 || yMove != 0) {
    if (xMove < 0) { //moving left, replace right
      tft.fillRect(xPos + WIDTH, yPos - 1, moveSpeed, WIDTH + 2, GREY);
    } else if (xMove > 0) { //moving right, replace left
      tft.fillRect(xPos - moveSpeed, yPos - 1, moveSpeed, WIDTH + 2, GREY);
    }
    xPos += moveSpeed * xMove;
    if (yMove < 0) { //moving down, replace up
      tft.fillRect(xPos - 1, yPos + WIDTH, WIDTH + 2, moveSpeed, GREY);
    } else if (yMove > 0) { //moving up, replace down
      tft.fillRect(xPos - 1, yPos, WIDTH + 2, moveSpeed, GREY);
    }
    yPos += moveSpeed * yMove;
    //no walking into walls
    if ((xPos <= 220 && yPos <= 40) || //upper left hori
        (xPos <= 40 && yPos <= 100) || //upper left verti
        (xPos >= 240 && yPos <= 40) || //upper right hori
        (xPos >= 420 && yPos <= 100) || //upper right verti
        (xPos <= 220 && yPos >= 180) || //bottom left hori
        (xPos <= 40 && yPos >= 120) || //bottom left verti 
        (xPos >= 240 && yPos >= 180) || //bottom right hori
        (xPos >= 420 && yPos >= 120)) { //bottom right verti
      xPos -= moveSpeed * xMove;
      yPos -= moveSpeed * yMove;
    }
    //if no entrance, no walking into wall
    if (current->getNorth() == NULL && yPos <= 40) {
      yPos -= moveSpeed * yMove;
    }
    if (current->getEast() == NULL && xPos >= 420) {
      xPos -= moveSpeed * xMove;
    }
    if (current->getSouth() == NULL && yPos >= 180) {
      yPos -= moveSpeed * yMove;
    }
    if (current->getWest() == NULL && xPos <= 40) {
      xPos -= moveSpeed * xMove;
    }
    tft.fillRect(xPos, yPos, WIDTH, WIDTH, WHITE);
  }
}

void RoomRender(Room* room) {
  if (room->getNorth() == NULL) { //north wall
    tft.fillRect(0, 0, 480, 40, DARKGREY);
  } else {
    tft.fillRect(0, 0, 220, 40, DARKGREY);
    tft.fillRect(260, 0, 220, 40, DARKGREY);
  }
  if (room->getEast() == NULL) { //east wall
    tft.fillRect(440, 0, 40, 240, DARKGREY);
  } else {
    tft.fillRect(440, 0, 40, 100, DARKGREY);
    tft.fillRect(440, 140, 40, 100, DARKGREY);
  }
  if (room->getSouth() == NULL) { //south wall
    tft.fillRect(0, 200, 480, 40, DARKGREY);
  } else {
    tft.fillRect(0, 200, 220, 40, DARKGREY);
    tft.fillRect(260, 200, 220, 40, DARKGREY);
  }
  if (room->getWest() == NULL) { //west wall
    tft.fillRect(0, 0, 40, 240, DARKGREY);
  } else {
    tft.fillRect(0, 0, 40, 100, DARKGREY);
    tft.fillRect(0, 140, 40, 100, DARKGREY);
  }
}

void SidebarRender() {
  tft.fillRect(0, 240, 200, 80, WHITE);
  tft.fillRect(200, 240, 280, 80, BLACK);
  //health display
  tft.setCursor(22, 250);
  tft.setTextSize(1);
  tft.setTextColor(BLACK);
  tft.println("HEALTH");
  tft.setCursor(10, 260);
  tft.setTextSize(2);
  tft.setTextColor(RED);
  tft.print(health);
  tft.print("/");
  tft.print(maxHealth);
  //mana display
  tft.setCursor(26, 285);
  tft.setTextSize(1);
  tft.setTextColor(BLACK);
  tft.println("MANA");
  tft.setCursor(10, 295);
  tft.setTextSize(2);
  tft.setTextColor(BLUE);
  tft.print(mana);
  tft.print("/");
  tft.print(maxMana);
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
    tft.println(wielding->getName());
  } else {
    tft.println("NONE");
  }  
}
