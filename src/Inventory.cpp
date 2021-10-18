#include <Inventory/Inventory.h>

Inventory::Inventory(unsigned int w, unsigned int h) {
    width = w;
    height = h;

    contents = (Stack**)malloc(w * h * sizeof(Stack*));

    for (unsigned int i = 0; i < width * height; i++) {
        contents[i] = NULL;
    }
}

Stack* Inventory::get(unsigned int x, unsigned int y) {
    if (x >= width || y >= height) {
        return NULL;
    }

    return contents[x * height + y];
}

bool Inventory::set(unsigned int x, unsigned int y, Stack* val) {
    if (x >= width || y >= height) {
        return false;
    }

    contents[x * height + y] = val;

    return true;
}

bool Inventory::setNextFree(Stack* val) {
    for (unsigned int i = 0; i < width * height; i++) {
        if (contents[i] == NULL) {
            contents[i] = val;

            return true;
        }
    }

    return false;
}

bool Inventory::addItem(BlockType type) {
    for (unsigned int i = 0; i < width * height; i++) {
        Stack* s = contents[i];

        if (contents[i] == NULL) {
            continue;
        }
        
        if (contents[i]->getType() != type) {
            continue;
        }

        if (contents[i]->add()) {
            return true;
        }
    }

    if (setNextFree(new Stack(type, 1, 64))) {
        return true;
    }
    
    return false;
}

BlockType Inventory::use(unsigned int x, unsigned int y) {
    Stack *s = get(x, y);

    if (s == NULL) {
        return BlockType::air;
    }

    if (s->remove()) {
        if (s->getNum() == 0) {
            delete s;

            set(x, y, NULL);
        }

        return s->getType();
    } else {
        return BlockType::air;
    }
}