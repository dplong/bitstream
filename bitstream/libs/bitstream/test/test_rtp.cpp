/** \file
    \brief High-level, RTP regression test for input bit-stream class.
    \details This header file contains a function to test for regression of the
        input bit-stream class at a high level by decoding a canned RTP header.
    \see http://www.boost.org/ for latest version.
    \see http://www.boost.org/libs/bitstream for documentation.

    Use, modification, and distribution is subject to the Boost Software
        License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/

#define BOOST_TEST_MAIN  "Bitstream Unit Tests"

#ifdef _MSC_VER
//#  pragma warning(disable : 4224)
#  pragma warning(disable : 4100)  // unreferenced formal parameter.  TDB  
#endif

#include <boost/test/included/unit_test.hpp>

#include <boost/bitstream/bstream.hpp>
#include <boost/bitstream/iomanip.hpp>

#include <sstream>
#include <deque>
#include <array>
#include <forward_list>

BOOST_AUTO_TEST_CASE(rtp)
{
    struct Rtp {
		const std::bitset<2> version = 0x2;
		bool padding, marker;
        std::bitset<7> payloadType;
		boost::uint16_t sequenceNumber;
		boost::uint32_t timestamp, ssrcIdentifier;
        std::vector<boost::uint32_t> csrcIdentifier;
        struct {
            bool present;
			boost::uint16_t identifier;
            std::vector<boost::uint8_t> contents;
        } extension;
    };

	const char rtpHeader[] = { '\x80', '\x08', '\xe7', '\x3c', '\x00', '\x00', '\x3c', '\x00', '\xde', '\xe0', '\xee', '\x8f' };

	{
		Rtp rtp;
		boost::bitstream::ibitstream bin(rtpHeader,
			sizeof rtpHeader * std::numeric_limits<unsigned char>::digits);

		std::bitset<4> csrcCount;
		boost::uint16_t extensionLength = 0;

		bin >> rtp.version;
		bin >> rtp.padding;
		bin >> rtp.extension.present;
		bin >> csrcCount;
		bin >> rtp.marker;
		bin >> rtp.payloadType;
		bin >> rtp.sequenceNumber;
		bin >> rtp.timestamp >> rtp.ssrcIdentifier;
		rtp.csrcIdentifier.resize(csrcCount.to_ulong());
		bin >> rtp.csrcIdentifier;
		if (rtp.extension.present) {
			bin >> rtp.extension.identifier >> extensionLength;
			rtp.extension.contents.resize(extensionLength * sizeof(boost::uint32_t));
			bin >> rtp.extension.contents;
		}
		BOOST_CHECK(bin);
		BOOST_CHECK(!rtp.padding);
		BOOST_CHECK(!rtp.extension.present);
		BOOST_CHECK(csrcCount.to_ulong() == 0);
		BOOST_CHECK(rtp.csrcIdentifier.empty());
		BOOST_CHECK(!rtp.marker);
		BOOST_CHECK(rtp.payloadType.to_ulong() == 8);
		BOOST_CHECK(rtp.sequenceNumber == 0xe73c);
		BOOST_CHECK(rtp.timestamp == 0x00003c00);
		BOOST_CHECK(rtp.ssrcIdentifier == 0xdee0ee8f);
		BOOST_CHECK(rtp.csrcIdentifier.empty());
	}

	{
		Rtp rtp;
		boost::bitstream::ibitstream bin(rtpHeader,
			sizeof rtpHeader * std::numeric_limits<unsigned char>::digits);

		std::bitset<4> csrcCount;
		boost::uint16_t extensionLength = 0;

		bin >> rtp.version >> rtp.padding >> rtp.extension.present
			>> csrcCount >> rtp.marker >> rtp.payloadType
			>> rtp.sequenceNumber >> rtp.timestamp >> rtp.ssrcIdentifier
			>> boost::bitstream::setrepeat(csrcCount.to_ulong()) >> rtp.csrcIdentifier;
		if (rtp.extension.present) {
			bin >> rtp.extension.identifier >> extensionLength
			>> boost::bitstream::setrepeat(extensionLength * sizeof(boost::uint32_t))
			>> rtp.extension.contents;
		}
		BOOST_CHECK(bin);
		BOOST_CHECK(!rtp.padding);
		BOOST_CHECK(!rtp.extension.present);
		BOOST_CHECK(csrcCount.to_ulong() == 0);
		BOOST_CHECK(rtp.csrcIdentifier.empty());
		BOOST_CHECK(!rtp.marker);
		BOOST_CHECK(rtp.payloadType.to_ulong() == 8);
		BOOST_CHECK(rtp.sequenceNumber == 0xe73c);
		BOOST_CHECK(rtp.timestamp == 0x00003c00);
		BOOST_CHECK(rtp.ssrcIdentifier == 0xdee0ee8f);
		BOOST_CHECK(rtp.csrcIdentifier.empty());

		{
			char buffer[1000];
			boost::bitstream::obitstream bout(buffer, sizeof buffer);

			bout << rtp.version << rtp.padding << rtp.extension.present
				<< std::bitset<4>(rtp.csrcIdentifier.size()) << rtp.marker
				<< rtp.payloadType << rtp.sequenceNumber << rtp.timestamp
				<< rtp.ssrcIdentifier << rtp.csrcIdentifier;
			if (rtp.extension.present) {
				bout << rtp.extension.identifier
					<< rtp.extension.contents.size() / sizeof(boost::uint32_t)
					<< rtp.extension.contents;
			}
			BOOST_CHECK(memcmp(bout.data(), rtpHeader, sizeof rtpHeader) == 0);
		}
	}
}

template<class T>
struct Bools
{
	Bools()
	{
		m_bools.clear();
		m_bools.push_back(true);
		m_bools.push_back(false);
		m_bools.push_back(true);
		m_bools.push_back(true);
		m_bools.push_back(false);
		m_bools.push_back(true);
		m_bools.push_back(true);
		m_bools.push_back(true);
		m_bools.push_back(false);
		m_bools.push_back(true);
	}

	T Get()
	{
		return m_bools;
	}

	T m_bools;
};

BOOST_AUTO_TEST_CASE(vector)
{
	Bools<std::vector<bool> > bools;
	const std::vector<bool> cvb(bools.Get());

	// Underrun of const vector<bool>. Read 10 of 16 bits.
	{
		char buffer[] = { '\xb7', '\x40' };
		boost::bitstream::ibitstream bin(buffer,
			sizeof buffer * std::numeric_limits<unsigned char>::digits);
		bin >> cvb;
		BOOST_CHECK(bin);
	}

	// Overrun of const vector<bool>. Attempt to read 10 of 8 bits.
	{
		const char buffer[] = { '\xb7' };
		boost::bitstream::ibitstream bin(buffer,
			sizeof buffer * std::numeric_limits<unsigned char>::digits);
		bin >> cvb;
		BOOST_CHECK(!bin);
		BOOST_CHECK(!bin.bad());
		BOOST_CHECK(bin.eof());
		BOOST_CHECK(bin.fail());
		BOOST_CHECK(!bin.good());
	}

	{
		std::vector<bool> vb(16);
		char buffer[] = { '\xb7', '\x40' };
		boost::bitstream::ibitstream bin(buffer,
			sizeof buffer * std::numeric_limits<unsigned char>::digits);
		bin >> vb;
		BOOST_CHECK(bin);
		BOOST_CHECK(!bin.bad());
		BOOST_CHECK(bin.eof());
		BOOST_CHECK(!bin.fail());
		BOOST_CHECK(!bin.good());
		BOOST_CHECK(vb.size() == 16);
		BOOST_CHECK(vb[0] == true);
		BOOST_CHECK(vb[1] == false);
		BOOST_CHECK(vb[2] == true);
		BOOST_CHECK(vb[3] == true);
		BOOST_CHECK(vb[4] == false);
		BOOST_CHECK(vb[5] == true);
		BOOST_CHECK(vb[6] == true);
		BOOST_CHECK(vb[7] == true);
		BOOST_CHECK(vb[8] == false);
		BOOST_CHECK(vb[9] == true);
		BOOST_CHECK(vb[10] == false);
		BOOST_CHECK(vb[11] == false);
		BOOST_CHECK(vb[12] == false);
		BOOST_CHECK(vb[13] == false);
		BOOST_CHECK(vb[14] == false);
		BOOST_CHECK(vb[15] == false);
	}

	{
		std::vector<bool> vb;
		char buffer[] = { '\xb7', '\x40' };
		boost::bitstream::ibitstream bin(buffer,
			sizeof buffer * std::numeric_limits<unsigned char>::digits);
		bin >> boost::bitstream::setrepeat(16) >> vb;
		BOOST_CHECK(bin);
		BOOST_CHECK(!bin.bad());
		BOOST_CHECK(bin.eof());
		BOOST_CHECK(!bin.fail());
		BOOST_CHECK(!bin.good());
		BOOST_CHECK(vb.size() == 16);
		BOOST_CHECK(vb[0] == true);
		BOOST_CHECK(vb[1] == false);
		BOOST_CHECK(vb[2] == true);
		BOOST_CHECK(vb[3] == true);
		BOOST_CHECK(vb[4] == false);
		BOOST_CHECK(vb[5] == true);
		BOOST_CHECK(vb[6] == true);
		BOOST_CHECK(vb[7] == true);
		BOOST_CHECK(vb[8] == false);
		BOOST_CHECK(vb[9] == true);
		BOOST_CHECK(vb[10] == false);
		BOOST_CHECK(vb[11] == false);
		BOOST_CHECK(vb[12] == false);
		BOOST_CHECK(vb[13] == false);
		BOOST_CHECK(vb[14] == false);
		BOOST_CHECK(vb[15] == false);
	}

	// Exact setrepeat read of vector<boost::uint16_t>. Reach eof before fail.
	{
		std::vector<boost::uint16_t> vu;
		char buffer[] = { '\x12', '\x34', '\x56', '\x78', '\x9a', '\xbc', '\xde', '\xf0' };
		boost::bitstream::ibitstream bin(buffer,
			sizeof buffer * std::numeric_limits<unsigned char>::digits);
		bin >> boost::bitstream::setrepeat(4);
		bin >> vu;
		BOOST_CHECK(bin);
		BOOST_CHECK(!bin.bad());
		BOOST_CHECK(bin.eof());
		BOOST_CHECK(!bin.fail());
		BOOST_CHECK(!bin.good());
		BOOST_CHECK(vu.size() == 4);
		BOOST_CHECK(vu[0] == 0x1234);
		BOOST_CHECK(vu[1] == 0x5678);
		BOOST_CHECK(vu[2] == 0x9abc);
		BOOST_CHECK(vu[3] == 0xdef0);
	}

	// Exact setrepeat read of vector<boost::uint16_t>. Reach eof before fail.
	{
		std::vector<boost::uint16_t> vu;
		char buffer[] = { '\x12', '\x34', '\x56', '\x78', '\x9a', '\xbc', '\xde', '\xf0', '\x12' };
		boost::bitstream::ibitstream bin(buffer,
			sizeof buffer * std::numeric_limits<unsigned char>::digits);
		bin >> boost::bitstream::setrepeat(4) >> vu;
		BOOST_CHECK(bin);
		BOOST_CHECK(!bin.bad());
		BOOST_CHECK(!bin.eof());
		BOOST_CHECK(!bin.fail());
		BOOST_CHECK(bin.good());
		BOOST_CHECK(vu.size() == 4);
		BOOST_CHECK(vu[0] == 0x1234);
		BOOST_CHECK(vu[1] == 0x5678);
		BOOST_CHECK(vu[2] == 0x9abc);
		BOOST_CHECK(vu[3] == 0xdef0);
	}

	// Overrun of setrepeat vector<boost::uint16_t> by boost::uint16_t. Reach eof same time as fail.
	{
		std::vector<boost::uint16_t> vu;
		char buffer[] = { '\x12', '\x34', '\x56', '\x78', '\x9a', '\xbc', '\xde', '\xf0' };
		boost::bitstream::ibitstream bin(buffer,
			sizeof buffer * std::numeric_limits<unsigned char>::digits);
		bin >> boost::bitstream::setrepeat(5) >> vu;
		BOOST_CHECK(!bin);
		BOOST_CHECK(!bin.bad());
		BOOST_CHECK(bin.eof());
		BOOST_CHECK(bin.fail());
		BOOST_CHECK(!bin.good());
		BOOST_CHECK(vu.size() == 5);
		BOOST_CHECK(vu[0] == 0x1234);
		BOOST_CHECK(vu[1] == 0x5678);
		BOOST_CHECK(vu[2] == 0x9abc);
		BOOST_CHECK(vu[3] == 0xdef0);
	}

	// Overrun of setrepeat vector<boost::uint16_t> by one byte. Reach eof same time as fail.
	{
		std::vector<boost::uint16_t> vu;
		char buffer[] = { '\x12', '\x34', '\x56', '\x78', '\x9a', '\xbc', '\xde', '\xf0', '\x12' };
		boost::bitstream::ibitstream bin(buffer,
			sizeof buffer * std::numeric_limits<unsigned char>::digits);
		bin >> boost::bitstream::setrepeat(5) >> vu;
		BOOST_CHECK(!bin);
		BOOST_CHECK(!bin.bad());
		BOOST_CHECK(bin.eof());
		BOOST_CHECK(bin.fail());
		BOOST_CHECK(!bin.good());
		BOOST_CHECK(vu.size() == 5);
		BOOST_CHECK(vu[0] == 0x1234);
		BOOST_CHECK(vu[1] == 0x5678);
		BOOST_CHECK(vu[2] == 0x9abc);
		BOOST_CHECK(vu[3] == 0xdef0);
	}

	// Exact sized read of vector<boost::uint16_t>. Reach eof before fail.
	{
		std::vector<boost::uint16_t> vu(4);
		char buffer[] = { '\x12', '\x34', '\x56', '\x78', '\x9a', '\xbc', '\xde', '\xf0' };
		boost::bitstream::ibitstream bin(buffer,
			sizeof buffer * std::numeric_limits<unsigned char>::digits);
		bin >> vu;
		BOOST_CHECK(bin);
		BOOST_CHECK(!bin.bad());
		BOOST_CHECK(bin.eof());
		BOOST_CHECK(!bin.fail());
		BOOST_CHECK(!bin.good());
		BOOST_CHECK(vu.size() == 4);
		BOOST_CHECK(vu[0] == 0x1234);
		BOOST_CHECK(vu[1] == 0x5678);
		BOOST_CHECK(vu[2] == 0x9abc);
		BOOST_CHECK(vu[3] == 0xdef0);
	}

	// Exact sized read of vector<boost::uint16_t>. Reach eof before fail.
	{
		std::vector<boost::uint16_t> vu(4);
		char buffer[] = { '\x12', '\x34', '\x56', '\x78', '\x9a', '\xbc', '\xde', '\xf0', '\x12' };
		boost::bitstream::ibitstream bin(buffer,
			sizeof buffer * std::numeric_limits<unsigned char>::digits);
		bin >> vu;
		BOOST_CHECK(bin);
		BOOST_CHECK(!bin.bad());
		BOOST_CHECK(!bin.eof());
		BOOST_CHECK(!bin.fail());
		BOOST_CHECK(bin.good());
		BOOST_CHECK(vu.size() == 4);
		BOOST_CHECK(vu[0] == 0x1234);
		BOOST_CHECK(vu[1] == 0x5678);
		BOOST_CHECK(vu[2] == 0x9abc);
		BOOST_CHECK(vu[3] == 0xdef0);
	}

	// Overrun of sized vector<boost::uint16_t> by boost::uint16_t. Reach eof same time as fail.
	{
		std::vector<boost::uint16_t> vu(5);
		char buffer[] = { '\x12', '\x34', '\x56', '\x78', '\x9a', '\xbc', '\xde', '\xf0' };
		boost::bitstream::ibitstream bin(buffer,
			sizeof buffer * std::numeric_limits<unsigned char>::digits);
		bin >> vu;
		BOOST_CHECK(!bin);
		BOOST_CHECK(!bin.bad());
		BOOST_CHECK(bin.eof());
		BOOST_CHECK(bin.fail());
		BOOST_CHECK(!bin.good());
		BOOST_CHECK(vu.size() == 5);
		BOOST_CHECK(vu[0] == 0x1234);
		BOOST_CHECK(vu[1] == 0x5678);
		BOOST_CHECK(vu[2] == 0x9abc);
		BOOST_CHECK(vu[3] == 0xdef0);
	}

	// Overrun of sized vector<boost::uint16_t> by one byte. Reach eof same time as fail.
	{
		std::vector<boost::uint16_t> vu(5);
		char buffer[] = { '\x12', '\x34', '\x56', '\x78', '\x9a', '\xbc', '\xde', '\xf0', '\x12' };
		boost::bitstream::ibitstream bin(buffer,
			sizeof buffer * std::numeric_limits<unsigned char>::digits);
		bin >> vu;
		BOOST_CHECK(!bin);
		BOOST_CHECK(!bin.bad());
		BOOST_CHECK(bin.eof());
		BOOST_CHECK(bin.fail());
		BOOST_CHECK(!bin.good());
		BOOST_CHECK(vu.size() == 5);
		BOOST_CHECK(vu[0] == 0x1234);
		BOOST_CHECK(vu[1] == 0x5678);
		BOOST_CHECK(vu[2] == 0x9abc);
		BOOST_CHECK(vu[3] == 0xdef0);
	}
}

BOOST_AUTO_TEST_CASE(list)
{
	Bools<std::list<bool> > bools;
	const std::list<bool> lb(bools.Get());

	// Underrun. Read 10 of 16 bits.
	{
		const char buffer[] = { '\xb7', '\x40' };
		boost::bitstream::ibitstream bin(buffer,
			sizeof buffer * std::numeric_limits<unsigned char>::digits);
		bin >> lb;
		BOOST_CHECK(bin);
	}

	// Overrun. Attempt to read 10 of 8 bits.
	{
		const char buffer[] = { '\xb7' };
		boost::bitstream::ibitstream bin(buffer,
			sizeof buffer * std::numeric_limits<unsigned char>::digits);
		bin >> lb;
		BOOST_CHECK(!bin);
		BOOST_CHECK(!bin.bad());
		BOOST_CHECK(bin.eof());
		BOOST_CHECK(bin.fail());
		BOOST_CHECK(!bin.good());
	}
}

BOOST_AUTO_TEST_CASE(deque)
{
	Bools<std::deque<bool> > bools;
	const std::deque<bool> db(bools.Get());

	// Underrun. Read 10 of 16 bits.
	{
		const char buffer[] = { '\xb7', '\x40' };
		boost::bitstream::ibitstream bin(buffer,
			sizeof buffer * std::numeric_limits<unsigned char>::digits);
		bin >> db;
		BOOST_CHECK(bin);
	}

	// Overrun. Attempt to read 10 of 8 bits.
	{
		const char buffer[] = { '\xb7' };
		boost::bitstream::ibitstream bin(buffer,
			sizeof buffer * std::numeric_limits<unsigned char>::digits);
		bin >> db;
		BOOST_CHECK(!bin);
		BOOST_CHECK(!bin.bad());
		BOOST_CHECK(bin.eof());
		BOOST_CHECK(bin.fail());
		BOOST_CHECK(!bin.good());
	}
}

template<class T>
struct ABools
{
	ABools()
	{
		m_bools[0] = true;
		m_bools[1] = false;
		m_bools[2] = true;
		m_bools[3] = true;
		m_bools[4] = false;
		m_bools[5] = true;
		m_bools[6] = true;
		m_bools[7] = true;
		m_bools[8] = false;
		m_bools[9] = true;
	}

	T Get()
	{
		return m_bools;
	}

	T m_bools;
};

BOOST_AUTO_TEST_CASE(array)
{
	ABools<std::array<bool, 10> > bools;
	const std::array<bool, 10> ab(bools.Get());

	// Underrun. Read 10 of 16 bits.
	{
		const char buffer[] = { '\xb7', '\x40' };
		boost::bitstream::ibitstream bin(buffer, sizeof
			buffer * std::numeric_limits<unsigned char>::digits);
		bin >> ab;
		BOOST_CHECK(bin);
	}

	// Overrun. Attempt to read 10 of 8 bits.
	{
		const char buffer[] = { '\xb7' };
		boost::bitstream::ibitstream bin(buffer,
			sizeof buffer * std::numeric_limits<unsigned char>::digits);
		bin >> ab;
		BOOST_CHECK(!bin);
		BOOST_CHECK(!bin.bad());
		BOOST_CHECK(bin.eof());
		BOOST_CHECK(bin.fail());
		BOOST_CHECK(!bin.good());
	}

	{
		std::array<bool, 16> vb;
		char buffer[] = { '\xb7', '\x40' };
		boost::bitstream::ibitstream bin(buffer,
			sizeof buffer * std::numeric_limits<unsigned char>::digits);
		bin >> vb;
		BOOST_CHECK(bin);
		BOOST_CHECK(!bin.bad());
		BOOST_CHECK(bin.eof());
		BOOST_CHECK(!bin.fail());
		BOOST_CHECK(!bin.good());
		BOOST_CHECK(vb.size() == 16);
		BOOST_CHECK(vb[0] == true);
		BOOST_CHECK(vb[1] == false);
		BOOST_CHECK(vb[2] == true);
		BOOST_CHECK(vb[3] == true);
		BOOST_CHECK(vb[4] == false);
		BOOST_CHECK(vb[5] == true);
		BOOST_CHECK(vb[6] == true);
		BOOST_CHECK(vb[7] == true);
		BOOST_CHECK(vb[8] == false);
		BOOST_CHECK(vb[9] == true);
		BOOST_CHECK(vb[10] == false);
		BOOST_CHECK(vb[11] == false);
		BOOST_CHECK(vb[12] == false);
		BOOST_CHECK(vb[13] == false);
		BOOST_CHECK(vb[14] == false);
		BOOST_CHECK(vb[15] == false);
	}
}

template<class T>
struct FLBools
{
	FLBools()
	{
		m_bools.clear();
		m_bools.push_front(true);
		m_bools.push_front(false);
		m_bools.push_front(true);
		m_bools.push_front(true);
		m_bools.push_front(false);
		m_bools.push_front(true);
		m_bools.push_front(true);
		m_bools.push_front(true);
		m_bools.push_front(false);
		m_bools.push_front(true);
		m_bools.reverse();
	}

	T Get()
	{
		return m_bools;
	}

	T m_bools;
};

BOOST_AUTO_TEST_CASE(forward_list)
{
	FLBools<std::forward_list<bool> > bools;
	const std::forward_list<bool> flb(bools.Get());

	// Underrun. Read 10 of 16 bits.
	{
		const char buffer[] = { '\xb7', '\x40' };
		boost::bitstream::ibitstream bin(buffer,
			sizeof buffer * std::numeric_limits<unsigned char>::digits);
		bin >> flb;
		BOOST_CHECK(bin);
	}

	// Overrun. Attempt to read 10 of 8 bits.
	{
		const char buffer[] = { '\xb7' };
		boost::bitstream::ibitstream bin(buffer,
			sizeof buffer * std::numeric_limits<unsigned char>::digits);
		bin >> flb;
		BOOST_CHECK(!bin);
		BOOST_CHECK(!bin.bad());
		BOOST_CHECK(bin.eof());
		BOOST_CHECK(bin.fail());
		BOOST_CHECK(!bin.good());
	}
}

BOOST_AUTO_TEST_CASE(operator_void_pointer)
{
	{
		boost::bitstream::ibitstream bin;
		void *vp = bin;
		BOOST_CHECK(vp != NULL);
		BOOST_CHECK((void *)bin);
		BOOST_CHECK(static_cast<void *>(bin));
	}
}