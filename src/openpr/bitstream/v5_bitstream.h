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


#include "openpr/bitstream/bitstream.h"
#include <fstream>
#include <iomanip>
#include <netinet/in.h>

namespace openpr {
namespace bitstream
{
	class v5_bitstream : public bitstream {

	protected:

		// packet type: UG191, v3.7, June 24, 2009, Tables 6-2 and 6-4
		enum EPacketType { eType1 = 1, eType2, eTypeCount = 8 };

		// opcode format: UG191, v3.7, June 24, 2009, Table 6-3
		enum EOpcode { eOpNOP = 0, eOpRead, eOpWrite, eOpReserved, eOpcodeCount };

		// configuration registers: UG191, v3.7, June 24, 2009, Table 6-5
		enum ERegister { eRegCRC = 0, eRegFAR, eRegFDRI, eRegFDRO, eRegCMD, eRegCTL0, eRegMASK, eRegSTAT, eRegLOUT, 
			eRegCOR0, eRegMFWR, eRegCBC, eRegIDCODE, eRegAXSS, eRegCOR1, eRegCSOB, eRegWBSTAR, eRegTIMER, 
			eRegBOOTSTS = 22, eRegCTL1 = 24, eRegCount };

		// CMD register commands: UG191, v3.7, June 24, 2009, Table 6-6
		enum ECommand { eCmdNULL = 0, eCmdWCFG, eCmdMFW, eCmdDGHIGH, eCmdLFRM = eCmdDGHIGH, eCmdRCFG, eCmdSTART, eCmdRCAP, 
			eCmdRCRC, eCmdAGHIGH, eCmdSWITCH, eCmdGRESTORE, eCmdSHUTDOWN, eCmdGCAPTURE, eCmdDESYNCH, eCmdIPROG = 15, 
			eCmdLTIMER = 17, eCmdCount };

		enum EShifts {
			// type 1 packet format: UG191, v3.7, June 24, 2009, Table 6-2
			// type 2 packet format: UG191, v3.7, June 24, 2009, Table 6-4
			// generic packet subfields
			eShiftPacketType =			29,
			eShiftPacketOpcode =		27,
			// type 1 packet subfields
			eShiftType1Address =		13,
			eShiftType1Reserved =		11,
			eShiftType1Count =			0,
			// type 2 packet subfields
			eShiftType2Count =			0
		};

		enum EMasks {
			// type 1 packet format: UG191, v3.7, June 24, 2009, Table 7-2
			// type 2 packet format: UG191, v3.7, June 24, 2009, Table 7-4
			// generic packet subfields
			eMaskPacketType =			0x00000007,
			eMaskPacketOpcode =			0x00000003,
			// type 1 packet subfields
			eMaskType1Address =			0x00003fff,
			eMaskType1Reserved =		0x00000003,
			eMaskType1Count =			0x000007ff,
			// type 2 packet subfields
			eMaskType2Count =			0x07ffffff
		};

		enum EWords {
			eDummyWord = 0xFFFFFFFF,
			eBusWidthWord = 0x000000BB,
			eBusWidth = 0x11220044,
			eSyncWord = 0xAA995566
		};

		enum EShiftFAR {
		/** @see Frame Address Register bitfields obtained from UG191 v3.6 Figure 6-10. */
			eShiftBlockType = 21,
			eShiftTB = 20,
			eShiftRow = 15,
			eShiftMajor = 7,
			eShiftMNA = 0
		};

		boost::uint32_t mRegister[eRegCount];
		static const char* sTypeName[eTypeCount];
		static const char* sOpcodeName[eOpcodeCount];
		static const char* sRegisterName[eRegCount];
		static const char* sCommandName[eCmdCount];

		// read frame data
		virtual bool writeFrameData(fstream& inStream, int inCount);
		/**
		 * Convert the hex Frame Address Register to a frame_addr struct.
		 * @param far Frame Address Register value to be converted.
		 */
		openpr::bitstream::frame_addr farToStruct(boost::uint32_t far);
		/**
		 * Convert the frame_addr struct to the 32bit hex frame address register format.
		 * @param far frame_addr struct to be converted.
		 */
		boost::uint32_t structToFar(openpr::bitstream::frame_addr far);
	public:


		// constructor
		v5_bitstream(string device_name);
		// null constructor for the standalone bitstream reader
		v5_bitstream(void);

		// parse packets and read bitstream contents
		virtual bool readPackets(fstream& inStream);
		/**
		 * Write a packet header to the bitstream.
		 * @param outStream Bitstream to be written.
		 * @param packetType uint indicating type 1 or type 2 packet.
		 * @param opcode Operation to be performed on register.
		 * @param address Register address to be written.
		 * @param reserved Param for reserved opcodes.
		 * @param count Number of words to be written in packet.
		 */ 
		virtual bool writePacketHeader(fstream& outStream, boost::uint32_t packetType, boost::uint32_t opcode, boost::uint32_t address, boost::uint32_t reserved, boost::uint32_t count);
		/**
		 * Write packets in proper sequence for a full bitstream.
		 * @param outStream Bitstream file to be written
		 */
		virtual bool writePackets(fstream& outStream);
		/**
		 * Write packets in proper sequence for a partial bitstream.
		 * @param outStream Bitstream file to be written
		 */
		virtual bool writePacketsPartial(fstream& outStream);
		/**
		 * Write out partial bitstream.
		 * @param outStream bitstream file to be written
		 */
		virtual bool writePartialFrames(fstream& outStream);
	};
}
}
