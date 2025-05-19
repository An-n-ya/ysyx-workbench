#include "VNPC__Dpi.h"
#include <cassert>
#include <cstdio>
#include <sys/types.h>

#include "common.h"
#include "engine.hpp"

bool npc_exit = false;

void ebreak() {
    npc_exit = true;
    printf("ebreak!\n");
}

int main(int argc, char **argv, char **env) {
    Engine engine(argc, argv);
    engine.loop();
}
