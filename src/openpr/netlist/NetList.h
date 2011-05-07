/*
 * NetList.h
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
#ifndef NETLIST_H_
#define NETLIST_H_
#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>
#include "torc/architecture/DDB.hpp"
#include "openpr/netlist/Net.h"
#include "openpr/netlist/NetHashStruct.h"
namespace openpr {
namespace netlist {
class NetList {
public:
	NetList();
	NetList(std::string inputFile, torc::architecture::DDB* mDB);
	NetList(std::string inputFile, std::string outputFile,
			torc::architecture::DDB* mDB);
	~NetList();
	void printData();
	void printData(std::ofstream* ofstream);
	std::ofstream* outputXDL;
	Net* findNet(Net* input);
	Net* findNet(std::string input);
	Net* findNet(Pin* input);
	Pin* findPin(Pin* input);
	Pip* findPip(Point* input);
	int insertNet(Net* inputNet);
	int insertPip(Net* inputNet, Pip* inputPip);
	int removePip(Pip* inputPip);
	std::vector<Net*>* getNetList();
	boost::unordered_map<Pin*, Net*, hash_pip, eq_pip>* getPipToNet();

private:
	void topLevelParser(std::ifstream* fStream);
	void netParser(std::ifstream* fin, std::string* netString);
	std::vector<Net*> netList;
	boost::unordered_set<Net*, hash_net, eq_net> netToPip;
	boost::unordered_map<Pin*, Net*, hash_pip, eq_pip> pipToNet;
	boost::unordered_map<Point*, Pip*, hash_point, eq_point> pointToPip;
	boost::unordered_map<torc::architecture::xilinx::CompactSegmentIndex, Net*, hash_segment, eq_segment> segmentToNet;
	torc::architecture::DDB* mDB;
};
}
}
#endif /* NETLIST_H_ */
