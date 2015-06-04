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

#include <cstdint>

BOOST_AUTO_TEST_CASE(test_rtp)
{
    struct Rtp {
		const std::bitset<2> version = 0x2;
		bool padding, marker;
        std::bitset<7> payloadType;
        uint16_t sequenceNumber;
        uint32_t timestamp, ssrcIdentifier;
        std::vector<uint32_t> csrcIdentifier;
        struct {
            bool present;
            uint16_t identifier;
            std::vector<uint8_t> contents;
        } extension;
    } rtp;

    const std::string rtpHeader("\x80\x08\xe7\x3c\x00\x00\x3c\x00\xde\xe0\xee\x8f");

    boost::bitstream::ibitstream bin(rtpHeader.c_str());

	std::bitset<4> csrcCount;
    uint16_t extensionLength = 0;

	bin >> rtp.version >> rtp.padding >> rtp.extension.present
		>> csrcCount >> rtp.marker >> rtp.payloadType
		>> rtp.sequenceNumber >> rtp.timestamp >> rtp.ssrcIdentifier;
	rtp.csrcIdentifier.resize(csrcCount.to_ulong());
	bin >> rtp.csrcIdentifier;
    if (rtp.extension.present) {
		bin >> rtp.extension.identifier >> extensionLength;
		rtp.extension.contents.resize(extensionLength * sizeof(uint32_t));
        bin >> rtp.extension.contents;
    }
    BOOST_CHECK(bin);
    BOOST_CHECK(!rtp.padding);
    BOOST_CHECK(!rtp.extension.present);
    BOOST_CHECK(csrcCount.to_ulong() == 0);
    BOOST_CHECK(rtp.csrcIdentifier.empty());
    BOOST_CHECK(!rtp.marker);
    BOOST_CHECK(rtp.payloadType.to_ulong() == 8);
    BOOST_CHECK(rtp.sequenceNumber == 59196);
    BOOST_CHECK(rtp.timestamp == 13421772);
    BOOST_CHECK(rtp.ssrcIdentifier == 3435973836);
    BOOST_CHECK(rtp.csrcIdentifier.empty());

	boost::bitstream::obitstream bout;
#if 0
	bout << rtp.version << rtp.padding << rtp.extension.present
		<< std::bitset<4>(rtp.csrcIdentifier.size()) << rtp.marker << rtp.payloadType
		<< rtp.sequenceNumber << rtp.timestamp << rtp.ssrcIdentifier
		<< rtp.csrcIdentifier;
	if (rtp.extension.present) {
		bout << rtp.extension.identifier << rtp.extension.contents.size() / sizeof(uint32_t)
			<< rtp.extension.contents;
	}
	BOOST_CHECK(memcmp(bout.data(), rtpHeader.c_str(), rtpHeader.size()) == 0);
#endif
}
