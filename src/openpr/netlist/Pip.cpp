/*
 * Pip.cpp
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
#ifndef PIP_HPP_
#define PIP_HPP_
#include "openpr/netlist/Pip.h"
/**
 * This constructor takes in a single string that is in the format of "pip CLBLM_X36Y59 CLBLM_X36Y59 -> CLBLM_X36Y59 ,"
 * @param input the input string
 */
namespace openpr {
namespace netlist {
Pip::Pip(std::string input, torc::architecture::DDB* mDB) {
	this->mDB = mDB;
	boost::char_separator<char> sep(" ");
	tokenizer tokens(input, sep);
	tokenizer::iterator tok_iter = tokens.begin();
	tok_iter++;
	std::string tempStr = *tok_iter;
	this->location = std::string(tempStr);
	this->parseLocation(tempStr);
	tok_iter++;
	tempStr = *tok_iter;

	// Adding points takes way too long if you don't need conflict resolution
	//this->source = new Point(this->location,tempStr , true,mDB);
	this->source = new Point(this->location, tempStr, true, NULL);
	tok_iter++;
	tok_iter++;
	tempStr = *tok_iter;

	// Adding points takes way too long if you don't need conflict resolution
	//this->destination = new Point(this->location,tempStr , false,mDB);
	this->destination = new Point(this->location, tempStr, false, NULL);
	// Import pips into CDB to avoid top_placed.xdl ugliness
	torc::architecture::Arc tempArc = mDB->tilePipToArc(this->location, this->getSourceStr(),
			this->getSinkStr());
	mDB->useArc(tempArc);
	//	printf("1%s\n", tempStr.c_str());
}
/**
 * This constructor takes in a paramaterized list of inputs to construct at Pip
 * @param type  This should be CLBLM, INT ...
 * @param x 	This is the X tile location
 * @param y 	This is the Y tile location
 * @param source This is the source of pip
 * @param destination This is the destination(sink) of the pip
 */
Pip::Pip(std::string type, int x, int y, std::string source,
		std::string destination, torc::architecture::DDB* mDB) {
	this->mDB = mDB;
	this->type = type;
	this->xLoc = x;
	this->yLoc = y;
	std::string tempLocation = this->generateLocation();
	this->source = new Point(tempLocation, source, true, mDB);
	this->destination = new Point(tempLocation, destination, false, mDB);
}
/**
 * This consturctor merges the two above constructors
 * @param location this should be INT_X27Y70
 * @param source this is the source of the pip
 * @param destination this is the destination(sink) of the pip
 */
Pip::Pip(std::string location, std::string source, std::string destination,
		torc::architecture::DDB* mDB) {
	this->mDB = mDB;
	this->location = location;
	this->parseLocation(location);
	this->source = new Point(location, source, true, mDB);
	this->destination = new Point(location, destination, false, mDB);
}
/**
 * This parses the location into individual parts(Type, x, and y)
 * @param location something like INT_X27Y70
 */
void Pip::parseLocation(std::string location) {
	int underscore = location.find("_");
	this->type = location.substr(0, underscore);
	std::string remainder = location.substr(underscore + 1);
	int y = remainder.find("Y");
	this->xLoc = atoi(remainder.substr(1, y - 1).c_str());
	this->yLoc = atoi(remainder.substr(y + 1).c_str());

}
/*
 * Destructor
 */
Pip::~Pip() {
	delete this->source;
	delete this->destination;
}
void Pip::printData(std::ofstream* outStream) {
	if (this->location == "") {
		*outStream << "pip" << "\n";
	} else {
		*outStream << "\tpip " << this->location << " "
				<< this->source->getSourceDestination() << " -> "
				<< this->destination->getSourceDestination() << "  ,\n";
	}
}
void Pip::printData(std::ostream* outStream) {
	if (this->location == "") {
		*outStream << "pip" << "\n";
	} else {
		*outStream << "\tpip " << this->location << " "
				<< this->source->getSourceDestination() << " -> "
				<< this->destination->getSourceDestination() << "  ,\n";
	}
}
bool Pip::operator!=(Pin &other) {
	Pip* c;
	try {
		c = (dynamic_cast<Pip*> (&other));

	} catch (std::bad_cast) {
		return true;
	}
	if (this->xLoc == c->xLoc && this->yLoc == c->yLoc) {
		if (this->type == c->type) {
			if (this->source == c->source && this->destination
					== c->destination) {
				return true;
			}
		}
	}
	return false;
}
bool Pip::operator==(const Pin &other) const {
	Pip* c;
	try {
		c = (dynamic_cast<Pip*> (const_cast<Pin*> (&other)));
	} catch (std::bad_cast) {
		return false;
	}

	if (this->xLoc == c->xLoc && this->yLoc == c->yLoc) {
		if (this->type == c->type) {
			if (this->source == c->source && this->destination
					== c->destination) {
				return true;
			}
		}
	}
	return false;
}
bool Pip::operator<(Pin &other) {
	Pip* c;
	try {
		c = (dynamic_cast<Pip*> (&other));
	} catch (std::bad_cast) {
		return false;
	}
	if (this->type == c->type) {
		if (this->xLoc == c->xLoc) {
			if (this->yLoc == c->yLoc) {
				if (this->source == c->source) {
					if (this->destination == c->destination) {
						return false;
					} else if (this->destination < c->destination) {
						return true;
					}
				} else if (this->source < c->source) {
					return true;
				}
			} else if (this->yLoc < c->yLoc) {
				return true;
			}
		} else if (xLoc < c->xLoc) {
			return true;
		}
	} else if (this->type < c->type) {
		return true;
	}
	return false;
}
bool Pip::operator>(Pin &other) {
	Pip* c;
	try {
		c = (dynamic_cast<Pip*> (&other));
	} catch (std::bad_cast) {
		return true;
	}
	if (this->type == c->type) {
		if (this->xLoc == c->xLoc) {
			if (this->yLoc == c->yLoc) {
				if (this->source == c->source) {
					if (this->destination == c->destination) {
						return false;
					} else if (this->destination > c->destination) {
						return true;
					}
				} else if (this->source > c->source) {
					return true;
				}
			} else if (this->yLoc > c->yLoc) {
				return true;
			}
		} else if (xLoc > c->xLoc) {
			return true;
		}
	} else if (this->type > c->type) {
		return true;
	}
	return false;
}
Pin& Pip::operator=(Pin& other) {
	Pip* c;
	try {
		c = (dynamic_cast<Pip*> (&other));
	} catch (std::bad_cast) {
		printf("Invalid Cast");
		return *this;
	}
	this->destination = c->destination;
	this->location = c->location;
	this->source = c->source;
	this->type = c->type;
	this->wholeData = c->wholeData;
	this->xLoc = c->xLoc;
	this->yLoc = c->yLoc;
	return *this;
}
bool Pip::operator!=(Pin* other) {
	Pip* c;

	try {
		c = (dynamic_cast<Pip*> (other));
	} catch (std::bad_cast) {
		return true;
	}
	if (this->xLoc == c->xLoc && this->yLoc == c->yLoc) {
		if (this->type == c->type) {
			if (this->source == c->source && this->destination
					== c->destination) {
				return true;
			}
		}
	}
	return false;
}
bool Pip::operator==(const Pin* other) const {
	Pip* c;

	try {
		c = (dynamic_cast<Pip*> (const_cast<Pin*> (other)));
		;
	} catch (std::bad_cast) {
		return false;
	}
	if (other == NULL) {
		return false;
	}
	if (this->xLoc == c->xLoc && this->yLoc == c->yLoc) {
		if (this->type == c->type) {
			if (this->source == c->source && this->destination
					== c->destination) {
				return true;
			}
		}
	}
	return false;
}
bool Pip::operator<(Pin* other) {
	Pip* c;
	try {
		c = (dynamic_cast<Pip*> (other));
	} catch (std::bad_cast) {
		return false;
	}
	if (this->type == c->type) {
		if (this->xLoc == c->xLoc) {
			if (this->yLoc == c->yLoc) {
				if (this->source == c->source) {
					if (this->destination == c->destination) {
						return false;
					} else if (this->destination < c->destination) {
						return true;
					}
				} else if (this->source < c->source) {
					return true;
				}
			} else if (this->yLoc < c->yLoc) {
				return true;
			}
		} else if (xLoc < c->xLoc) {
			return true;
		}
	} else if (this->type < c->type) {
		return true;
	}
	return false;
}
bool Pip::operator>(Pin* other) {
	Pip* c;

	try {
		c = (dynamic_cast<Pip*> (other));
	} catch (std::bad_cast) {

		return true;
	}
	if (this->type == c->type) {
		if (this->xLoc == c->xLoc) {
			if (this->yLoc == c->yLoc) {
				if (this->source == c->source) {
					if (this->destination == c->destination) {
						return false;
					} else if (this->destination > c->destination) {
						return true;
					}
				} else if (this->source > c->source) {
					return true;
				}
			} else if (this->yLoc > c->yLoc) {
				return true;
			}
		} else if (xLoc > c->xLoc) {
			return true;
		}
	} else if (this->type > c->type) {
		return true;
	}
	return false;
}
Pin* Pip::operator=(Pin* other) {
	Pip* c;
	try {
		c = (dynamic_cast<Pip*> (other));
	} catch (std::bad_cast) {
		printf("Invalid Cast");
		return this;
	}
	this->destination = c->destination;
	this->location = c->location;
	this->source = c->source;
	this->type = c->type;
	this->wholeData = c->wholeData;
	this->xLoc = c->xLoc;
	this->yLoc = c->yLoc;
	return this;
}

std::size_t Pip::operator ()(const Pin & x) const {
	std::size_t seed = 0;
	Pip* c;
	try {
		c = (dynamic_cast<Pip*> (const_cast<Pin*> (&x)));
	} catch (std::bad_cast) {
		printf("Invalid Cast");
		return seed;
	}
	boost::hash_combine(seed, c->type);
	boost::hash_combine(seed, c->xLoc);
	boost::hash_combine(seed, c->yLoc);
	boost::hash_combine(seed, c->source);
	boost::hash_combine(seed, c->destination);
	return seed;
}
std::size_t Pip::operator ()(const Pin * x) const {
	std::size_t seed = 0;
	Pip* c;
	try {
		c = (dynamic_cast<Pip*> (const_cast<Pin*> (x)));
	} catch (std::bad_cast) {
		printf("Invalid Cast");
		return seed;
	}
	boost::hash_combine(seed, c->type);
	boost::hash_combine(seed, c->xLoc);
	boost::hash_combine(seed, c->yLoc);
	boost::hash_combine(seed, c->source);
	boost::hash_combine(seed, c->destination);
	return seed;
}

std::size_t Pip::operator ()(const Pip & x) const {
	std::size_t seed = 0;
	Pip* c = const_cast<Pip*> (&x);
	boost::hash_combine(seed, c->type);
	boost::hash_combine(seed, c->xLoc);
	boost::hash_combine(seed, c->yLoc);
	boost::hash_combine(seed, c->source);
	boost::hash_combine(seed, c->destination);
	return seed;
}
std::size_t Pip::operator ()(const Pip * c) const {
	std::size_t seed = 0;
	boost::hash_combine(seed, c->type);
	boost::hash_combine(seed, c->xLoc);
	boost::hash_combine(seed, c->yLoc);
	boost::hash_combine(seed, c->source);
	boost::hash_combine(seed, c->destination);
	return seed;
}
std::string Pip::generateLocation() const {
	std::string output;
	output.append(this->location);
	output.append("_X");
	output.append(boost::lexical_cast<std::string>(this->xLoc));
	output.append("Y");
	output.append(boost::lexical_cast<std::string>(this->yLoc));
	return output;
}
Point* Pip::getDestination() const {
	return this->destination;
}
Point* Pip::getSource() const {
	return this->source;
}
std::string Pip::getTileStr() {
	return this->location;
}
std::string Pip::getSourceStr() {
	return this->source->getSourceDestination();
}
std::string Pip::getSinkStr() {
	return this->destination->getSourceDestination();
}
}
}
#endif /* PIP_HPP_ */
