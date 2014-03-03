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

THIS_DIR = openpr
SRC_DIR = ..
TORC_DIR = $(SRC_DIR)/torc
BOOST_INCLUDE_DIR = /usr/local/include/boost-1_42

# C++ flags
CCFLAGS = \
				-x c++ \
				-g \
				-Wall \
				-Werror \
				-I$(SRC_DIR) \
				-I$(TORC_DIR) \
				-I$(BOOST_INCLUDE_DIR) \
				$

# object files
OBJS = \
				AntiCore.o \
				$

# dependencies
DEPS = $(OBJS:.o=.d)

# generate object file lists for regular and test code
REAL_OBJS = $(filter-out %UnitTest.o %RegressionTest.o, $(OBJS))
TEST_OBJS = $(filter %UnitTest.o %RegressionTest.o, $(OBJS))

# not all targets are real
.PHONY: all anticore bitstream netlist clean

# make everything
all: code

# build the code
code: bitstream netlist anticore $(OBJS) RealObjectFileList TestObjectFileList

# build the subdirectories

anticore:
	make -C anticore

bitstream:
	make -C bitstream

netlist:
	make -C netlist

# build the real object list
RealObjectFileList:
	@echo $(REAL_OBJS:%=$(THIS_DIR)/%) > RealObjectFileList

# build the test object list
TestObjectFileList:
	@echo $(TEST_OBJS:%=$(THIS_DIR)/%) > TestObjectFileList

# compile c++ sources 
%.o: %.cpp
	gcc $(CCFLAGS) -c -o $@ $<

# build dependency files
%.d: %.cpp
	@set -e; rm -f $@; \
	gcc -MM $(CCFLAGS) $< |  \
	sed -e '/boost/d' -e 's,\($*\)\.o[ :]*,\1.o $@ : ,g' > $@;

clean:
	make -C anticore clean
	make -C bitstream clean
	make -C netlist clean

	rm -f $(OBJS) $(DEPS)
	rm -f ObjectFileList RealObjectFileList TestObjectFileList

