#ifndef INVENTORY_H
#define INVENTORY_H

#include <Inventory/Stack.h>

#include <array>

class Inventory {
public:
    Inventory() {}
    Inventory(unsigned int w, unsigned int h);

    Stack* get(unsigned int x, unsigned int y);
    bool set(unsigned int x, unsigned int y, Stack* val);

    bool setNextFree(Stack* val);

    bool addItem(BlockType type);

    BlockType use(unsigned int x, unsigned int y);

private:
    unsigned int width, height;
    Stack** contents;
};

#endif