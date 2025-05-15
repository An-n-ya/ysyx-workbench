BUILD_DIR = ./build

PRJ = playground

VERI_PRJ = $(PRJ)/csrc
VERI_DIR = ./verilator
VERI_TARGET = NPC

test:
	mill -i $(PRJ).test

verilator: verilog
	mkdir -p $(VERI_PRJ)
	mkdir -p $(VERI_DIR)
	verilator  -cc $(BUILD_DIR)/$(VERI_TARGET).sv --exe $(VERI_PRJ)/main.cpp --trace --Mdir $(VERI_DIR)
	make -j -C $(VERI_DIR) -f V$(VERI_TARGET).mk V$(VERI_TARGET)
	rm -rf logs
	mkdir -p logs
	$(VERI_DIR)/V$(VERI_TARGET)

verilog:
	$(call git_commit, "generate verilog")
	mkdir -p $(BUILD_DIR)
	mill -i $(PRJ).runMain Elaborate --target-dir $(BUILD_DIR)

help:
	mill -i $(PRJ).runMain Elaborate --help

reformat:
	mill -i __.reformat

checkformat:
	mill -i __.checkFormat

bsp:
	mill -i mill.bsp.BSP/install

idea:
	mill -i mill.idea.GenIdea/idea

clean:
	-rm -rf $(BUILD_DIR)
	-rm -rf $(VERI_DIR)

.PHONY: test verilog help reformat checkformat clean

sim:
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	@echo "Write this Makefile by yourself."

-include ../Makefile
