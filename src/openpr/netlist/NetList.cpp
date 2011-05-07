/*
 * NetList.cpp
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

#ifndef NETLIST_HPP_
#define NETLIST_HPP_
#include "openpr/netlist/NetList.h"
namespace openpr {
namespace netlist {


NetList::NetList() {
	// TODO Auto-generated constructor stub
}

NetList::~NetList() {
	// TODO Auto-generated destructor stub
	for (uint i = 0; i < this->netList.size(); i++) {
		delete this->netList[i];
	}
	delete this->outputXDL;
}

NetList::NetList(std::string inputFile, torc::architecture::DDB* mDB) {
	this->mDB = mDB;
	std::ifstream fin(inputFile.c_str());
	if (!fin) {
		printf("Cant open file %s", inputFile.c_str());
	}
	this->outputXDL = NULL;
	this->topLevelParser(&fin);

}

NetList::NetList(std::string inputFile, std::string outputFile,
		torc::architecture::DDB* mDB) {
	this->mDB = mDB;
	std::ifstream fin(inputFile.c_str());
	if (!fin) {
		printf("Cant open file %s", inputFile.c_str());
	}
	this->outputXDL = new std::ofstream(outputFile.c_str());
	this->topLevelParser(&fin);
}

void NetList::topLevelParser(std::ifstream* fin) {
	char line[2048];
	std::string temp;
	std::string pound = "#";
	std::string net = "net";
	std::string module = "module";
	std::string endmodule = "endmodule";
	std::string inst = "inst";
	std::string semicolon = ";";
	std::string cfg = "cfg";
	std::string outpin = "outpin";
	std::string inpin = "inpin";
	std::string pip = "pip";
	std::string design = "design";
	if (this->outputXDL == NULL) {
		this->outputXDL = new std::ofstream();
		outputXDL->open("output.xdl");
	}
	while (fin->getline(line, 1024)) {
		temp = line;
		//	StringTokenizer strtok = StringTokenizer(temp, " ");

		boost::char_separator<char> sep(" ");
		tokenizer tokens(temp, sep);
		tokenizer::iterator tok_iter = tokens.begin();
		std::string tempStr;
		if (tok_iter != tokens.end()) {
			tempStr = *tok_iter;
		} else {
			tempStr = "";

		}

		if (pound == tempStr) {
			//comment
			//do nothing
		} else if (tempStr == design) {
			//inside design everything needs to be outputed
			int finished = 0;
			*outputXDL << temp << "\n";
			while (!finished) {
				getline(*fin, temp);
				//StringTokenizer strtokLocal = StringTokenizer(temp, " ");
				tokenizer tokens2(temp, sep);
				tokenizer::iterator tok_iter = tokens2.begin();
				if (tok_iter == tokens2.end()) {
					tempStr = "";
				} else {
					tempStr = *tok_iter;
				}
				while (tok_iter != tokens2.end()) {
					tempStr = *tok_iter;
					if (tempStr == semicolon) {
						finished = 1;
					}
					tok_iter++;
				}
				*outputXDL << "\t" << temp << "\n";
			}
		} else if (tempStr == module) {
			//inside a module

			//inside design everything needs to be outputed
			int finished = 0;
			*outputXDL << temp << "\n";
			while (!finished) {
				getline(*fin, temp);
				//StringTokenizer strtokLocal = StringTokenizer(temp, " ");
				tokenizer tokens2(temp, sep);
				tokenizer::iterator tok_iter = tokens2.begin();
				tempStr = *tok_iter;
				while (tok_iter != tokens2.end()) {
					tempStr = *tok_iter;
					if (tempStr == endmodule) {
						finished = 1;
					}
					tok_iter++;
				}
				*outputXDL << "\t" << temp << "\n";
			}
		} else if (tempStr == inst) {

			//inside design everything needs to be outputed
			int finished = 0;
			*outputXDL << temp << "\n";
			while (!finished) {
				getline(*fin, temp);
				//StringTokenizer strtokLocal = StringTokenizer(temp, " ");
				tokenizer tokens2(temp, sep);
				tokenizer::iterator tok_iter = tokens2.begin();
				tempStr = *tok_iter;
				if (tok_iter != tokens2.end()) {
					tempStr = *tok_iter;
					if (tempStr == semicolon) {
						finished = 1;
					}
					//					tok_iter++;
				}
				*outputXDL << "\t" << temp << "\n";
			}
		} else if (tempStr == net) {
			Net* tempNet = new Net(&temp);
			netList.push_back(tempNet);

			while (tempStr != semicolon) {
				getline(*fin, temp);
				//StringTokenizer strtokLocal = StringTokenizer(temp, " ");
				tokenizer tokens2(temp, sep);
				tokenizer::iterator tok_iter2 = tokens2.begin();
				if (tok_iter2 == tokens2.end()) {
					tempStr = "";
				} else {
					tempStr = *tok_iter2;
				}

				if (pound == tempStr) {
					//comment
					//do nothing
				} else if (outpin == tempStr) {
					std::string name;
					std::string position;
					tok_iter2++;
					name = *tok_iter2;
					tok_iter2++;
					if (tok_iter2 == tokens2.end()) {
						getline(*fin, temp);
						tokenizer tokens3(temp, sep);
						tokenizer::iterator tok_iter3 = tokens3.begin();
						position = *tok_iter3;
					} else {
						position = *tok_iter2;
					}
					OutPin* tempOutPin = new OutPin(name, position);
					tempNet->addOutPin(tempOutPin);
				} else if (inpin == tempStr) {
					std::string name;
					std::string position;
					tok_iter2++;
					name = *tok_iter2;
					tok_iter2++;
					if (tok_iter2 == tokens2.end()) {
						getline(*fin, temp);
						tokenizer tokens3(temp, sep);
						tokenizer::iterator tok_iter3 = tokens3.begin();
						position = *tok_iter3;
					} else {
						position = *tok_iter2;
					}
					InPin* tempInPin = new InPin(name, position);
					tempNet->addInPin(tempInPin);
				} else if (pip == tempStr) {
					Pip* tempPip = new Pip(temp, this->mDB);
					tempNet->insertPip(tempPip);
					this->pipToNet[tempPip] = tempNet;
					this->pointToPip[tempPip->getDestination()] = tempPip;
					this->pointToPip[tempPip->getSource()] = tempPip;
					this->segmentToNet[tempPip->getDestination()->getSegmentIndex()]
							= tempNet;
					this->segmentToNet[tempPip->getSource()->getSegmentIndex()]
							= tempNet;
				}
			}
			this->netToPip.insert(tempNet);
		}
	}
}
void NetList::printData() {
	this->printData(this->outputXDL);
}

void NetList::printData(std::ofstream* ofstream) {
	for (uint i = 0; i < this->netList.size(); i++) {
		this->netList[i]->printData(ofstream);
	}
}

Net* NetList::findNet(Net* input) {
	boost::unordered_set<Net*, hash_net, eq_net>::iterator locIterator;
	locIterator = this->netToPip.find(input);
	if (locIterator == this->netToPip.end()) {
		std::cerr << input->name << "\n";
		return NULL;
	} else {
		return *locIterator;
	}
}

Net* NetList::findNet(std::string input) {
	Net tempNet(&input);
	return this->findNet(&tempNet);
}

Net* NetList::findNet(Pin* input) {
	boost::unordered_map<Pin*, Net*, hash_pip, eq_pip>::iterator netIterator;
	netIterator = this->pipToNet.find(input);
	if (netIterator == this->pipToNet.end()) {
		return NULL;
	} else {
		return netIterator->second;
	}
}
Pin* NetList::findPin(Pin* input) {
	boost::unordered_map<Pin*, Net*, hash_pip, eq_pip>::iterator netIterator;
	netIterator = this->pipToNet.find(input);
	if (netIterator == this->pipToNet.end()) {
		return NULL;
	} else {
		return netIterator->first;
	}
}

int NetList::insertNet(Net* inputNet) {
	if (inputNet == NULL)
		return -1;
	netList.push_back(inputNet);
	return 0;
}
int NetList::insertPip(Net* inputNet, Pip* inputPip) {
	/*
	 * RETURN CODES
	 * 0=success
	 * 1=conflict on source
	 * 2=conflict on destination
	 * 3=unable to find net
	 */

	Point* source = inputPip->getSource();
	Point* destination = inputPip->getDestination();
	boost::unordered_map<Point*, Pip*, hash_point, eq_point>::iterator
			pointIterator;
	boost::unordered_map<torc::architecture::Segments::CompactSegmentIndex, Net*, hash_segment, eq_segment>::iterator
			segmentIterator;

	boost::unordered_set<Net*, hash_net, eq_net>::iterator netIterator;
	netIterator = this->netToPip.find(inputNet);
	if (netIterator == this->netToPip.end()) {
		//		return 3;
	}

	Net* tempNet = *netIterator;
	tempNet->insertPip(inputPip);
	this->pipToNet[inputPip] = *netIterator;
	this->pointToPip[source] = inputPip;
	this->pointToPip[destination] = inputPip;
	return 0;
}
int NetList::removePip(Pip* inputPip) {
	//find pip
	// remove pip from pip/net correlation
	//find points
	//remove points
	//remove pip
	//delete pip

	/*
	 * return codes
	 * 0 success
	 * 1 can't find pip
	 * 2 cast failure on pip
	 */
	boost::unordered_map<Pin*, Net*, hash_pip, eq_pip>::iterator netIterator;
	netIterator = this->pipToNet.find(inputPip);
	if (netIterator == this->pipToNet.end()) {
		return 1;
	}
	Pin* tempPin = netIterator->first;
	Pip* tempPip;
	Net* tempNet = netIterator->second;
	try {
		tempPip = (dynamic_cast<Pip*> (tempPin));
	} catch (std::bad_cast) {
		return 2;
	}
	Point* tempSource = tempPip->getSource();
	Point* tempDestination = tempPip->getDestination();
	this->pointToPip.erase(tempSource);
	this->pointToPip.erase(tempDestination);
	this->pipToNet.erase(tempPip);
	tempNet->remotePin(tempPin, 1);
	delete tempPip;
	return 0;
}
std::vector<Net*>* NetList::getNetList() {
	return &this->netList;
}
boost::unordered_map<Pin*, Net*, hash_pip, eq_pip>* NetList::getPipToNet() {
	return &this->pipToNet;
}
}
}

#endif /* NETLIST_HPP_ */
