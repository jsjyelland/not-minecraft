#include <Inventory/Stack.h>

bool Stack::add() {
    if (num < max) {
        num++;

        return true;
    }

    return false;
}

bool Stack::remove() {
    if (num > 0) {
        num--;

        return true;
    }

    return false;
}

BlockType Stack::getType() {
    return type;
}

unsigned int Stack::getNum() {
    return num;
}