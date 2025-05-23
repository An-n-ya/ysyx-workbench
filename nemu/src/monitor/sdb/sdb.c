/***************************************************************************************
 * Copyright (c) 2014-2024 Zihao Yu, Nanjing University
 *
 * NEMU is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *
 * See the Mulan PSL v2 for more details.
 ***************************************************************************************/

#include "sdb.h"
#include "common.h"
#include "debug.h"
#include "memory/paddr.h"
#include <cpu/cpu.h>
#include <isa.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();
void new_wp(const char* expr);
void print_watchpoints();
void delete_wp_by_id(int id);

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char *rl_gets() {
    static char *line_read = NULL;

    if (line_read) {
        free(line_read);
        line_read = NULL;
    }

    line_read = readline("(nemu) ");

    if (line_read && *line_read) {
        add_history(line_read);
    }

    return line_read;
}

static int cmd_info(char *args) {
    char *arg = strtok(NULL, " ");
    if (arg == NULL) {
        return 0;
    }
    if (strcmp(arg, "r") == 0) {
        isa_reg_display();
    } else if (strcmp(arg, "w") == 0) {
        print_watchpoints();
    }
    return 0;
}
static int cmd_c(char *args) {
    cpu_exec(-1);
    return 0;
}
static int cmd_si(char *args) {
    /* extract the first argument */
    char *arg = strtok(NULL, " ");
    if (arg == NULL) {
        cpu_exec(1);
        return 0;
    }
    char *endptr;
    long num = strtol(arg, &endptr, 10);
    if (*endptr != '\0') {
        fprintf(stderr,"Failed to convert str to integer.");
        return -1;
    }
    cpu_exec(num);
    return 0;
}

static int cmd_p(char *args) {
    bool success = false;
    word_t val = expr(args, &success);
    if (success) {
        printf("%u\n", val);
    } else {
        fprintf(stderr, "Failed to evaluate this expression: %s\n", args);
    }
    return 0;
}
static int cmd_x(char *args) {
    int number;
    char e[128];

    if (sscanf(args, "%d %s",&number, e) != 2) {
        fprintf(stderr, "输入格式错误，应为：<指令> <整数> <表达式>\n");
        return -1;
    }
    int address;
    bool is_success;
    address = expr(e, &is_success);
    if (!is_success) {
        fprintf(stderr, "failed to evaluate the expression: %s\n", e);
    }
    for (int i = 0; i < number; i++) {
        word_t w = paddr_read(address + 4 * i, 4);
        printf("0x%08x\n" ,w);
    }
    return 0;
}
static int cmd_w(char *args) { 
    new_wp(args);
    return 0; 
}
static int cmd_d(char *args) { 
    int number;
    if (sscanf(args, "%d",&number) != 1) {
        fprintf(stderr, "输入格式错误，应为：<指令> <整数id>\n");
        return -1;
    }
    delete_wp_by_id(number);
    return 0; 
}
static int cmd_q(char *args) { return -1; }

static int cmd_help(char *args);

static struct {
    const char *name;
    const char *description;
    int (*handler)(char *);
} cmd_table[] = {
    {"help", "Display information about all supported commands", cmd_help},
    {"c", "Continue the execution of the program", cmd_c},
    {"q", "Exit NEMU", cmd_q},
    {"s", "Step in", cmd_si},
    {"si", "Step in", cmd_si},
    {"info", "Display register/watchpoint info", cmd_info},
    {"x", "Print value stored in memmory", cmd_x},
    {"p", "Print expreesion", cmd_p},
    {"w", "Add watchpoint", cmd_w},
    {"d", "Delete watchpoint", cmd_d},

    /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)
static int cmd_help(char *args) {
    /* extract the first argument */
    char *arg = strtok(NULL, " ");
    int i;

    if (arg == NULL) {
        /* no argument given */
        for (i = 0; i < NR_CMD; i++) {
            printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        }
    } else {
        for (i = 0; i < NR_CMD; i++) {
            if (strcmp(arg, cmd_table[i].name) == 0) {
                printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
                return 0;
            }
        }
        printf("Unknown command '%s'\n", arg);
    }
    return 0;
}

void sdb_set_batch_mode() { is_batch_mode = true; }

void sdb_mainloop() {
    if (is_batch_mode) {
        cmd_c(NULL);
        return;
    }

    for (char *str; (str = rl_gets()) != NULL;) {
        char *str_end = str + strlen(str);

        /* extract the first token as the command */
        char *cmd = strtok(str, " ");
        if (cmd == NULL) {
            continue;
        }

        /* treat the remaining string as the arguments,
         * which may need further parsing
         */
        char *args = cmd + strlen(cmd) + 1;
        if (args >= str_end) {
            args = NULL;
        }

#ifdef CONFIG_DEVICE
        extern void sdl_clear_event_queue();
        sdl_clear_event_queue();
#endif

        int i;
        for (i = 0; i < NR_CMD; i++) {
            if (strcmp(cmd, cmd_table[i].name) == 0) {
                if (cmd_table[i].handler(args) < 0) {
                    return;
                }
                break;
            }
        }

        if (i == NR_CMD) {
            printf("Unknown command '%s'\n", cmd);
        }
    }
}

void init_sdb() {
    /* Compile the regular expressions. */
    init_regex();

    /* Initialize the watchpoint pool. */
    init_wp_pool();
}
