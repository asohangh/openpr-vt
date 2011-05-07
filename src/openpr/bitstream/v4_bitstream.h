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
namespace bitstream {
using std::exception;
	class v4_bitstream : public bitstream {

	protected:

		/** @see packet type: UG071, v1.10, April 8, 2008, Tables 7-2 and 7-4 */
		enum EPacketType { eType1 = 1, eType2, eTypeCount = 8 };

		/** @see opcode format: UG071, v1.10, April 8, 2008, Table 7-3 */
		enum EOpcode { eOpNOP = 0, eOpRead, eOpWrite, eOpReserved, eOpcodeCount };

		/** @see configuration registers: UG071, v1.10, April 8, 2008, Table 7-5 */
		enum ERegister { eRegCRC = 0, eRegFAR, eRegFDRI, eRegFDRO, eRegCMD, eRegCTL, eRegMASK, eRegSTAT, eRegLOUT, 
			eRegCOR, eRegMFWR, eRegCBC, eRegIDCODE, eRegAXSS, eRegCount };

		/** @see CMD register commands: UG071, v1.10, April 8, 2008, Table 7-6 */
		enum ECommand { eCmdNULL = 0, eCmdWCFG, eCmdMFWR, eCmdLFRM, eCmdRCFG, eCmdSTART, eCmdRCAP, eCmdRCRC, 
			eCmdAGHIGH, eCmdSWITCH, eCmdGRESTORE, eCmdSHUTDOWN, eCmdGCAPTURE, eCmdDESYNC, eCmdCount };

		/** @see type 1 packet format: UG071, v1.10, April 8, 2008, Table 7-2 */
		/** @see type 2 packet format: UG071, v1.10, April 8, 2008, Table 7-4 */
		enum EShifts {
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

		/** @see type 1 packet format: UG071, v1.10, April 8, 2008, Table 7-2 */
		/** @see type 2 packet format: UG071, v1.10, April 8, 2008, Table 7-4 */
		enum EMasks {
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
			eSyncWord = 0xAA995566
		};

		/** @see Frame Address Register Description: UG071, v1.10, April 8, 2008, Table 7-8 */
		enum EShiftFAR {
			eShiftBlockType = 19,
			eShiftTB = 22,
			eShiftRow = 14,
			eShiftMajor = 6,
			eShiftMNA = 0
		};
		static const int top = 0;
		boost::uint32_t mRegister[eRegCount];
		static const char* sTypeName[eTypeCount];
		static const char* sOpcodeName[eOpcodeCount];
		static const char* sRegisterName[eRegCount];
		static const char* sCommandName[eCmdCount];

		// read frame data
		virtual bool writeFrameData(std::fstream& inStream, int inCount);
		/**
		 * unmangle the bytes for a tile pair
		 * @param ptr A pointer to the frame
		 */
		void unmangleTilePair(boost::uint16_t* ptr);
		/**
		 * remangle the bytes for a tile pair
		 * @param ptr A pointer to the frame
		 */
		//void remangleTilePair(boost::uint16_t* ptr);
		/**
		 * reverse the order of the bits in a frame
		 * @param ptr A pointer to the frame
		 */
		void reverseFrameBits(boost::uint8_t* ptr);
		/**
		 * Prepare frame for writeback.
		 * @param fIndex index of frame to prepare
		 */
		//void prepFrameForWrite(boost::uint32_t fIndex);
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
		v4_bitstream(std::string device_name, bool frame_ecc = false);
		// null constructor for the standalone bitstream reader
		v4_bitstream(void);

		// parse packets and read bitstream contents
		virtual bool readPackets(std::fstream& inStream);
		/**
		 * Write a packet header to the bitstream.
		 * @param outStream Bitstream to be written.
		 * @param packetType uint indicating type 1 or type 2 packet.
		 * @param opcode Operation to be performed on register.
		 * @param address Register address to be written.
		 * @param reserved Param for reserved opcodes.
		 * @param count Number of words to be written in packet.
		 */ 
		virtual bool writePacketHeader(std::fstream& outStream, boost::uint32_t packetType, boost::uint32_t opcode, boost::uint32_t address, boost::uint32_t reserved, boost::uint32_t count);
		/**
		 * Write packets in proper sequence for a partial bitstream.
		 * @param outStream Bitstream file to be written
		 * @todo get actual partial bitstream sequence and write this function
		 */
		virtual bool writePacketsPartial(std::fstream& outStream);
		/**
		 * Write packets in proper sequence for a full bitstream.
		 * @param outStream Bitstream file to be written
		 * @todo test whether this actually works
		 */
		virtual bool writePackets(std::fstream& outStream);
		/**
		 * Write out partial bitstream.
		 * @param outStream bitstream file to be written
		 */
		virtual bool writePartialFrames(std::fstream& outStream);
	};
}
}
