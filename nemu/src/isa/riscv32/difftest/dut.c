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

#include "../local-include/reg.h"
#include <cpu/difftest.h>
#include <isa.h>

void ref_reg_display(CPU_state *ref_r) {
    printf("===================ref register===================\n");
    for (int i = 0; i < 32; i += 4) {
        printf("%s: 0x%08x\t%s: 0x%08x\t%s: 0x%08x\t%s: 0x%08x\t\n", reg_name(i), ref_r->gpr[i],
               reg_name(i + 1), ref_r->gpr[i + 1], reg_name(i + 2), ref_r->gpr[i + 2], reg_name(i + 3),
               ref_r->gpr[i + 3]);
    }
    printf("\n===================dut register===================\n");
}
bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) { 
    for (int i = 0; i < 32; i++) {
        if (ref_r->gpr[i] != gpr(i)) {
            printf("diff reg on : %s at pc: 0x%08x, expect: 0x%08x, got: 0x%08x\n", reg_name(i), pc, ref_r->gpr[i], gpr(i));
            ref_reg_display(ref_r);
            return false;
        }
    }
    return true; 
}

void isa_difftest_attach() {}
