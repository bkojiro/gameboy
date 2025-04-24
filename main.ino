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
#define BROWN   0x8A22

#define WIDTH 20

using namespace std;

//screen dimensions: 320 x 480
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

enum ItemType{WEAPON, CONSUMABLE, SCROLL};

class Item {
 public:
  Item(char* name, int newDMG, int val, ItemType type) { //constructor: Item("name", DMG, specialVal, type);
    itemName = name; DMG = newDMG; specialVal = val; itemType = type;
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
  ItemType getType() {
    return itemType;
  }
 private:
  char* itemName = new char[30];
  int DMG; //for weapons
  int specialVal;
  ItemType itemType;
};

Item* Potion = new Item("Potion", 0, 10, CONSUMABLE);

class Room {
 public:
  Room() {
    north = NULL;
    east = NULL;
    south = NULL;
    west = NULL;
    item = NULL;
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
    chest = true;
  }
  Item* getItem() {
    return item;
  }
  bool isChest() {
    return chest;
  }
 private:
  Room* north;
  Room* east;
  Room* south;
  Room* west;
  Item* item;
  bool chest = false;
};

//position stats
int xPos;
int yPos;
int moveSpeed = 1;

//player stats
int health = 40;
int maxHealth = 40;
int mana = 20;
int maxMana = 20;
int atk = 10;
int def = 10;
Item* wielding;
Item* scroll1;
Item* scroll2;

Room* current;
bool textDisplayed;
bool touchingChest;

void setup() {
  Serial.begin(9600);
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.fillScreen(GREY);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  pinMode(12, INPUT);
  tft.setRotation(3);
  Serial.begin(9600);

  xPos = 60;
  yPos = 110;
  tft.fillRect(xPos, yPos, WIDTH, WIDTH, WHITE);
  textDisplayed = false;
  touchingChest = false;

  Item* dagger = new Item("Dagger", 1, 0, WEAPON);
  Item* sword = new Item("Sword", 2, 0, WEAPON);

  Item* infScroll = new Item("Infernal Scroll", 8, 10, SCROLL);

  wielding = dagger;

  Room* entrance = new Room();
  Room* R1 = new Room();
  Room* R2 = new Room();
  
  entrance->setEast(R1);
  entrance->addItem(sword);
  tft.fillRect(200, 150, 20, 20, RED);
  
  R1->setWest(entrance);
  R1->setNorth(R2);
  
  R2->setSouth(R1);
  R2->addItem(infScroll);

  current = entrance;
  RoomRender(current);
  SidebarRender();
}

void loop() {
  Movement();
  ChangeRooms();
  OpenChest();
  Encounter();
}

void Encounter() {
  int eLeft = 200;
  int eRight = 200 + 20;
  int eBottom = 150;
  int eTop = 150 - 20;

  if ((xPos + WIDTH > eLeft && xPos < eRight) && (yPos > eTop && yPos - WIDTH < eBottom)) {
    Serial.println("encounter");
  }
}

bool buttonDown;
bool chestOpened = false;

void OpenChest() {
  if ((xPos >= 318 && xPos <= 392 && yPos <= 72) && (current->isChest())) {
    if (digitalRead(12) == HIGH && buttonDown == false) {
      if (chestOpened) {
        if (current->getItem()->getType() == WEAPON) {
          Item* temp = current->getItem();
          current->addItem(wielding);
          wielding = temp;
          SidebarRender();
          tft.setTextSize(1);
          tft.setTextColor(WHITE);
          tft.setCursor(210, 250);
          tft.print("Equipped: ");
          tft.print(wielding->getName());
        } else if (current->getItem()->getType() == SCROLL) {
          tft.print("Select a scroll slot");
          //open scroll learn/unlearn menu
        } else {
          tft.print("Obtained: ");
          tft.print(current->getItem()->getName());
          //add to inventory...IMPLEMENT LINKED LIST
        }
      } else {
        tft.fillRect(200, 240, 280, 80, BLACK);
        tft.setCursor(210, 250);
        tft.setTextSize(1);
        tft.setTextColor(WHITE);
        if (current->getItem() == NULL) {
          tft.print("Chest is empty");
        } else {
          tft.print("Chest contains: ");
          tft.print(current->getItem()->getName());
          tft.setCursor(210, 260);
          tft.print("DMG: ");
          tft.print(current->getItem()->getDMG());
          tft.setCursor(210, 270);
          if (current->getItem()->getType() == WEAPON) {
            tft.print("Press button to pick up \(this will drop any ");
            tft.setCursor(210, 280);
            tft.print("current weapons\)");
          } else {
            tft.print("Press button to pick up");
          }
        }
        buttonDown = true;
        chestOpened = true;
      }
    }
  }
  if (digitalRead(12) == LOW) {
    buttonDown = false;
  }
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
    //no walking into chest
    if (current->isChest()) {
      if (xPos >= 320 && xPos <= 390 && yPos <= 70) {
        xPos -= moveSpeed * xMove;
        yPos -= moveSpeed * yMove;
        touchingChest = true;
        textDisplayed = true;
        tft.setCursor(210, 250);
        tft.setTextSize(1);
        tft.setTextColor(WHITE);
        tft.println("Press button to open chest");
      } else if (textDisplayed == true) {
        tft.fillRect(200, 240, 280, 80, BLACK);
        textDisplayed = false;
        touchingChest = false;
        chestOpened = false;
      }
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
  if (room->getItem() != NULL) { //GENERATE A CHEST AT 340, 40
    tft.fillRect(340, 40, 50, 30, BROWN);
    tft.fillRect(361, 65, 8, 5, YELLOW);
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
