#include "VNPC__Dpi.h"
#include <cassert>
#include <cstdio>
#include <sys/types.h>

#include "common.h"
#include "engine.hpp"

bool npc_exit = false;
int break_code = 0;

void ebreak(int code) {
    npc_exit = true;
    break_code = code;
    printf("ebreak! %d\n", code);
}

int is_exit_status_bad() {
    int good = (break_code == 0);
    return !good;
}

int main(int argc, char **argv, char **env) {
    Engine engine(argc, argv);
    engine.loop();
    return is_exit_status_bad();
}
