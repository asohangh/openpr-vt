# ===========================================================================
# = Makefile configuration
# ===========================================================================
# Top-level module
TOP = top

# Verilog sources
VERILOG_SOURCES = $(wildcard src/*.v)
VHDL_SOURCES = $(wildcard src/*.vhd)

# Precompiled cores (ngd, ncd, edif, etc):
PRECOMPILED_DIR   = src/precompiled
PRECOMPILED_CORES = $(wildcard $(PRECOMPILED_DIR)/*) 

# Destination directory where you want the BIT file ( usually ./$(DEST_DIR))
DEST_DIR = ./build

# Constraint file
UCF = src/sandbox.ucf

# XST synthesis constraint file
XCF = src/sandbox.xcf

# Xilinx part #
PART = xc5vlx110t-ff1136-1

# Anticore executable
ANTICORE = anticore

# Torc path
TORC = $(shell which $(ANTICORE))

# ===========================================================================
# = Shortcuts
# ===========================================================================

default:    bit
script:     $(DEST_DIR)/ptScripted.opr
xst:        $(DEST_DIR)/$(TOP).ngc
pm:         $(DEST_DIR)/macrosPlaced.opr
translate:  $(DEST_DIR)/$(TOP).ngd
map:        $(DEST_DIR)/$(TOP)_map.ncd $(DEST_DIR)/$(TOP).pcf
place:      $(DEST_DIR)/$(TOP)_placed.xdl
block:      $(DEST_DIR)/$(TOP)_blocked.ncd
route:      $(DEST_DIR)/$(TOP)_routed.ncd
unblock:    $(DEST_DIR)/$(TOP)_full.ncd
bit:        $(DEST_DIR)/$(TOP)_full.bit
partial:    $(DEST_DIR)/$(TOP)_partial.bit

# ===========================================================================
# = Create $(DEST_DIR)/project.prj
# ===========================================================================
$(shell mkdir -p $(DEST_DIR))
$(shell >$(DEST_DIR)/project.prj)
$(foreach x,$(VHDL_SOURCES),$(shell echo vhdl work ../$(x) >>$(DEST_DIR)/project.prj))
$(foreach x,$(VERILOG_SOURCES),$(shell echo verilog work ../$(x) >>$(DEST_DIR)/project.prj))


$(DEST_DIR)/$(TOP).ngc : $(VERILOG_SOURCES) $(VHDL_SOURCES)
	# ===========================================================================
	# = XST (Synthesize)
	# ===========================================================================
	mkdir -p $(DEST_DIR)
	@echo set -xsthdpdir ./xst/                 >$(DEST_DIR)/xst.scr
	@echo set -checkcmdline no                  >>$(DEST_DIR)/xst.scr
	@echo run                                   >>$(DEST_DIR)/xst.scr
	@echo -ifn project.prj                      >>$(DEST_DIR)/xst.scr
	@echo -ifmt mixed                           >>$(DEST_DIR)/xst.scr
	@echo -ofn $(TOP)                           >>$(DEST_DIR)/xst.scr
	@echo -p $(PART)                            >>$(DEST_DIR)/xst.scr
	@echo -top $(TOP)                           >>$(DEST_DIR)/xst.scr
	@echo -uc ../$(XCF)                         >>$(DEST_DIR)/xst.scr
	@echo -keep_hierarchy yes                   >>$(DEST_DIR)/xst.scr
	@echo -sd /home/asohangh/v4/ppcSystem/implementation      >>$(DEST_DIR)/xst.scr
	@echo -write_timing_constraints yes         >>$(DEST_DIR)/xst.scr
	cd $(DEST_DIR) && xst -ifn xst.scr -ofn xst.log -intstyle xflow

$(DEST_DIR)/ptScripted.opr : top.proj
	mkdir -p $(DEST_DIR)
	cp $(TOP).proj $(DEST_DIR)/$(TOP).proj
	cd $(DEST_DIR) && $(ANTICORE) $(TORC) $(TOP).proj genPTScripts 1> genPTScripts.log

$(DEST_DIR)/macrosPlaced.opr : $(DEST_DIR)/ptScripted.opr
	mkdir -p $(DEST_DIR)
	cp $(TOP).proj $(DEST_DIR)/$(TOP).opr
	cd $(DEST_DIR) && $(ANTICORE) $(TORC) ptScripted.opr placeMacros 1> placeMacros.log

$(DEST_DIR)/$(TOP).ngd : $(DEST_DIR)/$(TOP).ngc $(UCF) $(PRECOMPILED_CORES) $(DEST_DIR)/macrosPlaced.opr
	# ===========================================================================
	# = ngdbuild (Translate)
	# ===========================================================================
	mkdir -p $(DEST_DIR)	
	-cp $(PRECOMPILED_DIR)/* $(DEST_DIR)/
	cp -p $(UCF) $(DEST_DIR)/$(TOP).ucf
	cd $(DEST_DIR) && ngdbuild -aul -p $(PART) -intstyle xflow $(TOP) $(TOP).ngd 1> translate.log
	
$(DEST_DIR)/$(TOP)_map.ncd $(DEST_DIR)/$(TOP).pcf : $(DEST_DIR)/$(TOP).ngd
	# ===========================================================================
	# = map
	# ===========================================================================
	mkdir -p $(DEST_DIR)
	cd $(DEST_DIR) && map -detail -w -o $(TOP)_map.ncd -intstyle xflow $(TOP).ngd $(TOP).pcf 1> map.log

$(DEST_DIR)/lockConstrained.opr: $(DEST_DIR)/$(TOP).pcf
	mkdir -p $(DEST_DIR)
	cd $(DEST_DIR) && $(ANTICORE) $(TORC) macrosPlaced.opr genLockConstraints 1> genLockConstraints.log

$(DEST_DIR)/$(TOP)_placed.xdl : $(DEST_DIR)/$(TOP)_map.ncd $(DEST_DIR)/$(TOP).pcf
	# ===========================================================================
	# = routing passthroughs
	# ===========================================================================
	mkdir -p $(DEST_DIR)
	cd $(DEST_DIR) && fpga_edline -p ../src/route_passthroughs.scr $(TOP)_map.ncd && cp $(TOP)_map.ncd $(TOP)_placed.ncd 1> route_passthroughs.log
	cd $(DEST_DIR) && xdl -ncd2xdl $(TOP)_placed.ncd $(TOP)_placed.xdl 1>> route_passthroughs.log

$(DEST_DIR)/$(TOP)_mergedClockTree.opr $(DEST_DIR)/$(TOP)_merged.xdl : $(DEST_DIR)/$(TOP)_placed.xdl $(DEST_DIR)/lockConstrained.opr
	# ===========================================================================
	# = merge static clocktree into partial design
	# ===========================================================================
	mkdir -p $(DEST_DIR)
	cd $(DEST_DIR) && $(ANTICORE) $(TORC) lockConstrained.opr mergeClockTree 1> mergeClockTree.log
	cd $(DEST_DIR) && xdl -force -xdl2ncd $(TOP)_merged.xdl $(TOP)_merged.ncd && xdl -ncd2xdl $(TOP)_merged.ncd 1>> mergeClockTree.log

$(DEST_DIR)/$(TOP)_blocked.ncd : $(DEST_DIR)/$(TOP)_merged.xdl $(DEST_DIR)/lockConstrained.opr
	# ===========================================================================
	# = Adding blocking routes 
	# ===========================================================================
	mkdir -p $(DEST_DIR)
	cd $(DEST_DIR) && $(ANTICORE) $(TORC) lockConstrained.opr routeBlocker 1> routeBlocker.log
	cd $(DEST_DIR) && xdl -force -xdl2ncd $(TOP)_blocked.xdl $(TOP)_blocked.ncd 1>> routeBlocker.log

$(DEST_DIR)/$(TOP)_routed.ncd : $(DEST_DIR)/$(TOP)_blocked.ncd $(DEST_DIR)/$(TOP).pcf
	# ===========================================================================
	# = Route design with fpga_editor
	# ===========================================================================
	mkdir -p $(DEST_DIR)
	cd $(DEST_DIR) && fpga_edline -p ../src/route_all.scr $(TOP)_blocked.ncd $(TOP).pcf 1> router.log
	cd $(DEST_DIR) && cp $(TOP)_blocked.ncd $(TOP)_routed.ncd

$(DEST_DIR)/$(TOP)_full.ncd : $(DEST_DIR)/$(TOP)_routed.ncd $(DEST_DIR)/$(TOP).pcf
	# ===========================================================================
	# = unrouting passthroughs
	# ===========================================================================
	mkdir -p $(DEST_DIR)
	cd $(DEST_DIR) && fpga_edline -p ../src/unroute_passthroughs.scr $(TOP)_routed.ncd && cp $(TOP)_routed.ncd $(TOP)_full.ncd 1> unroute_passthroughs.log
	cd $(DEST_DIR) && xdl -ncd2xdl $(TOP)_full.ncd $(TOP)_full.xdl 1>> unroute_passthroughs.log

$(DEST_DIR)/$(TOP)_full.bit : $(DEST_DIR)/$(TOP)_full.ncd
	# ===========================================================================
	# = bitgen (DRC disabled)
	# ===========================================================================
	mkdir -p $(DEST_DIR)
	cd $(DEST_DIR) && bitgen $(TOP)_full.ncd -d -w -intstyle xflow $(TOP)_full.bit

$(DEST_DIR)/$(TOP)_partial.bit : $(DEST_DIR)/$(TOP)_full.bit $(DEST_DIR)/routeBlocked.opr
	# ===========================================================================
	# = Generate partial bitstream
	# ===========================================================================
	mkdir -p $(DEST_DIR)
	cd $(DEST_DIR) && $(ANTICORE) $(TORC) routeBlocked.opr genPartialBitstream 1> genPartialBitstream.log

clean:
	# ===========================================================================
	# = Remove intermediate and output files
	# ===========================================================================
	rm -rf $(DEST_DIR)
	rm -rf $(PRECOMPILED_DIR)/busmacro_xc5v_async_vert_input_*.nmc
	rm -rf $(PRECOMPILED_DIR)/busmacro_xc5v_async_vert_output_*.nmc
