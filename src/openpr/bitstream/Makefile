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

THIS_DIR = openpr/bitstream
SRC_DIR = ../..
BOOST_INCLUDE_DIR = /usr/local/include/boost-1_42

CCFLAGS = \
				-x c++ \
				-g \
				-Wall \
				-I$(SRC_DIR) \
				-I$(BOOST_INCLUDE_DIR) \
				$

OBJS = \
				device.o \
				v4_bitstream.o \
				v4_devices.o \
				v5_bitstream.o \
				v5_devices.o \
				virtex4.o \
				virtex5.o \
				bitstream.o \
				$

REAL_OBJS = $(filter-out %UnitTest.o %RegressionTest.o, $(OBJS))
TEST_OBJS = $(filter %UnitTest.o %RegressionTest.o, $(OBJS))

.PHONY: all code clean

all: code

code: $(OBJS) RealObjectFileList TestObjectFileList

RealObjectFileList:
	@echo $(REAL_OBJS:%=$(THIS_DIR)/%) > RealObjectFileList

TestObjectFileList:
	@echo $(TEST_OBJS:%=$(THIS_DIR)/%) > TestObjectFileList

# compile c++ sources 
%.o: %.cpp
	gcc $(CCFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS)
	rm -f ObjectFileList RealObjectFileList TestObjectFileList

