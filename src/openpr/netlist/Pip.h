/*
 * Pip.h
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
#ifndef PIP_H_
#define PIP_H_
#include "openpr/netlist/Pin.h"
#include "openpr/netlist/Point.h"
namespace openpr {
namespace netlist {
class Pip: public Pin {
public:
	Pip();
	Pip(std::string tok, torc::architecture::DDB* mdb);
	Pip(std::string type, int x, int y, std::string source,
			std::string destination, torc::architecture::DDB* mdb);
	Pip(std::string location, std::string source, std::string destination,
			torc::architecture::DDB* mdb);
	virtual ~Pip();
	void printData(std::ofstream* outStream);
	void printData(std::ostream* outStream);
	bool operator!=(Pin &other);
	bool operator!=(Pin* other);
	bool operator==(const Pin &other) const;
	bool operator==(const Pin* other) const;
	bool operator<(Pin &other);
	bool operator<(Pin* other);
	bool operator>(Pin &other);
	bool operator>(Pin* other);
	Pin& operator=(Pin &other);
	Pin* operator=(Pin* other);
	size_t operator()(const Pin& x) const;
	size_t operator()(const Pin* x) const;
	size_t operator()(const Pip& x) const;
	size_t operator()(const Pip* x) const;
	Point* getSource() const;
	Point* getDestination() const;
	std::string getTileStr();
	std::string getSourceStr();
	std::string getSinkStr();

private:
	std::string wholeData;
	std::string location;
	std::string type;
	Point* source;
	Point* destination;
	int xLoc;
	int yLoc;
	void parseLocation(std::string location);
	void generateFullStream();
	std::string generateLocation() const;
	torc::architecture::DDB* mDB;
};
}
}

#endif /* PIP_H_ */
