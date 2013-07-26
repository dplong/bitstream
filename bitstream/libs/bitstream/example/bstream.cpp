#pragma region Includes
////////////////////////////////////////////////////////////////////////////////

#include "bstream.h"

#pragma endregion
#pragma region Constants
////////////////////////////////////////////////////////////////////////////////

#pragma endregion
#pragma region Typedefs
////////////////////////////////////////////////////////////////////////////////

#pragma endregion
#pragma region bitbuf
////////////////////////////////////////////////////////////////////////////////

bitbuf::
bitbuf(std::ios_base::openmode which) :
    m_buffer(NULL), m_gptr(0), m_egptr(INT_MAX), m_eback(0)
{
    assert((which & std::ios_base::app) == 0); // Output not support, so can't append.
    assert((which & std::ios_base::ate) == 0); // Output not support, so can't append each time.
    assert((which & std::ios_base::out) == 0); // Output not currently supported.
    assert((which & std::ios_base::trunc) == 0); // Truncate not currently supported.
}

bitbuf::
bitbuf(signed char *buffer, std::streamsize size, std::ios_base::openmode which) :
    m_buffer(reinterpret_cast<unsigned char *>(buffer)), m_gptr(0), m_egptr(size), m_eback(0)
{
    assert(buffer != NULL);
    assert(size >= 0);
    assert((which & std::ios_base::app) == 0); // Output not support, so can't append.
    assert((which & std::ios_base::ate) == 0); // Output not support, so can't append each time.
    assert((which & std::ios_base::out) == 0); // Output not currently supported.
    assert((which & std::ios_base::trunc) == 0); // Truncate not currently supported.
}

bitbuf::
bitbuf(unsigned char *buffer, std::streamsize size, std::ios_base::openmode which) :
    m_buffer(buffer), m_gptr(0), m_egptr(size), m_eback(0)
{
    assert(buffer != NULL);
    assert(size >= 0);
    assert((which & std::ios_base::app) == 0); // Output not support, so can't append.
    assert((which & std::ios_base::ate) == 0); // Output not support, so can't append each time.
    assert((which & std::ios_base::out) == 0); // Output not currently supported.
    assert((which & std::ios_base::trunc) == 0); // Truncate not currently supported.
}

bitbuf::
bitbuf(const signed char *buffer, std::streamsize size, std::ios_base::openmode which) :
    m_buffer(reinterpret_cast<unsigned char *>(const_cast<signed char *>(buffer))),
    m_gptr(0), m_egptr(size), m_eback(0)
{
    assert(buffer != NULL);
    assert(size >= 0);
    assert((which & std::ios_base::app) == 0); // Output not support, so can't append.
    assert((which & std::ios_base::ate) == 0); // Output not support, so can't append each time.
    assert((which & std::ios_base::out) == 0); // Output not currently supported.
    assert((which & std::ios_base::trunc) == 0); // Truncate not currently supported.
}

bitbuf::
bitbuf(const unsigned char *buffer, std::streamsize size, std::ios_base::openmode which) :
    m_buffer(const_cast<unsigned char *>(buffer)), m_gptr(0), m_egptr(size), m_eback(0)
{
    assert(buffer != NULL);
    assert(size >= 0);
    assert((which & std::ios_base::app) == 0); // Output not support, so can't append.
    assert((which & std::ios_base::ate) == 0); // Output not support, so can't append each time.
    assert((which & std::ios_base::out) == 0); // Output not currently supported.
    assert((which & std::ios_base::trunc) == 0); // Truncate not currently supported.
}

bitbuf::
~bitbuf()
{
    // Do nothing.
}

std::streamsize
bitbuf::
in_avail() const
{
    return egptr() - gptr();
}

std::streampos
bitbuf::
pubseekoff(std::streamoff offset, std::ios_base::seekdir way)
{
    return seekoff(offset, way);
}

std::streampos
bitbuf::
pubseekpos(std::streampos position)
{
    assert(position >= 0);

    return seekpos(position);
}

bitbuf *
bitbuf::
pubsetbuf(unsigned char *buffer)
{
    assert(buffer != NULL);

    return pubsetbuf(buffer, npos);
}

bitbuf *
bitbuf::
pubsetbuf(unsigned char *buffer, std::streamsize size, std::streampos position)
{
    assert(buffer != NULL);
    assert(size >= 0);
    assert(position >= 0);

    return setbuf(buffer, size, position);
}

int
bitbuf::
pubsync()
{
    return sync();
}

bool
bitbuf::
sbumpb(bitfield &value)
{
    return sgetn(value, 1) == 1;
}

bool
bitbuf::
sgetb(bitfield &value)
{
    bool okay = sbumpb(value);

    if (!okay)
    {
        pubseekoff(-1, std::ios_base::cur);
    }

    return okay;
}

std::streamsize
bitbuf::
sgetn(bitfield &value, std::streamsize size)
{
    assert(size >= 0);

    return xsgetn(value, size);
}

bool
bitbuf::
snextb(bitfield &value)
{
    bool okay;

    if (pubseekoff(1, std::ios_base::cur) == bitbuf::npos)
    {
        okay = false;
    }
    else
    {
        okay = sgetb(value);
    }

    return okay;
}

bool
bitbuf::
sungetb(bitfield &value)
{
    bool okay;

    if (pubseekoff(-1, std::ios_base::cur) == bitbuf::npos)
    {
        okay = false;
    }
    else
    {
        okay = sgetb(value);
    }

    return okay;
}

std::streampos
bitbuf::
gptr() const
{
    return m_gptr;
}

std::streampos
bitbuf::
egptr() const
{
    return m_egptr;
}

std::streampos
bitbuf::
eback() const
{
    return m_eback;
}

void
bitbuf::
setg(unsigned char *buffer, std::streampos gbeg, std::streampos gnext, std::streampos gend)
{
    assert(buffer != NULL);
    assert(gbeg >= 0);
    assert(gnext >= 0);
    assert(gend >= 0);
    assert(gbeg <= gnext);
    assert(gnext <= gend);

    m_buffer = buffer;
    m_eback = gbeg;
    m_gptr = gnext;
    m_egptr = gend;
}

void
bitbuf::
gbump(std::streamoff offset)
{
    seekoff(offset, std::ios_base::cur);
}

std::streampos
bitbuf::
seekpos(std::streampos position)
{
    assert(position >= 0);

    return AssureValidGetPointer(position);
}

std::streampos
bitbuf::
seekoff(std::streamoff offset, std::ios_base::seekdir way)
{
    decltype(seekoff(0,std::ios_base::cur)) newPosition;

    switch (way)
    {
    case std::ios_base::beg:
        newPosition = AssureValidGetPointer(eback() + offset);
        break;

    case std::ios_base::cur:
        newPosition = AssureValidGetPointer(gptr() + offset);
        break;

    case std::ios_base::end:
        newPosition = AssureValidGetPointer(egptr() + offset);
        break;

    default:
        assert(false);
        newPosition = npos;
        break;

    }

    return newPosition;
}

std::streampos
bitbuf::
AssureValidGetPointer(std::streampos position)
{
    assert(position >= 0);

    decltype(AssureValidGetPointer(0)) newPosition;

    if (position < eback() || position > egptr())
    {
        newPosition = npos;
    }
    else
    {
        m_gptr = newPosition = position;
    }

    assert(newPosition == npos ||
        (newPosition >= eback() && newPosition <= egptr()));

    return newPosition;
}

bitbuf *
bitbuf::
setbuf(unsigned char *buffer, std::streamsize size, std::streampos position)
{
    assert(buffer != NULL);
    assert(size >= 0);
    assert(position >= 0);

    setg(buffer, position, position, size);

    return this;
}

int
bitbuf::
sync()
{
    return -1;
}

std::streamsize
bitbuf::
xsgetn(bitfield &value, std::streamsize size)
{
    assert(size >= 0);

    decltype(size) bitsGotten = 0;

    if (size > 0 && size <= egptr() - gptr())
    {
        // Generate "right-justified" mask, e.g., 0000000000111111.
        std::remove_reference<decltype(value)>::type mask = 0;
        mask = ~mask;
        // (Had to break up shift into two parts because shifting the width
        // of the integral all at once had no effect.)
        mask <<= (size - 1);
        mask <<= 1;
        mask = ~mask;

        // Shift over so mask appears at correct location within integral,
        // e.g., 0000000111111000.
        size_t intraByteBitOffset = m_gptr % CHAR_BIT; // Offset in current byte.
        size_t shiftAmount = (CHAR_BIT - ((size + intraByteBitOffset) % CHAR_BIT)) % CHAR_BIT;

        size_t oldShiftAmount = (static_cast<unsigned>(CHAR_BIT) * 2 -
            intraByteBitOffset - size) % CHAR_BIT;
        if (oldShiftAmount > CHAR_BIT) {
            oldShiftAmount += CHAR_BIT;
        }

        assert(shiftAmount == oldShiftAmount);

        mask <<= shiftAmount;

        // Accumulate current value of each byte involved in bit field.
        size_t byteCount = (static_cast<size_t>(size) + shiftAmount + CHAR_BIT - 1) / CHAR_BIT;
        std::remove_reference<decltype(value)>::type preMaskValue = 0;
        for (decltype(byteCount) i = 0; i < byteCount; ++i)
        {
            preMaskValue <<= static_cast<unsigned>(CHAR_BIT);
            preMaskValue |= currentByte()[i];
        }

        // Apply mask and right justify. (Consumer wants the normalized
        // value, not the value as it occurs in the buffer.)
        value = preMaskValue & mask;
        value >>= shiftAmount;

        // Advance bit pointer past field in preparation for next access.
        gbump(size);

        bitsGotten = size;
    }

    return bitsGotten;
}

unsigned char *
bitbuf::
currentByte() const
{
    return &m_buffer[m_gptr / CHAR_BIT];
}

///
/// Represents both an unbounded number of bits and an attempt to move past bit-stream bounds.
///
/// @note A consumer might encounter this value where member functions of
/// this class return a value of streamsize or std::streampos type.
const std::streampos bitbuf::npos = static_cast<std::streampos>(-1);

#pragma endregion
#pragma region ibitstream
////////////////////////////////////////////////////////////////////////////////

ibitstream::
ibitstream(std::ios_base::openmode which) :
    m_bitbuf(which), m_state(0), m_gcount(0), m_gvalue(0), m_repeat(0)
{
    assert((which & std::ios_base::app) == 0); // Append to input? Huh?
    assert((which & std::ios_base::ate) == 0); // Input at end of stream? Huh?
    assert((which & std::ios_base::out) == 0); // Output not currently supported.
    assert((which & std::ios_base::trunc) == 0); // Truncate not currently supported.
}

ibitstream::
ibitstream(signed char *buffer, std::streamsize size, std::ios_base::openmode which) :
    m_bitbuf(buffer, size, which), m_state(0), m_gcount(0), m_gvalue(0),
    m_repeat(0)
{
    assert(buffer != NULL);
    assert(size >= 0);
    assert((which & std::ios_base::app) == 0); // Append to input? Huh?
    assert((which & std::ios_base::ate) == 0); // Input at end of stream? Huh?
    assert((which & std::ios_base::out) == 0); // Output not currently supported.
    assert((which & std::ios_base::trunc) == 0); // Truncate not currently supported.
}

ibitstream::
ibitstream(unsigned char *buffer, std::streamsize size, std::ios_base::openmode which) :
    m_bitbuf(buffer, size, which), m_state(0), m_gcount(0), m_gvalue(0),
    m_repeat(0)
{
    assert(buffer != NULL);
    assert(size >= 0);
    assert((which & std::ios_base::app) == 0); // Append to input? Huh?
    assert((which & std::ios_base::ate) == 0); // Input at end of stream? Huh?
    assert((which & std::ios_base::out) == 0); // Output not currently supported.
    assert((which & std::ios_base::trunc) == 0); // Truncate not currently supported.
}

ibitstream::
ibitstream(const signed char *buffer, std::streamsize size, std::ios_base::openmode which) :
    m_bitbuf(buffer, size, which), m_state(0), m_gcount(0), m_gvalue(0),
    m_repeat(0)
{
    assert(buffer != NULL);
    assert(size >= 0);
    assert((which & std::ios_base::app) == 0); // Append to input? Huh?
    assert((which & std::ios_base::ate) == 0); // Input at end of stream? Huh?
    assert((which & std::ios_base::out) == 0); // Output not currently supported.
    assert((which & std::ios_base::trunc) == 0); // Truncate not currently supported.
}

ibitstream::
ibitstream(const unsigned char *buffer, std::streamsize size, std::ios_base::openmode which) :
    m_bitbuf(buffer, size, which), m_state(0), m_gcount(0), m_gvalue(0),
    m_repeat(0)
{
    assert(buffer != NULL);
    assert(size >= 0);
    assert((which & std::ios_base::app) == 0); // Append to input? Huh?
    assert((which & std::ios_base::ate) == 0); // Input at end of stream? Huh?
    assert((which & std::ios_base::out) == 0); // Output not currently supported.
    assert((which & std::ios_base::trunc) == 0); // Truncate not currently supported.
}

ibitstream::
~ibitstream()
{
    // Do nothing.
}

bitfield
ibitstream::
get()
{
    if (!m_bitbuf.sbumpb(m_gvalue))
    {
        failbit();
        eofbit();
        m_gcount = 0;
    }
    else
    {
        m_gcount = 1;
    }

    return m_gvalue;
}

ibitstream &
ibitstream::
ignore(std::streamsize bits)
{
    assert(bits >= 0);

    if (m_bitbuf.pubseekoff(bits, std::ios_base::cur) == bitbuf::npos)
    {
        eofbit();
        m_gcount = 0;
    }
    else
    {
        m_gcount = bits;
    }

    return *this;
}

ibitstream &
ibitstream::
aligng(size_t bit)
{
    assert(bit > 0);
    if (good() && bit > 0)
    {
        seekg(((static_cast<size_t>(tellg()) + bit - 1) / bit) * bit);
    }

    m_gcount = 0;

    return *this;
}

bitfield
ibitstream::
peek()
{
    if (!m_bitbuf.sgetb(m_gvalue))
    {
        eofbit();
        m_gcount = 0;
    }
    else
    {
        m_gcount = 1;
    }

    return m_gvalue;
}

ibitstream &
ibitstream::
read(bitfield &value, std::streamsize bits)
{
    assert(bits >= 0);

    decltype(bits) bitsRead = m_bitbuf.sgetn(value, bits);
    if (bitsRead != bits)
    {
        failbit();
        eofbit();
        m_gcount = 0;
        value = 0;
    }
    else
    {
        m_gcount = bitsRead;
    }

    m_gvalue = value;

    return *this;
}

ibitstream &
ibitstream::
readsome(bitfield &value, std::streamsize bits)
{
    assert(bits >= 0);

    return read(value, bits);
}

ibitstream &
ibitstream::
seekg(std::streampos position)
{
    assert(position >= 0);

    if (m_bitbuf.pubseekpos(position) == bitbuf::npos)
    {
        failbit();
    }

    return *this;
}

ibitstream &
ibitstream::
seekg(std::streamoff offset, std::ios_base::seek_dir dir)
{
    if (m_bitbuf.pubseekoff(offset, dir) == bitbuf::npos)
    {
        failbit();
    }

    return *this;
}

int
ibitstream::
sync()
{
    return m_bitbuf.pubsync();
}

std::streampos
ibitstream::
tellg()
{
    return m_bitbuf.pubseekoff(0, std::ios_base::cur);
}

ibitstream &
ibitstream::
unget()
{
    m_gcount = 0;

    if (static_cast<decltype(bitbuf::eof)>(m_bitbuf.pubseekoff(-1, std::ios_base::cur)) == bitbuf::eof)
    {
        failbit();
    }

    return *this;
}

std::ios_base::iostate
ibitstream::
rdstate() const
{
    return m_state;
}

void
ibitstream::
clear(std::ios_base::iostate state)
{
    m_state = state;
}

void
ibitstream::
setstate(std::ios_base::iostate state)
{
    m_state |= state;
}

bool
ibitstream::
bad() const
{
    return (rdstate() & std::ios_base::badbit) != 0;
}

bool
ibitstream::
fail() const
{
    return (rdstate() & std::ios_base::failbit) != 0 || eof();
}

bool
ibitstream::
eof() const
{
    return (rdstate() & std::ios_base::eofbit) != 0;
}

bool
ibitstream::
good() const
{
    return rdstate() == 0;
}

std::streamsize
ibitstream::
gcount() const
{
    return m_gcount;
}

bitfield
ibitstream::
gvalue() const
{
    return m_gvalue;
}

bool
ibitstream::
operator!() const
{
    return fail();
}

ibitstream &
ibitstream::
setrepeat(size_t repeat)
{
    m_repeat = repeat;

    return *this;
}

void
ibitstream::
badbit()
{
    m_state |= std::ios_base::badbit;
}


void
ibitstream::
failbit()
{
    m_state |= std::ios_base::failbit;
}

void
ibitstream::
eofbit()
{
    m_state |= std::ios_base::eofbit;
}

bool
ibitstream::
alignedg(size_t bit)
{
    return tellg() % bit == 0;
}

#pragma endregion
#pragma region Manipulators
////////////////////////////////////////////////////////////////////////////////

//@{
setrepeat::
setrepeat(size_t repeat) : m_repeat(repeat)
{
    // Do nothing.
}

ibitstream &
setrepeat::
operator()(ibitstream &ibs) const
{
    return ibs.setrepeat(m_repeat);
}

ibitstream &
operator>>(ibitstream &ibs, setrepeat repeat)
{
    return repeat(ibs);
}
//@}

//@{
ignore::
ignore(size_t bits) : m_bits(bits)
{
    // Do nothing.
}

ibitstream &
ignore::
operator()(ibitstream &ibs) const
{
    return ibs.ignore(m_bits);
}

ibitstream &operator>>(ibitstream &ibs, ignore skip)
{
    return skip(ibs);
}
//@}

//@{
aligng::
aligng(size_t bits) : m_bits(bits)
{
    assert(bits > 0);
}

ibitstream &
aligng::
operator()(ibitstream &ibs) const
{
    return ibs.aligng(m_bits);
}

ibitstream &operator>>(ibitstream &ibs, aligng align)
{
    return align(ibs);
}
//@}

#pragma endregion
#pragma region Operator overloads
////////////////////////////////////////////////////////////////////////////////

ibitstream &operator>>(ibitstream &ibs, bool &b)
{
    // Extract bit; must be equal to b
    bitfield value;
    ibs.read(value, 1);
    b = value != 0;

    return ibs;
}

ibitstream &operator>>(ibitstream &ibs, const bool &b)
{
    // Extract bit; must be equal to b
    bitfield value;
    ibs.read(value, 1);
    if (b != (value == 0 ? false : true))
    {
        ibs.badbit();
    }

    return ibs;
}

#pragma endregion
