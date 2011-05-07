/*
 * Net.h
 *
 *  Created on: Feb 5, 2010
 *      Author: jacouch
 */
// This program is free software: you can redistribute it and/or modify it under the terms of the
// GNU General Public License as published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
// the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program.  If
// not, see <http://www.gnu.org/licenses/>.
#ifndef NET_H_
#define NET_H_
#include <boost/unordered_set.hpp>
#include "openpr/netlist/Pip.h"
#include "openpr/netlist/InPin.h"
#include "openpr/netlist/OutPin.h"
#include "openpr/netlist/HashStructs.h"
namespace openpr {
namespace netlist {
class Net {
public:
	Net();
	~Net();
	Net(std::string* inputString);
	Net(const Net* other);
	void addCfg(std::string cfg);
	void addInPin(InPin* inpin);
	void addOutPin(Pin* outpin);
	void insertPip(Pip* pip);
	void remotePin(Pin* tempPin, int type);
	void clearInPins();
	void printData(std::ofstream* outstream);
	void printData(std::ostream* outstream);
	bool operator!=(const Net &other) const;
	bool operator==(const Net &other) const;
	bool operator<(const Net &other) const;
	bool operator>(const Net &other) const;
	Net& operator=(const Net &other);
	bool operator!=(const Net* other) const;
	bool operator==(const Net* other) const;
	bool operator<(const Net* other) const;
	bool operator>(const Net* other) const;
	Net& operator=(const Net* other);
	void mergePips(const Net& other);
	std::size_t operator()(const Net* &other) const;
	std::size_t operator()(const Net& other) const;
	std::size_t hash_value(const Net& net) const;
	std::size_t hash_value(const Net* net) const;
	std::string name;
private:
	boost::unordered_set<Pin*, hash_pip, eq_pip> pips;
	boost::unordered_set<Pin*, hash_pip, eq_pip> outPins;
	boost::unordered_set<Pin*, hash_pip, eq_pip> inPins;
	std::string outpin;
	std::string outpinLetter;
	std::string cfg;
	bool configure;

};
}
}
#endif /* NET_H_ */
