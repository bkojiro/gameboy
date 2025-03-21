#include <Adafruit_GFX.h>
#include <Adafruit_TFTLCD.h>
#include <Vector.h>

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
#define PURPLE  0xF810
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define WIDTH 20

//screen dimensions: 320 x 480
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

class Item {
 public:
  Item(char* name, int DMG, int val, int width) { //constructor: Item("name", DMG, specialVal, width);
    itemName = name; DMG = DMG; specialVal = val;
    itemX = 0; itemY = 0; itemWidth = width;
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
  //int itemColor;
  int itemWidth;
};

Item* Potion = new Item("Potion", 0, 10, 4);

typedef Vector<Item*> ItemVect;

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
  void addItem(Item* item) {
    items->push_back(item);
  }
  Item* getItem(char* findItem) {
    Item* tempItem;
    for (int a = 0; items[a] != items->end(); a++) {
      //this is broken, please fix or i will kms
    }
    return tempItem;
  }
 private:
  Room* north;
  Room* east;
  Room* south;
  Room* west;
  ItemVect* items;
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

void setup() {
  Serial.begin(9600);
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.fillScreen(GREY);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  pinMode(12, INPUT);
  tft.setRotation(3);

  xPos = 0;
  yPos = 100;
  tft.fillRect(xPos, yPos, WIDTH, WIDTH, WHITE);
  SidebarRender();

  Item* sword = new Item("Sword", 1, 0, 10);

  Room* entrance = new Room();
  entrance->east = R1;
  items.push_back(sword);

  Room* R1 = new Room();
  R1->west = entrance;

  RoomRender(entrance);
}

void loop() {
  Movement();
}

void Movement() {
  int xMove = map(analogRead(A5), 50, 300, 1, -1);
  int yMove = map(analogRead(A4), 50, 300, 1, -1);
  if (xMove !=0 || yMove != 0) {
    if (xMove < 0) { //moving left, replace right
      tft.fillRect(xPos + WIDTH, yPos - 1, moveSpeed, WIDTH + 2, BLACK);
    } else if (xMove > 0) { //moving right, replace left
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

void RoomRender(Room* room) {
  if (room->north == NULL) {
    tft.fillRect(0, 0, 480, 40, PURPLE);
  } else {

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
    tft.println(wielding->itemName);
  } else {
    tft.println("NONE");
  }  
}
