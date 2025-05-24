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
            printf("symbol file: %s\n", symbol_file);
            break;
        case 'l':
            log_file = optarg;
            printf("log file: %s\n", log_file);
            break;
        case 1:
            img_file = optarg;
            printf("loaded img file: %s\n", img_file);
            break;
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

void Engine::setup_trace() {
    m_trace = new VerilatedVcdC;
    contextp->traceEverOn(true);
    top->trace(m_trace, 5);
    m_trace->open("logs/vlt_dump.vcd");
}

Engine::Engine(int argc, char **argv) {
    top = new VNPC;
    parse_args(argc, argv);
    mem.init_pmem(img_file);
    setup_trace();
    cycle_cnt = 0;
}
Engine::~Engine() { m_trace->close(); }

void Engine::reset() {
    top->clock = 0;
    top->reset = 1;
    top->eval();
    top->clock = 1;
    top->reset = 1;
    top->eval();
    top->clock = 1;
    top->reset = 0;
    top->eval();
    printf("reset complete\n");
}

void Engine::loop() {
    reset();
    while (!npc_exit && !Verilated::gotFinish()) {
        contextp->timeInc(1);
        printf("cycle %d begin\n", cycle_cnt);
        top->clock = 0;
        top->io_inst = pmem_read(top->io_pc);
        top->eval();

        contextp->timeInc(1);
        top->clock = 1;
        top->io_inst = pmem_read(top->io_pc);
        printf("[npc] pc: %08x ", top->io_pc);
        printf("[npc] inst: %08x\n", top->io_inst);
        disasm.disasm(top->io_pc, (uint8_t *)&(top->io_inst));
        printf("[npc] disasm: %s\n", disasm.out());
        top->eval();
        printf("cycle %d finished\n", cycle_cnt);
        printf("==================\n");
        m_trace->dump(cycle_cnt);
        cycle_cnt++;
        /*if (cycle_cnt > 6) {*/
        /*    return;*/
        /*}*/
        /*printf("a = %d, b = %d, f = %d\n", a, b, top->io_f);*/
        /*assert(top->io_f == (a ^ b));*/
    }
}
