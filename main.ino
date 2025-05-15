#include <Adafruit_GFX.h>
#include <Adafruit_TFTLCD.h>

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
  Item(char* name, int newDMG, int ncrit, int val, ItemType type) { //constructor: Item("name", DMG, crit%, specialVal, type);
    itemName = name; DMG = newDMG; crit = ncrit; specialVal = val; itemType = type;
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
  int getCrit() {
    return crit;
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
  int crit;
  int specialVal;
  ItemType itemType;
};

Item* Potion = new Item("Life Potion", 0, 0, 10, CONSUMABLE);
Item* MPotion = new Item("Mana Potion", 0, 0, 10, CONSUMABLE);

enum EnemyType{GOBLIN, SKELE, TROLL, MINOTAUR, LICH};

class Enemy {
  public:
   Enemy(EnemyType type) {
    Type = type;
    x = random(140, 280);
    y = random(80, 140);
    if (type == GOBLIN) {
      name = "GOBLIN";
      width = 20;
      HP = random(9, 13); //9-12
      ATK = random(9, 12); //9-11
      DEF = random (14, 16); //14-15
    } else if (type == SKELE) {
      name = "SKELETON";
      width = 20;
      HP = random(7, 11); //7-10
      ATK = random(16, 19); //16-18
      DEF = random (9, 12); //9-11
    } else if (type == TROLL) {
      name = "TROLL";
      width = 40;
      HP = random(16, 20); //16-19
      ATK = random(14, 17); //14-16
      DEF = random (15, 19); //15-18
    } else if (type == MINOTAUR) {
      name = "MINOTAUR";
      width = 50;
      HP = 26;
      ATK = 18;
      DEF = 16;
    } else if (type == LICH) {
      name = "LICH";
      width = 80;
      x = 200;
      y = 120;
      HP = 20;
      ATK = 20;
      DEF = 20;
    }
   }
   ~Enemy() {
    delete name;
   }
   char* getName() {
    return name;
   }
   void setHP(int newhp) {
    HP = newhp;
   }
   int getHP() {
    return HP;
   }
   void setATK(int newatk) {
    ATK = newatk;
   }
   int getATK() {
    return ATK;
   }
   void setDEF(int newdef) {
    DEF = newdef;
   }
   int getDEF() {
    return DEF;
   }
   EnemyType getType() {
    return Type;
   }
   int getX() {
    return x;
   }
   int getY() {
    return y;
   }
   int getWidth() {
    return width;
   }
  private:
   char* name = new char[20];
   EnemyType Type;
   int HP;
   int ATK;
   int DEF;
   int x;
   int y;
   int width;
};

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
  Enemy* getEnemy() {
    return enemy;
  }
  void setEnemy(Enemy* e) {
    enemy = e;
  }
 private:
  Room* north;
  Room* east;
  Room* south;
  Room* west;
  Item* item;
  bool chest = false;
  Enemy* enemy;
};

int DMG_MULTI = 1; //increase this as game progresses

//position stats
int xPos;
int yPos;
int moveSpeed = 1;

//player stats
int health = 20;
int maxHealth = 20;
int mana = 20;
int maxMana = 20;
int atk = 10;
int def = 10;
int crit = 10;
Item* wielding;
Item* scroll1;
Item* scroll2;

Room* current;
bool textDisplayed;
bool touchingChest;
bool battlePhase;

void setup() {
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.fillScreen(GREY);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  pinMode(12, INPUT);
  tft.setRotation(3);
  Serial.begin(9600);
  randomSeed(analogRead(A5));

  xPos = 60;
  yPos = 110;
  tft.fillRect(xPos, yPos, WIDTH, WIDTH, WHITE);
  textDisplayed = false;
  touchingChest = false;
  battlePhase = false;

  Item* dagger = new Item("Dagger", 1, 10, 0, WEAPON);
  Item* sword = new Item("Sword", 4, 20, 0, WEAPON);

  Item* infScroll = new Item("Infernal Scroll", 8, 0, 10, SCROLL);

  wielding = dagger;

  Room* entrance = new Room();
  Room* R1 = new Room();
  Room* R2 = new Room();
  
  //entrance
  entrance->setEast(R1);
  entrance->addItem(sword);
  Enemy* e1 = new Enemy(TROLL);
  entrance->setEnemy(e1);
  //r1
  R1->setWest(entrance);
  R1->setNorth(R2);
  //r2
  R2->setSouth(R1);
  R2->addItem(infScroll);

  current = entrance;
  RoomRender(current);
  SidebarRender();
}

bool inCombat = false;

void loop() {
  if (!battlePhase) {
    Movement();
    ChangeRooms();
    OpenChest();
    battlePhase = Encounter(current->getEnemy());
  } else {
    inCombat = true;
    Battle(current->getEnemy());
  }
}

void Battle(Enemy* e) {
  tft.fillRect(0, 0, 480, 240, GREY); //bg
  tft.fillRect(0, 160, 480, 80, DARKGREY); //ground
  tft.fillRect(100, 120, WIDTH * 2, WIDTH * 2, WHITE); //player
  tft.fillRect(380 - (e->getWidth() * 2), 160 - (e->getWidth() * 2), e->getWidth() * 2, e->getWidth() * 2, RED); //enemy
  //player/enemy text
  tft.setCursor(60, 20);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.println("PLAYER");
  tft.setCursor(340, 20);
  tft.setTextColor(RED);
  tft.println(e->getName());
  tft.setCursor(340, 40);
  tft.setTextColor(BLACK);
  tft.print("HP: ");
  tft.print(e->getHP());
  //option text
  tft.setTextColor(WHITE);
  tft.setCursor(60, 190);
  tft.print("SMITE");
  tft.setCursor(210, 190);
  tft.print("MAGIC");
  tft.setCursor(360, 190);
  tft.print("ITEM");
  tft.fillRect(40, 192, 10, 10, WHITE);
  
  int SMITE = 1; int MAGIC = 2; int ITEM = 3;
  int option = SMITE;
  int PLAYER = 0; int ENEMY = 1;
  int turn = PLAYER;
  while (inCombat) {     
    //player turn
    while (turn == PLAYER) {
      int xMove = map(analogRead(A5), 50, 300, 1, -1);
      if (xMove == 1) {
        if (option != ITEM) {
          tft.fillRect(40 + (150 * (option - 1)), 192, 10, 10, DARKGREY);
          tft.fillRect(40 + (150 * option), 192, 10, 10, WHITE);
          option++;
        }
      } else if (xMove == -1) {
        if (option != SMITE) {
          tft.fillRect(40 + (150 * (option - 1)), 192, 10, 10, DARKGREY);
          tft.fillRect(40 + (150 * (option - 2)), 192, 10, 10, WHITE);
          option--;
        }
      }
      if (digitalRead(12) == HIGH) {
        if (option == SMITE) {
          for (int i = 0; i < 50; i++) {
            tft.fillRect(100 + (i * 3), 120, WIDTH * 2, WIDTH * 2, WHITE);
            tft.fillRect(100 + ((i - 1) * 3), 120, 3, WIDTH * 2, GREY);
            delay(5);
          }
          //calculate new health
          int DMG = round(wielding->getDMG() * atk * (1/((float)e->getDEF())));
          int CRIT = random(1, 100);
          int critChance = crit + wielding->getCrit();
          if (CRIT < critChance) DMG = DMG * 2;
          e->setHP(e->getHP() - DMG);
          if (e->getHP() < 0) e->setHP(0);
          Serial.println(DMG);
          //display new health
          tft.fillRect(340, 40, 80, 20, GREY);
          tft.setCursor(340, 40);
          tft.setTextColor(BLACK);
          tft.print("HP: ");
          tft.print(e->getHP());
          for (int i = 0; i < 50; i++) {
            tft.fillRect(250 - (i * 3), 120, WIDTH * 2, WIDTH * 2, WHITE);
            tft.fillRect(250 + WIDTH * 2 - ((i - 1) * 3), 120, 3, WIDTH * 2, GREY);
            delay(5);
          }
        } else if (option == MAGIC) {

        } else if (option == ITEM) {

        }
        turn = ENEMY;
      }
      delay(100);
    }
    if (e->getHP() == 0) { //win battle!
      inCombat = false;
    }
    //enemy turn
    int DMG = 2 * e->getATK() * DMG_MULTI * (1/(float)def);
    for (int i = 0; i < 50; i++) {
      tft.fillRect(380 - e->getWidth() * 2 - (i * 3), 160 - (e->getWidth() * 2), e->getWidth() * 2, e->getWidth() * 2, RED);
      tft.fillRect(380 - ((i - 1) * 3), 160 - (e->getWidth() * 2), 3, e->getWidth() * 2, GREY);
      delay(5);
    }
    for (int i = 0; i < 50; i++) {
      tft.fillRect(230 - e->getWidth() * 2 + (i * 3), 160 - (e->getWidth() * 2), e->getWidth() * 2, e->getWidth() * 2, RED);
      tft.fillRect(230 - e->getWidth() * 2 + ((i - 1) * 3), 160 - (e->getWidth() * 2), 3, e->getWidth() * 2, GREY);
      delay(5);
    }
    turn = PLAYER;
    if (health == 0) { //lose battle
      inCombat = false;
    }
  }
}

bool Encounter(Enemy* e) {
  if (e != NULL) {
    int eLeft = e->getX();
    int eRight = e->getX() + e->getWidth();
    int eBottom = e->getY()+ e->getWidth();
    int eTop = e->getY();
    if ((xPos + WIDTH > eLeft && xPos < eRight) && (yPos + WIDTH > eTop && yPos < eBottom)) {
      //enter battle phase
      return true;
    }
    return false;
  }
  return false;
}

bool buttonDown;
bool chestOpened = false;

void OpenChest() {
  if ((xPos >= 318 && xPos <= 392 && yPos <= 72) && (current->isChest())) {
    if (digitalRead(12) == HIGH && buttonDown == false) {
      if (chestOpened) {
        tft.setTextSize(1);
        tft.setTextColor(WHITE);
        tft.setCursor(210, 250);
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
          tft.fillRect(200, 240, 280, 80, BLACK);
          tft.print("Select a scroll slot");
          tft.setCursor(215, 260);
          if (scroll1 != NULL) {
            tft.print(scroll1->getName());
          } else {
            tft.print("EMPTY");
          }
          tft.setCursor(215, 270);
          if (scroll2 != NULL) {
            tft.print(scroll2->getName());
          } else {
            tft.print("EMPTY");
          }
          tft.drawRect(210, 261, 4, 4, GREEN);
          //open scroll learn/unlearn menu
          bool scrolling = true;
          int option = 0;
          while (scrolling) {
            delay(200);  
            int yMove = map(analogRead(A4), 50, 300, 1, -1);
            if (yMove == 1) {
              if (option != 1) {
                tft.drawRect(210, 261, 4, 4, BLACK);
                tft.drawRect(210, 271, 4, 4, GREEN);
                option++;
              }
            } else if (yMove == -1) {
              if (option != 0) {
                tft.drawRect(210, 271, 4, 4, BLACK);
                tft.drawRect(210, 261, 4, 4, GREEN);
                option--;
              }
            }
            if (digitalRead(12) == HIGH) {
              if (option == 0) { //scroll spot 1
                scroll1 = current->getItem();
              }
              SidebarRender();
              tft.setTextSize(1);
              tft.setTextColor(WHITE);
              tft.setCursor(210, 250);
              tft.print("Equipped: ");
              tft.print(wielding->getName());
              scrolling = false;
            }
          }
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
  if (room->isChest() != NULL) { //GENERATE A CHEST AT 340, 40
    tft.fillRect(340, 40, 50, 30, BROWN);
    tft.fillRect(361, 65, 8, 5, YELLOW);
  }
  if (room->getEnemy() != NULL) {
    Enemy* e = room->getEnemy();
    tft.fillRect(e->getX(), e->getY(), e->getWidth(), e->getWidth(), RED);
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
