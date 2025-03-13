#ifndef ROOM_H
#define ROOM_H

#include <Adafruit_TFTLCD.h>
#include <Adafruit_GFX.h>
#include "Item.h"
#include "Weapon.h"

class Room {

public:
 Room();
 ~Room();
 void setNorth(Room* room);
 void setEast(Room* room);
 void setSouth(Room* room);
 void setWest(Room* room);
 Room* getNorth();
 Room* getEast();
 Room* getSouth();
 Room* getWest();
 Item* getItem(Item* findItem);
private:
 Room* north;
 Room* east;
 Room* south;
 Room* west;
 ArrayList<Item*> itemVect;
 ArrayList<Weapon*> wpnVect;
 //add some way to keep track of enemies, shutting down doors?
};

#endif ROOM_H
