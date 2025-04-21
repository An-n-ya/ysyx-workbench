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

#include "debug.h"
#include "sdb.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <isa.h>

#define NR_WP 32
#define EXPR_LEN 256

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  char str[EXPR_LEN];
  bool is_free;

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].is_free = true;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

void cleal_wp(WP* wp) {
    int i;
    for (i = 0; i < EXPR_LEN; i++) {
        wp->str[i] = 0;
    }
    wp->is_free = true;
}

void new_wp(const char* expr) {
    assert(free_ != NULL);
    WP* wp = free_;
    free_ = wp->next;
    wp->next = head;
    wp->is_free = false;
    strcpy(wp->str, expr);
    head = wp;
}


void delete_wp(WP* wp) {
    cleal_wp(wp);
    if (wp == head) {
        head = wp->next;
    } else {
        // find parent
        WP* cur = head;
        bool found = false;
        while (cur != NULL) {
            if (cur->next == wp) {
                // found parent here
                cur->next = wp->next;
                found = true;
            }
        }
        if (!found) {
            fprintf(stderr, "Cannot find parent of this watchpoint.\n");
        }
    }
    wp->next = free_;
    free_ = wp;
}
void delete_wp_by_id(int id) {
    WP* wp = &wp_pool[id];
    if (wp->is_free) {
        return;
    }
    delete_wp(wp);
}

void print_watchpoints() {
    WP* cur = head;
    printf("id\texpr\n");
    while (cur != NULL) {
        printf("%d\t%s\n", cur->NO, cur->str);
        cur = cur->next;
    }
}

bool need_stop() {
    WP* cur = head;
    while (cur != NULL) {
        bool success = false;
        if (expr(cur->str, &success) != 0) {
            printf("stopped at watchpoint %d, at: %s\n", cur->NO, cur->str);
            return true;
        }
        cur = cur->next;
    }
    return false;
}


