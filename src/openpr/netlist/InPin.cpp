/*
 * InPin.cpp
 *
 *  Created on: Feb 23, 2010
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
#ifndef INPIN_HPP_
#define INPIN_HPP_
#include "openpr/netlist/InPin.h"
namespace openpr {
namespace netlist {
InPin::InPin(std::string inpin, std::string location) {
	// TODO Auto-generated constructor stub
	this->inpin = inpin;
	this->location = location;
}

InPin::~InPin() {
	// TODO Auto-generated destructor stub

}
void InPin::printData(std::ofstream* outStream) {
	*outStream << "inpin " << this->inpin << " " << this->location << " ,\n";
}
void InPin::printData(std::ostream* outStream) {
	*outStream << "inpin " << this->inpin << " " << this->location << " ,\n";
}
bool InPin::operator !=(Pin& other) {
	InPin* c;
	try {
		c = (dynamic_cast<InPin*> (&other));
	} catch (std::bad_cast) {
		return true;
	}
	if (this->inpin == c->inpin) {
		return false;
	}
	if (this->location == c->location) {
		return false;
	}
	return true;
}
bool InPin::operator <(Pin & other) {
	InPin* c;
	try {
		c = (dynamic_cast<InPin*> (&other));
	} catch (std::bad_cast) {
		return true;
	}
	if (this->inpin < c->inpin) {
		return true;
	} else if (this->inpin == c->inpin) {
		if (this->location < c->location) {
			return true;
		}
	}
	return false;
}

Pin& InPin::operator=(Pin& other) {
	InPin* c;
	try {
		c = (dynamic_cast<InPin*> (&other));
	} catch (std::bad_cast) {
		printf("Invalid Cast");
		return *this;
	}
	this->inpin = c->inpin;
	this->location = c->location;
	return *this;
}
bool InPin::operator ==(const Pin& other) const {
	InPin* c;
	try {
		c = (dynamic_cast<InPin*> (const_cast<Pin*> (&other)));
	} catch (std::bad_cast) {
		return false;
	}
	if (this->inpin == c->inpin && this->location == c->location) {
		return true;
	}
	return false;
}
bool InPin::operator >(Pin& other) {
	InPin* c;
	try {
		c = (dynamic_cast<InPin*> (&other));
	} catch (std::bad_cast) {
		return false;
	}
	if (this->inpin > c->inpin) {
		return true;
	} else if (this->inpin == c->inpin) {
		if (this->location > c->location) {
			return true;
		}
	}
	return false;
}
std::size_t InPin::operator ()(const Pin & x) const {
	std::size_t seed = 0;
	InPin* c;
	try {
		c = (dynamic_cast<InPin*> (const_cast<Pin*> (&x)));
	} catch (std::bad_cast) {
		printf("Invalid Cast");
		return seed;
	}
	boost::hash_combine(seed, c->inpin);
	boost::hash_combine(seed, c->location);
	return seed;
}
bool InPin::operator !=(Pin* other) {
	InPin* c;
	try {
		c = (dynamic_cast<InPin*> (other));
	} catch (std::bad_cast) {
		return true;
	}
	if (this->inpin == c->inpin) {
		return false;
	}
	if (this->location == c->location) {
		return false;
	}
	return true;
}
bool InPin::operator <(Pin* other) {
	InPin* c;
	try {
		c = (dynamic_cast<InPin*> (other));
	} catch (std::bad_cast) {
		return true;
	}
	if (this->inpin < c->inpin) {
		return true;
	} else if (this->inpin == c->inpin) {
		if (this->location < c->location) {
			return true;
		}
	}
	return false;
}

Pin* InPin::operator=(Pin* other) {
	InPin* c;
	try {
		c = (dynamic_cast<InPin*> (other));
	} catch (std::bad_cast) {
		printf("Invalid Cast");
		return this;
	}
	this->inpin = c->inpin;
	this->location = c->location;
	return this;
}
bool InPin::operator ==(const Pin* other) const {
	InPin* c;
	try {
		c = (dynamic_cast<InPin*> (const_cast<Pin*> (other)));
	} catch (std::bad_cast) {
		return false;
	}
	if (other == NULL) {
		return false;
	}
	if (this->inpin == c->inpin && this->location == c->location) {
		return true;
	}
	return false;
}
bool InPin::operator >(Pin* other) {
	InPin* c;
	try {
		c = (dynamic_cast<InPin*> (other));
	} catch (std::bad_cast) {
		return false;
	}
	if (this->inpin > c->inpin) {
		return true;
	} else if (this->inpin == c->inpin) {
		if (this->location > c->location) {
			return true;
		}
	}
	return false;
}
std::size_t InPin::operator ()(const Pin * x) const {
	std::size_t seed = 0;
	InPin* c;
	try {
		c = (dynamic_cast<InPin*> (const_cast<Pin*> (x)));
	} catch (std::bad_cast) {
		printf("Invalid Cast");
		return seed;
	}
	boost::hash_combine(seed, c->inpin);
	boost::hash_combine(seed, c->location);
	return seed;
}
}
}
#endif /* INPIN_HPP_ */
