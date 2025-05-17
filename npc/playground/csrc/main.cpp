#include "VNPC__Dpi.h"
#include <cassert>
#include <cstdio>
#include <sys/types.h>

#include "engine.hpp"
#include "common.h"

bool npc_exit = false;

void ebreak() {
    npc_exit = true;
    printf("ebreak!\n");
}

int main(int argc, char **argv, char **env) { 
    Engine engine; 
    engine.loop();
}
