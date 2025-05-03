#include <common.h>
#include <stdio.h>
#include "utils.h"
#ifdef CONFIG_IRINGBUF
void print_iringbuf() {
    for (int i = 0; i < IRINGBUF_SIZE; i++) {
        if (nemu_state.iring_ind == 0 || nemu_state.iringbuf[i][0] == '\0') {
            return;
        }
        if (i == nemu_state.iring_ind - 1) {
            fprintf(stderr, "--> ");
        } else {
            fprintf(stderr, "    ");
        }
        fprintf(stderr, "%s\n", nemu_state.iringbuf[i]);
    }
}
#endif
