# This program is free software: you can redistribute it and/or modify it under the terms of the 
# GNU General Public License as published by the Free Software Foundation, either version 3 of the 
# License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
# without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See 
# the GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License along with this program.  If 
# not, see <http://www.gnu.org/licenses/>.

LDFLAGS = \
				-L/usr/lib64 \
				-L/usr/local/lib \
				-dead_strip \
				-lboost_filesystem \
				-lboost_regex \
				-lboost_signals \
				-lboost_system \
				-lboost_thread \
				-lboost_serialization \
				$
LDTESTFLAGS = \
				-lboost_unit_test_framework \
				$

REAL_OBJECTS = \
				@torc/architecture/RealObjectFileList \
				@torc/common/RealObjectFileList \
				@torc/externals/zlib/RealObjectFileList \
				@torc/physical/RealObjectFileList \
				@torc/physical/xdl/RealObjectFileList \
				@torc/utils/RealObjectFileList \
				@openpr/anticore/RealObjectFileList \
				@openpr/bitstream/RealObjectFileList \
				@openpr/netlist/RealObjectFileList \
				$

TEST_OBJECTS = \
				@torc/architecture/TestObjectFileList \
				@torc/common/TestObjectFileList \
				@torc/physical/TestObjectFileList \
				@torc/physical/xdl/TestObjectFileList \
				@torc/utils/TestObjectFileList \
				$

DEVICES = $(wildcard torc/devices/*.db)




PHYSICAL_REFERENCES = \
				DesignUnitTest.reference.xdl \
				$


.PHONY: torc references databases openpr clean

# make everything

all: databases anticore

install: all
	install -D anticore /usr/local/bin/anticore
	install -d /usr/local/share/openpr
	cp -r torc/devices /usr/local/share/openpr/

anticore: torc databases openpr
	g++ \
				openpr/AntiCore.o \
				$(REAL_OBJECTS) \
				$(LDFLAGS) \
				-o anticore \
				$

# make the torc subdirectory
torc:
	make -C torc
openpr: 
	make -C openpr

# copy all of the regression test references into place
references: \
				regression \
				$

# make the regression directory
regression: 
	mkdir regression


# copy all of the family and device databases into place
databases: devices $(DEVICES:torc/%=%)

devices: 
	mkdir devices

devices/%.db: torc/devices/%.db
	ln -sf $< $@


# clean everything
clean:
	make -C torc clean
	make -C openpr clean
	rm -f $(OBJS)
	rm -f test XdlExample edif_example xdl_example xdldiff 
	rm -rf regression
	rm -rf devices
