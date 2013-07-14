#include "bstream.h"
#include "stdint.h"
#include <bitset>
#include <vector>
using namespace std;
bool rtpTest()
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

    const string rtpHeader("\x8008e73c00003c00dee0ee8f");

    ibitstream bin(rtpHeader.c_str());
    static const bitset<2> version(0x2);
    bitset<4> csrcCount;
    uint16_t extensionLength;

    bin >> version >> rtp.padding >> rtp.extension.present
        >> csrcCount >> rtp.marker >> rtp.payloadType
        >> rtp.sequenceNumber >> rtp.timestamp >> rtp.ssrcIdentifier
        >> setrepeat(csrcCount.to_ulong()) >> rtp.csrcIdentifier;
    if (rtp.extension.present) {
        bin >> rtp.extension.identifier >> extensionLength
            >> setrepeat(extensionLength * sizeof(uint32_t))
            >> rtp.extension.contents;
    }

    obitstream bout;
    bout << version << rtp.padding << rtp.extension.present
        << csrcCount << rtp.marker << rtp.payloadType
        << rtp.sequenceNumber << rtp.timestamp << rtp.ssrcIdentifier
        << rtp.csrcIdentifier;
    if (rtp.extension.present) {
        bout << rtp.extension.identifier << extensionLength
            << rtp.extension.contents;
    }

    return bin.good() && bout.good() && rtpHeader == bout.str();
}

