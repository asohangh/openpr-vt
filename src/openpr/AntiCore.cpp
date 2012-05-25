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
#ifndef ADB_ANTICORE_CC
#define ADB_ANTICORE_CC
#include "openpr/anticore/AntiCoreV5.hpp"
#include "openpr/anticore/AntiCoreV4.hpp"
#include "openpr/anticore/OpenPRTree.hpp"
#include "openpr/anticore/OpenPR.hpp"
#include "torc/common/Devices.hpp"
#include "torc/common/DirectoryTree.hpp"
#include "torc/common/DottedVersion.hpp"
#include "torc/common/EncapsulatedInteger.hpp"
#include "torc/common/Endian.hpp"
#include "torc/common/NullOutputStream.hpp"
#include "torc/common/TestHelpers.hpp"
#include "torc/common/DirectoryTree.hpp"
#include <string>
#include <fstream>
#include<boost/archive/xml_oarchive.hpp>
#include<boost/archive/xml_iarchive.hpp>
#include<boost/serialization/nvp.hpp>
#include<boost/filesystem.hpp>

void help_menu();

int main(int argc, char *argv[]) {
	using std::string;
	std::string arch, bottom, top, operations;
	std::fstream ucfFile("system.ucf", std::fstream::out);
	std::stringstream pathBuf;

	if (argc == 1) {
		help_menu();
		return -1;
	}

	if(argc == 2) {
		std::string argv1 = argv[1];
		if(!argv1.compare("-h") || !argv1.compare("--help")) {
			help_menu();
			return 0;
		}
	}

	torc::common::DirectoryTree directoryTree(argv[1]);
	pathBuf << operator/(torc::common::DirectoryTree::getExecutablePath(), "/torc/") << std::endl;
	openpr::OpenPRTree tr(pathBuf.str().c_str());

	if (argc == 6) {
		arch = argv[2];
		bottom = argv[3];
		top = argv[4];
		operations = argv[5];
		torc::architecture::DDB db(arch);
		openpr::AntiCoreBase* anticore = NULL;
		if (!arch.find("xc5", 0))
			anticore = new openpr::AntiCoreV5(db, bottom, top);
		if (!arch.find("xc4", 0))
			anticore = new openpr::AntiCoreV4(db, bottom, top);
		if (!operations.find("s", 0))
			anticore->blockSites();
		if (!operations.find("p", 0))
			anticore->genPlaceConstraints(16, ucfFile);
		if (!operations.find("r", 0))
			anticore->blockRoutes("top_placed.xdl", "top_blocked.xdl",
					"\"data_temp<3>\"");
		if (!operations.find("rp", 0)) {
			anticore->setMode(openpr::ePartial);
			anticore->blockRoutes("top_placed.xdl", "top_blocked.xdl",
					"\"data_temp<3>\"");
		}
	}
	else if (argc == 4)
	{
		string projFile = argv[2];
		string command = argv[3];
		std::ifstream project(projFile.c_str());
		if (!project.is_open()) {
			std::cerr << "Unable to open project file " << std::endl;
			return -1;
		}
		boost::archive::xml_iarchive ia(project);
		openpr::openPR myProj;
		boost::filesystem::path outputDb;
		ia >> BOOST_SERIALIZATION_NVP(myProj);
		bool success = true;
		if (command == "placeMacros") {
			success = myProj.placeMacros();
			outputDb /= "macrosPlaced.opr";
		} else if (command == "siteBlocker") {
			success = myProj.siteBlocker();
			outputDb /= "siteBlocked.opr";
		} else if (command == "genPlaceConstraints") {
			success = myProj.genPlaceConstraints();
			outputDb /= "placeConstrained.opr";
		} else if (command == "genLockConstraints") {
			success = myProj.genLockConstraints();
			outputDb /= "lockConstrained.opr";
		} else if (command == "routeBlocker") {
			success = myProj.routeBlocker();
			outputDb /= "routeBlocked.opr";
		} else if (command == "mergeClockTree") {
			success = myProj.mergeClockTree();
			outputDb /= "mergedClockTree.opr";
		} else if (command == "genPTScripts") {
			success = myProj.genPassThroughScripts();
			outputDb /= "ptScripted.opr";
		} else if (command == "genPartialBitstream") {
			success = myProj.genPartialBitstream();
			outputDb /= "partialGenerated.opr";
		} else {
			std::cerr << "Error: Invalid command specified" << std::endl;
			return -1;
		}
		if (!success)
			return -1;

		std::ofstream outProj(outputDb.string().c_str());
		boost::archive::xml_oarchive oa(outProj);
		oa << BOOST_SERIALIZATION_NVP(myProj);
	} else {
		std::cerr
				<< "Incorrect number of parameters"
				<< std::endl;
    help_menu();
	}
};

void help_menu()
{
	std::cout 
		<< "Invalid number of parameters for anticore, please follow usage below:"
		<< std::endl;
	std::cout 
		<< "\tFor use without project file:"
		<< std::endl
		<< "\t\tanticore <device> <starting_site> <ending_site> <anticore operation>"
		<< std::endl;
	std::cout 
		<< "\tFor use with project file:"
		<< std::endl
		<< "\t\tanticore <anticore executable location> <project file> <OpenPR operation> <ending_site> <anticore operation>"
		<< std::endl;
}

void self_test()
{
	std::fstream ucfFile("system.ucf", std::fstream::out);
  // Virtex 5 Testbench
  try {
    std::cout
      << "Attempting self test"
      << std::endl;
		torc::architecture::DDB db("xc5vsx95t");
		// Construct anticore with and blockSites in the same column as bus macro tiles
		openpr::AntiCoreV5 anticore(db, "TIEOFF_X36Y60", "SLICE_X91Y79");
		anticore.blockSites();
		anticore.genPlaceConstraints(8, ucfFile);
		anticore.blockRoutes();
  }
  catch (std::exception &e) {
    std::cerr
      << "Unable to locate device database"
      << std::endl;
    exit(-1);
  }
}

#endif // ADB_CANTICORE_H
