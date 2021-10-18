#ifndef STACK_H
#define STACK_H

#include <Block/Block.h>

class Stack {
public:
    Stack() {}
    Stack(BlockType t, unsigned int n, unsigned int m) : type(t), num(n), max(m) {}

    bool add();
    bool remove();

    BlockType getType();
    unsigned int getNum();

private:
    BlockType type;
    unsigned int num;
    unsigned int max;
};

#endif