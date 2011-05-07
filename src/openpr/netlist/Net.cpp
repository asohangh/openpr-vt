/*
 * Net.cpp
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

#ifndef NET_HPP_
#define NET_HPP_
#include "openpr/netlist/Net.h"
namespace openpr {
namespace netlist {
Net::Net() {
	// TODO Auto-generated constructor stub

}
Net::~Net() {
	// TODO Auto-generated destructor stub
	boost::unordered_set<Pin*, hash_pip, eq_pip>::iterator pipIterator =
			this->pips.begin();
	boost::unordered_set<Pin*, hash_pip, eq_pip>::iterator outIterator =
			this->outPins.begin();
	boost::unordered_set<Pin*, hash_pip, eq_pip>::iterator inIterator =
			this->inPins.begin();

}
Net::Net(std::string* input) {
	using namespace std;
	boost::char_separator<char> sep(" ,");
	tokenizer tokens(*input, sep);
	tokenizer::iterator tok_iter = tokens.begin();
	std::string config = "cfg";
	this->name = *tok_iter;
	tok_iter++;
	this->configure = false;
	if (tok_iter != tokens.end()) {
		this->name = *tok_iter;
		tok_iter++;
	}
	if (tok_iter != tokens.end()) {
		this->cfg = *tok_iter;
		tok_iter++;
	}
	if (this->cfg == config) {
		this->configure = true;
		boost::char_separator<char> sep2("\"");
		tokenizer tokens2(*input, sep2);
		tokenizer::iterator tok_iter2 = tokens2.begin();
		tok_iter2++;
		tok_iter2++;
		tok_iter2++;
		this->cfg = *tok_iter2;
	}
}
Net::Net(const Net* other) {
	*this = *other;
}

void Net::addCfg(std::string cfg) {
	this->cfg = cfg;
	configure = true;

}
void Net::addInPin(InPin* inpin) {
	this->inPins.insert(inpin);
}
void Net::addOutPin(Pin* outpin) {
	this->outPins.insert(outpin);
}
void Net::insertPip(Pip* pip) {
	this->pips.insert(pip);
}
void Net::printData(std::ofstream* outStream) {

	if (!this->cfg.empty() && this->configure == true) {
		*outStream << "net " << this->name << " , ";
		*outStream << "cfg \" " << this->cfg << "\",";
	} else {
		*outStream << "net " << this->name << " " << this->cfg << ",";
	}
	*outStream << " \n";
	boost::unordered_set<Pin*, hash_pip, eq_pip>::iterator pipIterator =
			this->pips.begin();
	boost::unordered_set<Pin*, hash_pip, eq_pip>::iterator outIterator =
			this->outPins.begin();
	boost::unordered_set<Pin*, hash_pip, eq_pip>::iterator inIterator =
			this->inPins.begin();

	while (outIterator != this->outPins.end()) {
		Pin* tempPin = *outIterator;
		tempPin->printData(outStream);
		outIterator++;
	}
	while (inIterator != this->inPins.end()) {
		Pin* tempPin = *inIterator;
		tempPin->printData(outStream);
		inIterator++;
	}
	while (pipIterator != this->pips.end()) {
		Pin* tempPin = *pipIterator;
		tempPin->printData(outStream);
		pipIterator++;
	}
	*outStream << "\t;\n";
}
void Net::printData(std::ostream* outStream) {
	if (!this->cfg.empty() && this->configure == true) {
		*outStream << "net " << this->name << " , ";
		*outStream << "cfg \" " << this->cfg;
	} else {
		*outStream << "net " << this->name << " " << this->cfg << ",";
	}
	*outStream << " \n";
	boost::unordered_set<Pin*, hash_pip, eq_pip>::iterator pipIterator =
			this->pips.begin();
	boost::unordered_set<Pin*, hash_pip, eq_pip>::iterator outIterator =
			this->outPins.begin();
	boost::unordered_set<Pin*, hash_pip, eq_pip>::iterator inIterator =
			this->inPins.begin();

	while (outIterator != this->outPins.end()) {
		Pin* tempPin = *outIterator;
		tempPin->printData(outStream);
		outIterator++;
	}
	while (inIterator != this->inPins.end()) {
		Pin* tempPin = *inIterator;
		tempPin->printData(outStream);
		inIterator++;
	}
	while (pipIterator != this->pips.end()) {
		Pin* tempPin = *pipIterator;
		tempPin->printData(outStream);
		pipIterator++;
	}
	*outStream << "\t;\n";
}
/**
 * only compares net name
 */
bool Net::operator!=(const Net& other) const {
	if (this->name == other.name) {
		return false;
	}
	return true;
}
bool Net::operator==(const Net& other) const {
	if (this->name == other.name) {
		return true;
	}
	return false;
}
bool Net::operator<(const Net& other) const {
	if (this->name < other.name) {
		return true;
	}
	return false;
}
bool Net::operator>(const Net& other) const {
	if (this->name > other.name) {
		return true;
	}
	return false;
}
Net& Net::operator=(const Net& other) {
	this->name = other.name;
	this->cfg = other.cfg;
	boost::unordered_set<Pin*, hash_pip, eq_pip>::iterator pipIterator =
			other.pips.begin();
	boost::unordered_set<Pin*, hash_pip, eq_pip>::iterator outIterator =
			other.outPins.begin();
	boost::unordered_set<Pin*, hash_pip, eq_pip>::iterator inIterator =
			other.inPins.begin();
	while (outIterator != other.outPins.end()) {
		this->outPins.insert(*outIterator);
		outIterator++;
	}
	while (inIterator != other.inPins.end()) {
		this->inPins.insert(*inIterator);
		inIterator++;
	}
	while (pipIterator != other.pips.end()) {
		this->pips.insert(*pipIterator);
		pipIterator++;
	}
	return *this;
}
void Net::mergePips(const Net& other) {
	boost::unordered_set<Pin*, hash_pip, eq_pip>::iterator pipIterator =
			other.pips.begin();
	while (pipIterator != other.pips.end()) {
		this->pips.insert(*pipIterator);
		pipIterator++;
	}

}
std::size_t Net::hash_value(const Net & net) const {
	std::size_t seed = 0;
	boost::hash_combine(seed, net.name);
	return seed;
}
std::size_t Net::hash_value(const Net* net) const {
	std::size_t seed = 0;
	boost::hash_combine(seed, net->name);
	return seed;
}
bool Net::operator!=(const Net* other) const {
	if (this->name == other->name) {
		return false;
	}
	return true;
}
bool Net::operator==(const Net* other) const {
	if (this->name == other->name) {
		return true;
	}
	return false;
}
bool Net::operator<(const Net* other) const {
	if (this->name < other->name) {
		return true;
	}
	return false;
}
bool Net::operator>(const Net* other) const {
	if (this->name > other->name) {
		return true;
	}
	return false;
}
Net& Net::operator=(const Net* other) {
	this->name = other->name;
	this->cfg = other->cfg;
	boost::unordered_set<Pin*, hash_pip, eq_pip>::iterator pipIterator =
			other->pips.begin();
	boost::unordered_set<Pin*, hash_pip, eq_pip>::iterator outIterator =
			other->outPins.begin();
	boost::unordered_set<Pin*, hash_pip, eq_pip>::iterator inIterator =
			other->inPins.begin();

	while (outIterator != other->outPins.end()) {
		this->outPins.insert(*outIterator);
		outIterator++;
	}
	while (inIterator != other->inPins.end()) {
		this->inPins.insert(*inIterator);
		inIterator++;
	}
	while (pipIterator != other->pips.end()) {
		this->pips.insert(*pipIterator);
		pipIterator++;
	}
	return *this;
}
std::size_t Net::operator ()(const Net* &other) const {
	std::size_t seed = 0;
	boost::hash_combine(seed, other->name);
	return seed;
}
std::size_t Net::operator ()(const Net& other) const {
	std::size_t seed = 0;
	boost::hash_combine(seed, other.name);
	return seed;
}
void Net::remotePin(Pin* tempPin, int type) {
	/*
	 * 1=pip
	 * 2=inpin
	 * 3=outpin
	 */
	Pip* tempPip;
	InPin* tempInPin;
	OutPin* tempOutPin;

	if (type == 1) {
		tempPip = (dynamic_cast<Pip*> (tempPin));
		this->pips.erase(tempPip);
	}
	if (type == 2) {
		tempInPin = (dynamic_cast<InPin*> (tempPin));
		this->inPins.erase(tempInPin);
	}
	if (type == 3) {
		tempOutPin = (dynamic_cast<OutPin*> (tempPin));
		this->outPins.erase(tempOutPin);
	}
	//return type;
}

void Net::clearInPins() {
	boost::unordered_set<Pin*, hash_pip, eq_pip>::iterator inIterator =
			this->inPins.begin();
	for (; inIterator != inPins.end(); inIterator++) {
		this->inPins.erase(*inIterator);
	}
}
}
}
#endif /* NET_HPP_ */
