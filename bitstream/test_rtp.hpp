/** \file
    \brief High-level, RTP regression test for input bit-stream class.
    \details This source file contains a function to test for regression of the
        input bit-stream class at a high level by decoding a canned RTP header.
    \copyright Copyright (C) 2013 Paul Long.
    \note Use, modification, and distribution is subject to the Boost Software
        License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
    \see http://www.boost.org/ for latest version.
    \see http://www.boost.org/libs/bitstream for documentation.
*/

BOOST_AUTO_TEST_CASE(test_rtp)
{
    struct {
        bool padding, marker;
        bitset<7> payloadType;
        uint16_t sequenceNumber;
        uint32_t timestamp, ssrcIdentifier;
        vector<uint32_t> csrcIdentifier;
        struct {
            bool present;
            uint16_t identifier;
            vector<uint8_t> contents;
        } extension;
    } rtp;

    const string rtpHeader("\x80\x08\xe7\x3c\x00\x00\x3c\x00\xde\xe0\xee\x8f");

    ibitstream bin(rtpHeader.c_str());
    static const bitset<2> version(0x2);
    bitset<4> csrcCount;
    uint16_t extensionLength = 0;

    bin >> version >> rtp.padding >> rtp.extension.present
        >> csrcCount >> rtp.marker >> rtp.payloadType
        >> rtp.sequenceNumber >> rtp.timestamp >> rtp.ssrcIdentifier
        >> setrepeat(csrcCount.to_ulong()) >> rtp.csrcIdentifier;
    if (rtp.extension.present) {
        bin >> rtp.extension.identifier >> extensionLength
            >> setrepeat(extensionLength * sizeof(uint32_t))
            >> rtp.extension.contents;
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
}