#include "Room.h"

Node::Node() {
  north = NULL;
  east = NULL;
  south = NULL;
  west = NULL;
}

Node::~Node() {
  north = NULL;
  east = NULL;
  south = NULL;
  west = NULL;
  delete itemVect;
  delete wpnVect;
}

