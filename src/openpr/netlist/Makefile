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

THIS_DIR = openpr/netlist
SRC_DIR = ../..
TORC_DIR = $(SRC_DIR)/torc
BOOST_INCLUDE_DIR = /usr/local/include/boost-1_42

# C++ flags -- enable or disable -O3 as needed for sanity
CCFLAGS = \
				-x c++ \
				-g \
				-O3 \
				-Wall \
				-I$(SRC_DIR) \
				-I$(TORC_DIR) \
				-I$(BOOST_INCLUDE_DIR) \
				$

# object files
OBJS = \
				InPin.o \
				Net.o \
				NetList.o \
				OutPin.o \
				Pin.o \
				Pip.o \
				Point.o \
				$

# dependencies
DEPS = $(OBJS:.o=.d)

# generate object file lists for regular and test code
REAL_OBJS = $(filter-out %UnitTest.o %RegressionTest.o, $(OBJS))
TEST_OBJS = $(filter %UnitTest.o %RegressionTest.o, $(OBJS))

# not all targets are real
.PHONY: all code clean

# make everything
all: code

# Include automatically generated dependencies or silently make them.
-include $(DEPS)

# build the code
code: $(OBJS) RealObjectFileList TestObjectFileList

# build the real object list
RealObjectFileList: $(OBJS)
	@echo $(REAL_OBJS:%=$(THIS_DIR)/%) > RealObjectFileList

# build the test object list
TestObjectFileList: $(OBJS)
	@echo $(TEST_OBJS:%=$(THIS_DIR)/%) > TestObjectFileList

# compile c++ sources 
%.o: %.cpp
	gcc $(CCFLAGS) -c -o $@ $<

# build dependency files
%.d: %.cpp
	@set -e; rm -f $@; \
	gcc -MM $(CCFLAGS) $< |  \
	sed -e '/boost/d' -e 's,\($*\)\.o[ :]*,\1.o $@ : ,g' > $@;

# clean everything
clean:
	rm -f $(OBJS) $(DEPS)
	rm -f ObjectFileList RealObjectFileList TestObjectFileList
