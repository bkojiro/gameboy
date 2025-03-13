#ifndef ITEM_H
#define ITEM_H

class Item {

public:
 Item();
 ~Item();
 void setName(char* name);
 char* getName();
private:
 char* itemName;

};

#endif ROOM_H
