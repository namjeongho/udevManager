#include "udevManager.h"


int main() {
    UdevManager um;
    um.setup();
    printf("UdevManager setup ok\n");

    while(1) {
        um.readEvent();
    }
}

