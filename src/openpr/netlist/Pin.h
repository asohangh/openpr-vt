/*
 * Pin.h
 *
 *  Created on: Feb 24, 2010
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
#ifndef PIN_H_
#define PIN_H_
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <typeinfo>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <boost/functional/hash.hpp>
#include <boost/lexical_cast.hpp>

namespace openpr {
namespace netlist {
typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

class Pin {
public:
	Pin();
	virtual ~Pin();
	virtual void printData(std::ofstream* outStream)=0;
	virtual void printData(std::ostream* outStream)=0;
	virtual bool operator!=(Pin &other)=0;
	virtual bool operator!=(Pin* other)=0;
	virtual bool operator==(const Pin &other) const =0;
	virtual bool operator==(const Pin* other) const =0;
	virtual bool operator<(Pin &other)=0;
	virtual bool operator<(Pin* other)=0;
	virtual bool operator>(Pin &other)=0;
	virtual bool operator>(Pin* other)=0;
	virtual Pin& operator=(Pin &other)=0;
	virtual Pin* operator=(Pin* other)=0;
	virtual size_t operator()(const Pin& x) const =0;
	virtual size_t operator()(const Pin* x) const =0;
	friend std::size_t hash_value(Pin* p);
	friend std::size_t hash_value(Pin &p);

};
}
}
#endif /* PIN_H_ */
