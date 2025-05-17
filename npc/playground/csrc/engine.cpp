#include "engine.hpp"
#include "VNPC.h"
#include "VNPC__Dpi.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <getopt.h>

extern bool npc_exit;

void set_batch_mode();

void Engine::parse_args(int argc, char *argv[]) {
    Verilated::commandArgs(argc, argv);
    const struct option table[] = {
        {"batch", no_argument, NULL, 'b'},      {"log", required_argument, NULL, 'l'},
        {"port", required_argument, NULL, 'p'}, {"symbol", required_argument, NULL, 's'},
        {"help", no_argument, NULL, 'h'},       {0, 0, NULL, 0},
    };
    int o;
    while ((o = getopt_long(argc, argv, "-bhl:p:s:", table, NULL)) != -1) {
        switch (o) {
        case 'b':
            is_batch_mode = true;
            break;
        case 'p':
            sscanf(optarg, "%d", &difftest_port);
            break;
        case 's':
            symbol_file = optarg;
            break;
        case 'l':
            log_file = optarg;
            break;
        case 1:
            img_file = optarg;
        default:
            printf("Usage: %s [OPTION...] IMAGE [args]\n\n", argv[0]);
            printf("\t-b,--batch              run with batch mode\n");
            printf("\t-l,--log=FILE           output log to FILE\n");
            printf("\t-s,--symbol=FILE        specify symbol file\n");
            printf("\t-p,--port=PORT          run DiffTest with port PORT\n");
            printf("\n");
            exit(0);
        }
    }
}

void Engine::init_pmem() {
    PMEM[0] = 0x01008093; // addi x1, x1, 16
    PMEM[1] = 0x01410113; // addi x2, x2, 20
    PMEM[2] = 0xffc18193; // addi x3, x3, -4
    PMEM[3] = 0xffc10513; // addi x10, x2, -4
    PMEM[4] = 0x00100073; // ebreak
}
u_int32_t Engine::pmem_read(u_int32_t pc) {
    if (pc % 4 != 0) {
        fprintf(stderr, "pc is not align to 4, got pc: %x", pc);
        exit(-1);
    }
    return PMEM[pc / 4];
}

void Engine::setup_trace() {
    m_trace = new VerilatedVcdC;
    contextp->traceEverOn(true);
    top->trace(m_trace, 5);
    m_trace->open("logs/vlt_dump.vcd");
}

Engine::Engine() {
    top = new VNPC;
    init_pmem();
    setup_trace();
    cycle_cnt = 0;
}
Engine::~Engine() {
    m_trace->close();
}

void Engine::loop() {
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
        if (cycle_cnt > 6) {
            return;
        }
        /*printf("a = %d, b = %d, f = %d\n", a, b, top->io_f);*/
        /*assert(top->io_f == (a ^ b));*/
    }
}

