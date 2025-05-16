#include "VNPC.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
#include "VNPC__Dpi.h"
#include <cassert>
#include <cstdio>
#include <sys/types.h>

u_int32_t PMEM[4096];
bool npc_exit = false;

void ebreak() {
    npc_exit = true;
    printf("ebreak!\n");
}

void init_pmem() { 
    PMEM[0] = 0x01008093; // addi x1, x1, 16
    PMEM[1] = 0x01410113; // addi x2, x2, 20 
    PMEM[2] = 0xffc18193; // addi x3, x3, -4
    PMEM[3] = 0x00100073; // ebreak
}

u_int32_t pmem_read(u_int32_t pc) {
    if (pc % 4 != 0) {
        fprintf(stderr, "pc is not align to 4, got pc: %x", pc);
        exit(-1);
    }
    return PMEM[pc / 4];
}

int main(int argc, char **argv, char **env) {
    Verilated::commandArgs(argc, argv);
    VerilatedVcdC *m_trace = new VerilatedVcdC;
    const std::unique_ptr<VerilatedContext> contextp{new VerilatedContext};
    contextp->traceEverOn(true);
    VNPC *top = new VNPC;
    top->trace(m_trace, 5);
    m_trace->open("logs/vlt_dump.vcd");
    init_pmem();
    int cycle_cnt = 0;
    while (!npc_exit && !Verilated::gotFinish()) {
        contextp->timeInc(1);
        printf("cycle %d begin\n", cycle_cnt);
        top->clock = 0;
        top->io_inst = pmem_read(top->io_pc);
        top->eval();

        contextp->timeInc(1);
        top->clock = 1;
        top->io_inst = pmem_read(top->io_pc);
        top->eval();
        printf("cycle %d finished\n", cycle_cnt);
        printf("==================\n");
        m_trace->dump(cycle_cnt);
        cycle_cnt++;
        /*printf("a = %d, b = %d, f = %d\n", a, b, top->io_f);*/
        /*assert(top->io_f == (a ^ b));*/
    }
    m_trace->close();
    delete top;
    exit(0);
}
