/*
 * OutPin.cpp
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

#ifndef OUTPIN_HPP_
#define OUTPIN_HPP_
#include "openpr/netlist/OutPin.h"
namespace openpr {
namespace netlist {
OutPin::OutPin(std::string outpin, std::string location) {
	// TODO Auto-generated constructor stub
	this->outpin = outpin;
	this->location = location;
}

OutPin::~OutPin() {
	// TODO Auto-generated destructor stub

}
void OutPin::printData(std::ofstream* outStream) {
	*outStream << "outpin " << this->outpin << " " << this->location << " ,\n";
}
void OutPin::printData(std::ostream* outStream) {
	*outStream << "outpin " << this->outpin << " " << this->location << " ,\n";
}
bool OutPin::operator !=(Pin& other) {
	OutPin* c;
	try {
		c = (dynamic_cast<OutPin*> (&other));
	} catch (std::bad_cast) {
		return true;
	}
	if (this->outpin == c->outpin) {
		return false;
	}
	if (this->location == c->location) {
		return false;
	}
	return true;
}
bool OutPin::operator <(Pin & other) {
	OutPin* c;
	try {
		c = (dynamic_cast<OutPin*> (&other));
	} catch (std::bad_cast) {
		return true;
	}
	if (this->outpin < c->outpin) {
		return true;
	} else if (this->outpin == c->outpin) {
		if (this->location < c->location) {
			return true;
		}
	}
	return false;
}

Pin& OutPin::operator=(Pin& other) {
	OutPin* c;
	try {
		c = (dynamic_cast<OutPin*> (&other));
	} catch (std::bad_cast) {
		printf("Invalid Cast");
		return *this;
	}
	this->outpin = c->outpin;
	this->location = c->location;
	return *this;
}
bool OutPin::operator ==(const Pin& other) const {
	OutPin* c;
	try {
		c = (dynamic_cast<OutPin*> (const_cast<Pin*> (&other)));
	} catch (std::bad_cast) {
		return false;
	}
	if (this->outpin == c->outpin && this->location == c->location) {
		return true;
	}
	return false;
}
bool OutPin::operator >(Pin& other) {
	OutPin* c;
	try {
		c = (dynamic_cast<OutPin*> (&other));
	} catch (std::bad_cast) {
		return false;
	}
	if (this->outpin > c->outpin) {
		return true;
	} else if (this->outpin == c->outpin) {
		if (this->location > c->location) {
			return true;
		}
	}
	return false;
}
std::size_t OutPin::operator ()(const Pin & x) const {
	std::size_t seed = 0;
	OutPin* c;
	try {
		c = (dynamic_cast<OutPin*> (const_cast<Pin*> (&x)));
	} catch (std::bad_cast) {
		printf("Invalid Cast");
		return seed;
	}
	boost::hash_combine(seed, c->outpin);
	boost::hash_combine(seed, c->location);
	return seed;
}
bool OutPin::operator !=(Pin* other) {
	OutPin* c;
	try {
		c = (dynamic_cast<OutPin*> (other));
	} catch (std::bad_cast) {
		return true;
	}
	if (this->outpin == c->outpin) {
		return false;
	}
	if (this->location == c->location) {
		return false;
	}
	return true;
}
bool OutPin::operator <(Pin* other) {
	OutPin* c;
	try {
		c = (dynamic_cast<OutPin*> (other));
	} catch (std::bad_cast) {
		return true;
	}
	if (this->outpin < c->outpin) {
		return true;
	} else if (this->outpin == c->outpin) {
		if (this->location < c->location) {
			return true;
		}
	}
	return false;
}

Pin* OutPin::operator=(Pin* other) {
	OutPin* c;
	try {
		c = (dynamic_cast<OutPin*> (other));
	} catch (std::bad_cast) {
		printf("Invalid Cast");
		return this;
	}
	this->outpin = c->outpin;
	this->location = c->location;
	return this;
}
bool OutPin::operator ==(const Pin* other) const {
	if ((void*) other == NULL)
		return false;

	OutPin* c;
	try {
		c = (dynamic_cast<OutPin*> (const_cast<Pin*> (other)));
	} catch (std::bad_cast) {
		return false;
	}
	if (this->outpin == c->outpin && this->location == c->location) {
		return true;
	}
	return false;
}
bool OutPin::operator >(Pin* other) {
	OutPin* c;
	try {
		c = (dynamic_cast<OutPin*> (other));
	} catch (std::bad_cast) {
		return false;
	}
	if (this->outpin > c->outpin) {
		return true;
	} else if (this->outpin == c->outpin) {
		if (this->location > c->location) {
			return true;
		}
	}
	return false;
}
std::size_t OutPin::operator ()(const Pin * x) const {
	std::size_t seed = 0;
	OutPin* c;
	try {
		c = (dynamic_cast<OutPin*> (const_cast<Pin*> (x)));
	} catch (std::bad_cast) {
		printf("Invalid Cast");
		return seed;
	}
	boost::hash_combine(seed, c->outpin);
	boost::hash_combine(seed, c->location);
	return seed;
}
}
}
#endif /* OUTPIN_HPP_ */
