#include "VExample.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
#include <cassert>

int main(int argc, char **argv, char **env) {
    Verilated::commandArgs(argc, argv);
    VerilatedVcdC *m_trace = new VerilatedVcdC;
    const std::unique_ptr<VerilatedContext> contextp{new VerilatedContext};
    contextp->traceEverOn(true);
    VExample *top = new VExample;
    top->trace(m_trace, 5);
    m_trace->open("logs/vlt_dump.vcd");
    int total_cycle = 100;
    while (!Verilated::gotFinish()) {
        for (int i = 0; i <= total_cycle; i++) {
            int a = rand() & 1;
            int b = rand() & 1;
            top->io_a = a;
            top->io_b = b;
            top->eval();
            m_trace->dump(i);
            printf("a = %d, b = %d, f = %d\n", a, b, top->io_f);
            assert(top->io_f == (a ^ b));
        }
        break;
    }
    m_trace->close();
    delete top;
    exit(0);
}
