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

#ifndef PROHIBITRANGE_H
#define PROHIBITRANGE_H

#include<string>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
namespace openpr {
using namespace std;
/**
 * The prohibitRange struct represents a range of Sites that can be added to a
 * PROHIBIT constraint by the tools.  */
struct prohibitRange {
	/** First site in range for this SITE type */
	string minSite;
	/** Last site in range for this SITE type */
	string maxSite;
	prohibitRange() :
		minSite(""), maxSite("") {
	}
	prohibitRange(string minSite, string maxSite) :
		minSite(minSite), maxSite(maxSite) {
	}
	/**
	 * Copy constructor.
	 * @param rhs object to copy from.
	 */
	prohibitRange(const prohibitRange& rhs) {
		minSite = rhs.minSite;
		maxSite = rhs.maxSite;
	}
	/**
	 * Compare two prohibitRanges and check whether the RHS or LHS value
	 * should represent the bottom corner of the range.
	 * @param rhs Right hand side value to be compared
	 */
	bool operator <(const prohibitRange& rhs) {
		// Extract X,Y coordinates from site
		static const boost::regex extractCoords("(\\w*)_X(\\d+)Y(\\d+)");
		boost::smatch siteACoords, siteBCoords;

		try {
			if (!regex_match(minSite, siteACoords, extractCoords))
				throw(minSite);
			if (!regex_match(rhs.minSite, siteBCoords, extractCoords))
				throw(rhs.minSite);

			int xA = boost::lexical_cast<int>(siteACoords[2]);
			int xB = boost::lexical_cast<int>(siteBCoords[2]);
			int yA = boost::lexical_cast<int>(siteACoords[3]);
			int yB = boost::lexical_cast<int>(siteBCoords[3]);

			if (xA < xB)
				return true;
			else if (xA == xB) {
				if (yA < yB)
					return true;
				else
					return false;
			} else
				return false;
		} catch (const string& failSite) {
			cerr << "CAntiCore::Site " << failSite
					<< " does not have valid coordinates" << endl;
			return false;
		}
	}
	/**
	 * Compare two prohibitRanges and check whether the RHS or LHS value
	 * should represent the top corner of the range.
	 * @param rhs prohibitRange object to compare too
	 */
	bool operator >(const prohibitRange& rhs) {
		// Extract X,Y coordinates from site
		static const boost::regex extractCoords("(\\w*)_X(\\d+)Y(\\d+)");
		boost::smatch siteACoords, siteBCoords;

		try {
			if (!regex_match(maxSite, siteACoords, extractCoords))
				throw(maxSite);
			if (!regex_match(rhs.maxSite, siteBCoords, extractCoords))
				throw(rhs.maxSite);

			int xA = boost::lexical_cast<int>(siteACoords[2]);
			int xB = boost::lexical_cast<int>(siteBCoords[2]);
			int yA = boost::lexical_cast<int>(siteACoords[3]);
			int yB = boost::lexical_cast<int>(siteBCoords[3]);

			if (yA > yB)
				return true;
			else if (yA == yB) {
				if (xA > xB)
					return true;
				else
					return false;
			} else
				return false;
		} catch (const string& failSite) {
			cerr << "CAntiCore::Site " << failSite
					<< " does not have valid coordinates" << endl;
			return false;
		}
	}
};
}
#endif
