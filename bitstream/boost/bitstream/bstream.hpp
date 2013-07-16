// Boost general library bstream.hpp header file //////////////////////////////
//
// Copyright (C) 2013 Paul Long.
//
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/ for latest version.
//
// See http://www.boost.org/libs/bitstream for documentation.

#ifndef BOOST_BITSTREAM_BSTREAM_HPP
#define BOOST_BITSTREAM_BSTREAM_HPP

// Includes ///////////////////////////////////////////////////////////////////

#include <bitset>
#include <vector>
#include <cassert>
#include "boost/typeof/typeof.hpp"
#include "boost/assert.hpp"
#include "boost/bitstream/ibstream.hpp"

// This code uses decltype, which is defined in terms of BOOST's type_of. BOOST
// only knows about C++'s core types, so we have to tell it about streampos.
#include BOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()
BOOST_TYPEOF_REGISTER_TYPE(std::streampos)

namespace boost {

namespace bitstream {

// Constants //////////////////////////////////////////////////////////////////

// Typedefs ///////////////////////////////////////////////////////////////////

///
/// Integral type for bit-field values.
typedef decltype(std::bitset<0>().to_ulong()) bitfield;

// bitbuf /////////////////////////////////////////////////////////////////////

///
/// This class represents contiguous memory, accessed as a sequence of bit
/// fields.
///
/// @note This class is based on but does not inherit from the standard class,
/// stringbuf. The main difference is that this class provides access to bits
/// whereas stringbuf provides access to characters.
///
/// @note Currently, this class can only be used to read bits from a buffer;
/// however, it could be easily be fleshed out for writing bits, too.
class bitbuf
{
public:
    ///
    /// Constructor.
    ///
    /// @param[in] which Open mode.
    explicit bitbuf(std::ios_base::openmode which =
        std::ios_base::in | std::ios_base::out) :
        m_buffer(NULL), m_gptr(0), m_egptr(INT_MAX), m_eback(0)
    {
        // Output not support, so can't append.
        BOOST_ASSERT((which & std::ios_base::app) == 0);
        // Output not support, so can't append each time.
        BOOST_ASSERT((which & std::ios_base::ate) == 0);
        // Output not currently supported.
        BOOST_ASSERT((which & std::ios_base::out) == 0);
        // Truncate not currently supported.
        BOOST_ASSERT((which & std::ios_base::trunc) == 0);
    }

    ///
    /// Constructor.
    ///
    /// @param[in] buffer Pointer to char array to be accessed.
    /// @param[in] which Open mode.
    /// @param[in] size Number of accessible bits in char array.
    bitbuf(char *buffer, std::streamsize size = INT_MAX,
        std::ios_base::openmode which =
        std::ios_base::in | std::ios_base::out) :
        m_buffer(reinterpret_cast<unsigned char *>(buffer)), m_gptr(0),
        m_egptr(size), m_eback(0)
    {
        BOOST_ASSERT(buffer != NULL);
        BOOST_ASSERT(size >= 0);
        // Output not support, so can't append.
        BOOST_ASSERT((which & std::ios_base::app) == 0);
        // Output not support, so can't append each time.
        BOOST_ASSERT((which & std::ios_base::ate) == 0);
        // Output not currently supported.
        BOOST_ASSERT((which & std::ios_base::out) == 0);
        // Truncate not currently supported.
        BOOST_ASSERT((which & std::ios_base::trunc) == 0);
    }

    ///
    /// Constructor.
    ///
    /// @param[in] buffer Pointer to char array to be accessed.
    /// @param[in] which Open mode.
    /// @param[in] size Number of accessible bits in char array.
    bitbuf(signed char *buffer, std::streamsize size = INT_MAX,
        std::ios_base::openmode which =
        std::ios_base::in | std::ios_base::out) :
        m_buffer(reinterpret_cast<unsigned char *>(buffer)), m_gptr(0),
        m_egptr(size), m_eback(0)
    {
        BOOST_ASSERT(buffer != NULL);
        BOOST_ASSERT(size >= 0);
        // Output not support, so can't append.
        BOOST_ASSERT((which & std::ios_base::app) == 0);
        // Output not support, so can't append each time.
        BOOST_ASSERT((which & std::ios_base::ate) == 0);
        // Output not currently supported.
        BOOST_ASSERT((which & std::ios_base::out) == 0);
        // Truncate not currently supported.
        BOOST_ASSERT((which & std::ios_base::trunc) == 0);
    }

    ///
    /// Constructor.
    ///
    /// @param[in] buffer Pointer to char array to be accessed.
    /// @param[in] which Open mode.
    /// @param[in] size Number of accessible bits in char array.
    bitbuf(unsigned char *buffer, std::streamsize size = INT_MAX,
        std::ios_base::openmode which =
        std::ios_base::in | std::ios_base::out) :
        m_buffer(buffer), m_gptr(0), m_egptr(size), m_eback(0)
    {
        BOOST_ASSERT(buffer != NULL);
        BOOST_ASSERT(size >= 0);
        // Output not support, so can't append.
        BOOST_ASSERT((which & std::ios_base::app) == 0);
        // Output not support, so can't append each time.
        BOOST_ASSERT((which & std::ios_base::ate) == 0);
        // Output not currently supported.
        BOOST_ASSERT((which & std::ios_base::out) == 0);
        // Truncate not currently supported.
        BOOST_ASSERT((which & std::ios_base::trunc) == 0);
    }

    ///
    /// Constructor.
    ///
    /// @param[in] buffer Pointer to char array to be accessed.
    /// @param[in] which Open mode.
    /// @param[in] size Number of accessible bits in char array.
    bitbuf(const char *buffer, std::streamsize size = INT_MAX,
        std::ios_base::openmode which =
        std::ios_base::in | std::ios_base::out) :
        m_buffer(reinterpret_cast<unsigned char *>(const_cast<char *>(
        buffer))), m_gptr(0), m_egptr(size), m_eback(0)
    {
        BOOST_ASSERT(buffer != NULL);
        BOOST_ASSERT(size >= 0);
        // Output not support, so can't append.
        BOOST_ASSERT((which & std::ios_base::app) == 0);
        // Output not support, so can't append each time.
        BOOST_ASSERT((which & std::ios_base::ate) == 0);
        // Output not currently supported.
        BOOST_ASSERT((which & std::ios_base::out) == 0);
        // Truncate not currently supported.
        BOOST_ASSERT((which & std::ios_base::trunc) == 0);
    }

    ///
    /// Constructor.
    ///
    /// @param[in] buffer Pointer to char array to be accessed.
    /// @param[in] which Open mode.
    /// @param[in] size Number of accessible bits in char array.
    bitbuf(const signed char *buffer, std::streamsize size = INT_MAX,
        std::ios_base::openmode which =
        std::ios_base::in | std::ios_base::out) :
        m_buffer(reinterpret_cast<unsigned char *>(const_cast<signed char *>(
        buffer))), m_gptr(0), m_egptr(size), m_eback(0)
    {
        BOOST_ASSERT(buffer != NULL);
        BOOST_ASSERT(size >= 0);
        // Output not support, so can't append.
        BOOST_ASSERT((which & std::ios_base::app) == 0);
        // Output not support, so can't append each time.
        BOOST_ASSERT((which & std::ios_base::ate) == 0);
        // Output not currently supported.
        BOOST_ASSERT((which & std::ios_base::out) == 0);
        // Truncate not currently supported.
        BOOST_ASSERT((which & std::ios_base::trunc) == 0);
    }

    ///
    /// Constructor.
    ///
    /// @param[in] buffer Pointer to char array to be accessed.
    /// @param[in] which Open mode.
    /// @param[in] size Number of accessible bits in char array.
    bitbuf(const unsigned char *buffer, std::streamsize size = INT_MAX,
        std::ios_base::openmode which =
        std::ios_base::in | std::ios_base::out) :
        m_buffer(const_cast<unsigned char *>(buffer)), m_gptr(0),
        m_egptr(size), m_eback(0)
    {
        BOOST_ASSERT(buffer != NULL);
        BOOST_ASSERT(size >= 0);
        // Output not support, so can't append.
        BOOST_ASSERT((which & std::ios_base::app) == 0);
        // Output not support, so can't append each time.
        BOOST_ASSERT((which & std::ios_base::ate) == 0);
        // Output not currently supported.
        BOOST_ASSERT((which & std::ios_base::out) == 0);
        // Truncate not currently supported.
        BOOST_ASSERT((which & std::ios_base::trunc) == 0);
    }

    ///
    /// Destructor.
    ~bitbuf()
    {
        // Do nothing.
    }

    ///
    /// Number of bits currently available to read.
    ///
    /// @return Number of readable bits.
    std::streamsize in_avail() const
    {
        return egptr() - gptr();
    }

    ///
    /// Reposition get-next-bit pointer relative to current position.
    ///
    /// @param[in] offset Signed offset from current position for new position.
    /// @param[in] way From which pointer offset is applied for new position.
    /// @return Position after offset applied.
    std::streampos pubseekoff(std::streamoff offset,
        std::ios_base::seekdir way)
    {
        return seekoff(offset, way);
    }

    ///
    /// Reposition get-next-bit pointer.
    ///
    /// @param[in] position New bit position.
    /// @return Position after repositioning.
    std::streampos pubseekpos(std::streampos position)
    {
        BOOST_ASSERT(position >= 0);

        return seekpos(position);
    }

    ///
    /// Set buffer to access.
    ///
    /// @param[in] buffer Pointer to char array to be accessed.
    /// @return Pointer to this object; NULL if error.
    bitbuf *pubsetbuf(unsigned char *buffer)
    {
        BOOST_ASSERT(buffer != NULL);

        return pubsetbuf(buffer, npos);
    }

    ///
    /// Set buffer to access.
    ///
    /// @param[in] buffer Pointer to char array to be accessed.
    /// @param[in] size Number of accessible bits in char array.
    /// @param[in] position Offset of first accessible bit in char array.
    /// @return Pointer to this object; NULL if error.
    bitbuf *pubsetbuf(unsigned char *buffer, std::streamsize size,
        std::streampos position = 0)
    {
        BOOST_ASSERT(buffer != NULL);
        BOOST_ASSERT(size >= 0);
        BOOST_ASSERT(position >= 0);

        return setbuf(buffer, size, position);
    }

    ///
    /// Synchronize stream buffer with input or output device.
    ///
    /// @return 0 if buffered stream and successful; -1 otherwise.
    int pubsync()
    {
        return sync();
    }

    ///
    /// Get current bit and advance get pointer.
    ///
    /// @param[out] value Current bit before advancing pointer.
    /// @return Whether okay--eof has not been encountered.
    bool sbumpb(bitfield &value)
    {
        return sgetn(value, 1) == 1;
    }

    ///
    /// Get current bit at get pointer.
    ///
    /// @param[out] value Current bit.
    /// @return Whether okay--eof has not been encountered.
    bool sgetb(bitfield &value)
    {
        bool okay = sbumpb(value);

        if (!okay)
        {
            pubseekoff(-1, std::ios_base::cur);
        }

        return okay;
    }

    ///
    /// Get sequence of bits.
    ///
    /// @param[out] value Value of bit field.
    /// @param[in] size Number of bits in sequence of bits.
    /// @return Number of bits read from buffer or zero if error or eof.
    std::streamsize sgetn(bitfield &value, std::streamsize size)
    {
        BOOST_ASSERT(size >= 0);

        return xsgetn(value, size);
    }

    ///
    /// Advance get pointer and return next bit.
    ///
    /// @param[out] value Next bit.
    /// @return Whether okay--eof has not been encountered.
    bool snextb(bitfield &value)
    {
        bool okay;

        if (pubseekoff(1, std::ios_base::cur) ==
            static_cast<std::streampos>(bitbuf::npos))
        {
            okay = false;
        }
        else
        {
            okay = sgetb(value);
        }

        return okay;
    }

    ///
    /// Move get pointer backwards and return bit at new position.
    ///
    /// @param[out] value Bit before position prior to call.
    /// @return Whether okay--eof has not been encountered.
    bool sungetb(bitfield &value)
    {
        bool okay;

        if (pubseekoff(-1, std::ios_base::cur) ==
            static_cast<std::streampos>(bitbuf::npos))
        {
            okay = false;
        }
        else
        {
            okay = sgetb(value);
        }

        return okay;
    }

    ///
    /// Represents both an unbounded number of bits and an attempt to move past
    /// bit-stream bounds.
    ///
    /// @note A consumer might encounter this value where member functions of
    /// this class return a value of streamsize or streampos type.
    static const int npos = -1;

    ///
    /// Special, end-of-file value.
    ///
    /// @note This class cannot extract a bit sequence that is equal to this
    /// value. Therefore, when a member function returns this value, the
    /// consumer should knowthat the end of file has been reached. Actually,
    /// an attempt was made to either advance past the end or beginning of the
    /// accessible bits in the buffer.
    static const bitfield eof = static_cast<bitfield>(-1);

protected:
    ///
    /// Returns bit position within accessible input sequence of next bit to be
    /// read.
    ///
    /// @note The "get pointer."
    ///
    /// @return Next bit position.
    std::streampos gptr() const
    {
        return m_gptr;
    }

    ///
    /// Returns bit position just past last bit in accessible input sequence.
    ///
    /// @return Position after last bit.
    std::streampos egptr() const
    {
        return m_egptr;
    }

    ///
    /// Returns first bit position in accessible input sequence.
    ///
    /// @return Position of first bit.
    std::streampos eback() const
    {
        return m_eback;
    }

    ///
    /// Set pointer and offsets that define boundaries of and position within
    /// accessible input sequence.
    ///
    /// @param[in] buffer Pointer to char array to be accessed.
    /// @param[in] gbeg Position of first accessible bit in char array.
    /// @param[in] gnext Position of next bit to read.
    /// @param[in] gend Position of bit immediately after last accessible bit
    /// in char array.
    void setg(unsigned char *buffer, std::streampos gbeg, std::streampos gnext,
        std::streampos gend)
    {
        BOOST_ASSERT(buffer != NULL);
        BOOST_ASSERT(gbeg >= 0);
        BOOST_ASSERT(gnext >= 0);
        BOOST_ASSERT(gend >= 0);
        BOOST_ASSERT(gbeg <= gnext);
        BOOST_ASSERT(gnext <= gend);

        m_buffer = buffer;
        m_eback = gbeg;
        m_gptr = gnext;
        m_egptr = gend;
    }

    ///
    /// Advances the get pointer by specified number of bit positions.
    ///
    /// @param[in] Value by which to increase the get pointer.
    void gbump(std::streamoff offset)
    {
        seekoff(offset, std::ios_base::cur);
    }

    ///
    /// Set get pointer to absolute position.
    ///
    /// @param[in] position New absolute position for get pointer.
    /// @return New position after get pointer modified or npos if error.
    virtual std::streampos seekpos(std::streampos position)
    {
        BOOST_ASSERT(position >= 0);

        return assure_valid_get_pointer(position);
    }

    ///
    /// Set get pointer relative to current position.
    ///
    /// @param[in] offset Amount by which get pointer is adjusted.
    /// @param[in] way From which pointer offset is applied for new position.
    /// @return New position after get pointer modified.
    virtual std::streampos seekoff(std::streamoff offset,
        std::ios_base::seekdir way)
    {
        decltype(seekoff(0,std::ios_base::cur)) new_position;

        switch (way)
        {
        case std::ios_base::beg:
            new_position = assure_valid_get_pointer(eback() + offset);
            break;

        case std::ios_base::cur:
            new_position = assure_valid_get_pointer(gptr() + offset);
            break;

        case std::ios_base::end:
            new_position = assure_valid_get_pointer(egptr() + offset);
            break;

        default:
            BOOST_ASSERT(false);
            new_position = npos;
            break;

        }

        return new_position;
    }

    ///
    /// Assure that position is within bounds of accessible input sequence.
    ///
    /// If bit position is within bounds, use as internal get pointer and
    /// return it; otherwise, return npos.
    ///
    /// @param[in] position Candidate for new current position, or get pointer.
    std::streampos assure_valid_get_pointer(std::streampos position)
    {
        BOOST_ASSERT(position >= 0);

        decltype(assure_valid_get_pointer(0)) new_position;

        if (position < eback() || position > egptr())
        {
            new_position = npos;
        }
        else
        {
            m_gptr = new_position = position;
        }

        BOOST_ASSERT(new_position == static_cast<std::streampos>(npos) ||
            (new_position >= eback() && new_position <= egptr()));

        return new_position;
    }

    ///
    /// Set buffer to access.
    ///
    /// @param[in] buffer Pointer to char array containing bits to access.
    /// @param[in] size Number of accessible bits in char array.
    /// @param[in] position Offset of first accessible bit in char array.
    /// @return Pointer to this object; NULL if error.
    virtual bitbuf *setbuf(unsigned char *buffer, std::streamsize size,
        std::streampos position)
    {
        BOOST_ASSERT(buffer != NULL);
        BOOST_ASSERT(size >= 0);
        BOOST_ASSERT(position >= 0);

        setg(buffer, position, position, size);

        return this;
    }

    ///
    /// Synchronize stream buffer with input or output device.
    ///
    /// @note Since we do not currently have an I/O device with which to
    /// synchronize--we are unbuffered--this function always returns in error
    /// with -1.
    ///
    /// @return 0 if buffered stream and successful; -1 otherwise.
    virtual int sync()
    {
        return -1;
    }

    ///
    /// Get sequence of bits.
    ///
    /// @param[out] value Value of bit field.
    /// @param[in] size Number of bits in sequence of bits.
    /// @return Number of bits read from buffer or zero if error or eof.
    virtual std::streamsize xsgetn(bitfield &value, std::streamsize size)
    {
        BOOST_ASSERT(size >= 0);

        decltype(size) bits_gotten = 0;

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
            // Offset in current byte.
            size_t intra_byte_bit_offset = m_gptr % CHAR_BIT;
            size_t shift_amount = (CHAR_BIT -
                ((size + intra_byte_bit_offset) % CHAR_BIT)) % CHAR_BIT;

            size_t old_shift_amount = (static_cast<unsigned>(CHAR_BIT) * 2 -
                intra_byte_bit_offset - size) % CHAR_BIT;
            if (old_shift_amount > CHAR_BIT) {
                old_shift_amount += CHAR_BIT;
            }

            BOOST_ASSERT(shift_amount == old_shift_amount);

            mask <<= shift_amount;

            // Accumulate current value of each byte involved in bit field.
            size_t byte_count = (static_cast<size_t>(size) + shift_amount +
                CHAR_BIT - 1) / CHAR_BIT;
            std::remove_reference<decltype(value)>::type pre_mask_value = 0;
            for (decltype(byte_count) i = 0; i < byte_count; ++i)
            {
                pre_mask_value <<= static_cast<unsigned>(CHAR_BIT);
                pre_mask_value |= current_byte()[i];
            }

            // Apply mask and right justify. (Consumer wants the normalized
            // value, not the value as it occurs in the buffer.)
            value = pre_mask_value & mask;
            value >>= shift_amount;

            // Advance bit pointer past field in preparation for next access.
            gbump(size);

            bits_gotten = size;
        }

        return bits_gotten;
    }

private:
    ///
    /// Get pointer to current byte.
    ///
    /// @return Pointer to byte containing current bit position (the next bit
    /// to read).
    unsigned char *current_byte() const
    {
        return &m_buffer[m_gptr / CHAR_BIT];
    }

    ///
    /// Current bit position in buffer.
    ///
    /// @note Position is zero-based, starting with MSB in byte pointed to by
    /// m_buffer, progressing to LSB of that byte, then MSB of next byte, etc.
    ///
    /// @note This is analogous to streambuf::gptr, except it "points" to a
    /// bit position rather than a character position.
    std::streampos m_gptr;

    ///
    /// End of accessible input sequence.
    ///
    /// @note Points just past last bit.
    std::streampos m_egptr;

    ///
    /// Beginning of accessible input sequence.
    ///
    /// @note Not currently used. Always 0, which is first bit in first byte.
    std::streampos m_eback;

    ///
    /// Pointer to first byte of char array containing the bits.
    unsigned char *m_buffer;
};

// ibitstream /////////////////////////////////////////////////////////////////

///
/// This class provides an interface to manipulate bits as an input stream.
///
/// @note This class is based on but does not inherit from the standard class,
/// istringstream. The main difference is that this class provides access to
/// bits whereas istringstream provides access to characters.
///
/// @note Currently, this class can only be used to read bits from a stream;
/// however, it could be easily be fleshed out for writing bits, too. For
/// example, it currently does not support the analogue to the
/// istringstream::putback method.
class ibitstream : public ibstream
{
public:
    ///
    /// Constructor.
    ///
    /// @param[in] which Open mode.
    explicit ibitstream(std::ios_base::openmode which = std::ios_base::in) :
        m_bitbuf(which), m_gvalue(0), m_repeat(0)
    {
        // Append to input? Huh?
        BOOST_ASSERT((which & std::ios_base::app) == 0);
        // Input at end of stream? Huh?
        BOOST_ASSERT((which & std::ios_base::ate) == 0);
        // Output not currently supported.
        BOOST_ASSERT((which & std::ios_base::out) == 0);
        // Truncate not currently supported.
        BOOST_ASSERT((which & std::ios_base::trunc) == 0);
    }

    ///
    /// Constructor.
    ///
    /// @param[in] buffer Pointer to char array to be accessed.
    /// @param[in] size Number of accessible bits in char array.
    /// @param[in] which Open mode.
    explicit ibitstream(char *buffer, std::streamsize size = INT_MAX,
        std::ios_base::openmode which = std::ios_base::in) :
        m_bitbuf(buffer, size, which), m_gvalue(0), m_repeat(0)
    {
        BOOST_ASSERT(buffer != NULL);
        BOOST_ASSERT(size >= 0);
        // Append to input? Huh?
        BOOST_ASSERT((which & std::ios_base::app) == 0);
        // Input at end of stream? Huh?
        BOOST_ASSERT((which & std::ios_base::ate) == 0);
        // Output not currently supported.
        BOOST_ASSERT((which & std::ios_base::out) == 0);
        // Truncate not currently supported.
        BOOST_ASSERT((which & std::ios_base::trunc) == 0);
    }

    ///
    /// Constructor.
    ///
    /// @param[in] buffer Pointer to char array to be accessed.
    /// @param[in] size Number of accessible bits in char array.
    /// @param[in] which Open mode.
    explicit ibitstream(signed char *buffer, std::streamsize size = INT_MAX,
        std::ios_base::openmode which = std::ios_base::in) :
        m_bitbuf(buffer, size, which), m_gvalue(0), m_repeat(0)
    {
        BOOST_ASSERT(buffer != NULL);
        BOOST_ASSERT(size >= 0);
        // Append to input? Huh?
        BOOST_ASSERT((which & std::ios_base::app) == 0);
        // Input at end of stream? Huh?
        BOOST_ASSERT((which & std::ios_base::ate) == 0);
        // Output not currently supported.
        BOOST_ASSERT((which & std::ios_base::out) == 0);
        // Truncate not currently supported.
        BOOST_ASSERT((which & std::ios_base::trunc) == 0);
    }

    ///
    /// Constructor.
    ///
    /// @param[in] buffer Pointer to char array to be accessed.
    /// @param[in] size Number of accessible bits in char array.
    /// @param[in] which Open mode.
    explicit ibitstream(unsigned char *buffer, std::streamsize size = INT_MAX,
        std::ios_base::openmode which = std::ios_base::in) :
        m_bitbuf(buffer, size, which), m_gvalue(0), m_repeat(0)
    {
        BOOST_ASSERT(buffer != NULL);
        BOOST_ASSERT(size >= 0);
        // Append to input? Huh?
        BOOST_ASSERT((which & std::ios_base::app) == 0);
        // Input at end of stream? Huh?
        BOOST_ASSERT((which & std::ios_base::ate) == 0);
        // Output not currently supported.
        BOOST_ASSERT((which & std::ios_base::out) == 0);
        // Truncate not currently supported.
        BOOST_ASSERT((which & std::ios_base::trunc) == 0);
    }

    ///
    /// Constructor.
    ///
    /// @param[in] buffer Pointer to char array to be accessed.
    /// @param[in] size Number of accessible bits in char array.
    /// @param[in] which Open mode.
    explicit ibitstream(const char *buffer, std::streamsize size = INT_MAX,
        std::ios_base::openmode which = std::ios_base::in) :
        m_bitbuf(buffer, size, which), m_gvalue(0), m_repeat(0)
    {
        BOOST_ASSERT(buffer != NULL);
        BOOST_ASSERT(size >= 0);
        // Append to input? Huh?
        BOOST_ASSERT((which & std::ios_base::app) == 0);
        // Input at end of stream? Huh?
        BOOST_ASSERT((which & std::ios_base::ate) == 0);
        // Output not currently supported.
        BOOST_ASSERT((which & std::ios_base::out) == 0);
        // Truncate not currently supported.
        BOOST_ASSERT((which & std::ios_base::trunc) == 0);
    }

    ///
    /// Constructor.
    ///
    /// @param[in] buffer Pointer to char array to be accessed.
    /// @param[in] size Number of accessible bits in char array.
    /// @param[in] which Open mode.
    explicit ibitstream(const signed char *buffer,
        std::streamsize size = INT_MAX,
        std::ios_base::openmode which = std::ios_base::in) :
        m_bitbuf(buffer, size, which), m_gvalue(0), m_repeat(0)
    {
        BOOST_ASSERT(buffer != NULL);
        BOOST_ASSERT(size >= 0);
        // Append to input? Huh?
        BOOST_ASSERT((which & std::ios_base::app) == 0);
        // Input at end of stream? Huh?
        BOOST_ASSERT((which & std::ios_base::ate) == 0);
        // Output not currently supported.
        BOOST_ASSERT((which & std::ios_base::out) == 0);
        // Truncate not currently supported.
        BOOST_ASSERT((which & std::ios_base::trunc) == 0);
    }

    ///
    /// Constructor.
    ///
    /// @param[in] buffer Pointer to char array to be accessed.
    /// @param[in] size Number of accessible bits in char array.
    /// @param[in] which Open mode.
    explicit ibitstream(const unsigned char *buffer,
        std::streamsize size = INT_MAX,
        std::ios_base::openmode which = std::ios_base::in) :
        m_bitbuf(buffer, size, which), m_gvalue(0), m_repeat(0)
    {
        BOOST_ASSERT(buffer != NULL);
        BOOST_ASSERT(size >= 0);
        // Append to input? Huh?
        BOOST_ASSERT((which & std::ios_base::app) == 0);
        // Input at end of stream? Huh?
        BOOST_ASSERT((which & std::ios_base::ate) == 0);
        // Output not currently supported.
        BOOST_ASSERT((which & std::ios_base::out) == 0);
        // Truncate not currently supported.
        BOOST_ASSERT((which & std::ios_base::trunc) == 0);
    }

    ///
    /// Destructor.
    ~ibitstream()
    {
        // Do nothing.
    }

    ///
    /// Get one bit from stream.
    ///
    /// @return Next bit from stream.
    bitfield get()
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

    ///
    /// Ignore, or skip over, bits in stream.
    ///
    /// @param[in] bits Number of bits to ignore.
    /// @return This bit stream.
    ibitstream &ignore(std::streamsize bits = 1)
    {
        BOOST_ASSERT(bits >= 0);

        if (m_bitbuf.pubseekoff(bits, std::ios_base::cur) ==
            static_cast<std::streampos>(bitbuf::npos))
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


    ///
    /// Align get pointer to next bit multiple if not already at one.
    ///
    /// @param[in] bit Bit multiple, such as 8 for byte alignment.
    /// @return This bit stream.
    ibitstream &aligng(size_t bit)
    {
        BOOST_ASSERT(bit > 0);
        if (good() && bit > 0)
        {
            seekg(((static_cast<size_t>(tellg()) + bit - 1) / bit) * bit);
        }

        m_gcount = 0;

        return *this;
    }

    ///
    /// Get next bit from stream without advancing get pointer.
    ///
    /// @return Next bit from stream.
    bitfield peek()
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

    ///
    /// Get bits from stream.
    ///
    /// @note Unlike get(), this function returns the bit value in an integral,
    /// not a bitset.
    ///
    /// @param[out] Integral to receive bits from stream.
    /// @param[in] bits Number of bits to read.
    /// @return This bit stream.
    ibitstream &read(bitfield &value, std::streamsize bits)
    {
        BOOST_ASSERT(bits >= 0);

        decltype(bits) bits_read = m_bitbuf.sgetn(value, bits);
        if (bits_read != bits)
        {
            failbit();
            eofbit();
            m_gcount = 0;
            value = 0;
        }
        else
        {
            m_gcount = bits_read;
        }

        m_gvalue = value;

        return *this;
    }

    ///
    /// Get "some" bits from stream.
    ///
    /// @note This function is provided for completeness. A bit stream residing
    /// in memory does not possess the behavior that, unlike istringstream,
    /// would make readsome() behave any different from read().
    ///
    /// @param[out] Integral to receive bits from stream.
    /// @param[in] bits Number of bits to read.
    /// @return This bit stream.
    ibitstream &readsome(bitfield &value, std::streamsize bits)
    {
        BOOST_ASSERT(bits >= 0);

        return read(value, bits);
    }

    ///
    /// Set position of get pointer relative to indicated internal pointer.
    ///
    /// @param[in] offset Relative offset from indicated pointer.
    /// @param[in] dir Bit pointer to which offset is applied.
    /// @return This bit stream.
    ibitstream &seekg(std::streamoff offset, std::ios_base::seek_dir dir)
    {
        if (m_bitbuf.pubseekoff(offset, dir) ==
            static_cast<std::streampos>(bitbuf::npos))
        {
            failbit();
        }

        return *this;
    }

    ///
    /// Synchronize input buffer with source of bits.
    ///
    /// @return 0 if buffered stream and successful; -1 otherwise.
    int sync()
    {
        return m_bitbuf.pubsync();
    }

    ///
    /// Get position of get pointer.
    ///
    /// @return Bit position of next bit to be read.
    std::streampos tellg()
    {
        return m_bitbuf.pubseekoff(0, std::ios_base::cur);
    }

    ///
    /// Set position of get pointer.
    ///
    /// @param[in] position Bit position.
    /// @return This bit stream.
    ibitstream &seekg(std::streampos position)
    {
        BOOST_ASSERT(position >= 0);

        if (m_bitbuf.pubseekpos(position) ==
            static_cast<std::streampos>(bitbuf::npos))
        {
            failbit();
        }

        return *this;
    }

    ///
    /// Move get pointer backwards and return bit at new position.
    ///
    /// @return This bit stream.
    ibitstream &unget()
    {
        m_gcount = 0;

        if (static_cast<decltype(bitbuf::eof)>(
            m_bitbuf.pubseekoff(-1, std::ios_base::cur)) == bitbuf::eof)
        {
            failbit();
        }

        return *this;
    }

    ///
    /// Get value extracted by last input operation.
    ///
    /// @note The value returned by this function is only valid if gcount()
    /// returns a value greater than zero.
    ///
    /// @return Most recent extracted value.
    bitfield gvalue() const
    {
        return m_gvalue;
    }

    ///
    /// Set repeat count for subsequent vector extractions.
    ///
    /// @note This function does not extract anything from ibitstream. It
    /// merely saves a value that any subsequent vector extractions use to know
    /// how many bit fields to extract into the same number of vector elements.
    ///
    /// @param repeat Number of bit fields to extract to each subsequent
    /// vector.
    /// @return This bit stream.
    ibitstream &setrepeat(size_t repeat)
    {
        m_repeat = repeat;

        return *this;
    }

private:
    ///
    /// Friend const functions for access to badbit().
    //@{
    template <typename T> friend ibitstream &operator>>(ibitstream &ibs,
        const T &b);
    template <size_t N> friend ibitstream &operator>>(ibitstream &ibs,
        const std::bitset<N> &bs);
    friend ibitstream &operator>>(ibitstream &ibs, const bool &b);
    //@}

    ///
    /// Determine whether get pointer is aligned to bit multiple.
    ///
    /// Example:
    /// @code
    /// static const bitset<2> version(0x2);
    /// bitset<4> csrcCount;
    /// bool marker;
    /// bitset<7> payloadType;
    /// DWORD timestamp, ssrcIdentifier;
    /// ibitstream bin(rtpHeader);
    /// bin >> version >> aligng(4) >> csrcCount;
    /// assert(bin.alignedg(8));
    /// // (aligng is redundant here because already at byte alignment)
    /// bin >> aligng(8) >> marker >> payloadType;
    /// bin >> timestamp >> ssrcIdentifier;
    /// @endcode
    ///
    /// @param[in] bit Bit multiple, such as 8 for byte alignment.
    /// @return Whether get pointer is bit-multiple aligned.
    bool alignedg(size_t bit)
    {
        return tellg() % bit == 0;
    }

    ///
    /// Buffer from which this class serially extracts bits.
    bitbuf m_bitbuf;

    ///
    /// Value extracted by last input operation.
    bitfield m_gvalue;

    ///
    /// Number of bit fields extracted to a container.
    size_t m_repeat;
};

// Manipulators ///////////////////////////////////////////////////////////////

//@{
///
/// This class represents the setrepeat bit-stream manipulator.
///
/// @note This is a roundabout but necessary way of implementing manipulators
/// that take parameters, such as the setprecision() manipulator.
class setrepeat
{
public:
    ///
    /// Constructor.
    ///
    /// @param[in] repeat Number of bit fields to extract to each subsequent
    /// integral container.
    setrepeat(size_t repeat) : m_repeat(repeat)
    {
        // Do nothing.
    }

    ///
    /// Overload for the () operator on this class.
    ///
    /// @param[in,out] ibs Reference to ibitstream on lhs of >> operator.
    /// @return Reference to ibitstream parameter.
    ibitstream &operator()(ibitstream &ibs) const
    {
        return ibs.setrepeat(m_repeat);
    }

private:
    ///
    /// Number of bit fields to extract per container.
    size_t m_repeat;
};

///
/// Manipulator for ibitstream that sets repeat count for subsequent container
/// extractions.
///
/// @param[in,out] ibs Reference to ibitstream on left-hand side of operator.
/// @param[in] repeat Instance of setrepeat class.
/// @return Reference to ibitstream parameter.
inline ibitstream &operator>>(ibitstream &ibs, setrepeat repeat)
{
    return repeat(ibs);
}
//@}

//@{
///
/// This class represents the ignore bit-stream manipulator.
///
/// @see Implementation note for setrepeat manipulator.
class ignore
{
public:
    ///
    /// Constructor.
    ///
    /// @param[in] repeat Number of bits to ignore in bit stream.
    ignore(size_t bits) : m_bits(bits)
    {
        // Do nothing.
    }

    ///
    /// Overload for the () operator on this class.
    ///
    /// @param[in,out] ibs Reference to ibitstream on lhs of >> operator.
    /// @return Reference to ibitstream parameter.
    ibitstream &operator()(ibitstream &ibs) const
    {
        return ibs.ignore(m_bits);
    }

private:
    ///
    /// Number of bit fields to ignore in bit stream.
    size_t m_bits;
};

///
/// Manipulator for ibitstream that ignores bits.
///
/// @param[in,out] ibs Reference to ibitstream on left-hand side of operator.
/// @param[in] repeat Instance of ignore class.
/// @return Reference to ibitstream parameter.
inline ibitstream &operator>>(ibitstream &ibs, ignore skip)
{
    return skip(ibs);
}
//@}

//@{
///
/// This class represents the aligng bit-stream manipulator.
///
/// @see Implementation note for setrepeat manipulator.
///
/// Examples:
/// @code
/// // Advance get pointer to next nibble.
/// static const bitset<2> version(0x2);
/// bitset<4> csrcCount;
/// bool marker;
/// bitset<7> payloadType;
/// DWORD timestamp, ssrcIdentifier;
/// ibitstream bin(rtpHeader);
/// bin >> version >> aligng(4) >> csrcCount >> marker >> payloadType;
/// bin >> timestamp >> ssrcIdentifier;
/// @endcode
///
/// @code
/// // Advance get pointer to next word.
/// bitset<16> sequenceNumber;
/// ibitstream(rtpHeader).aligng(sizeof(WORD) * CHAR_BIT) >> sequenceNumber;
/// @endcode
///
/// @code
/// // Advance get pointer to next double word.
/// bool b;
/// static const bitset<2> version(0x2);
/// bitset<4> csrcCount;
/// bitset<7> payloadType;
/// DWORD timestamp;
/// ibitstream bin(rtpHeader);
/// bin >> version >> b >> b >> csrcCount >> b >> payloadType
///     >> aligng(32) >> timeStamp;
/// @endcode
class aligng
{
public:
    ///
    /// Constructor.
    ///
    /// @param[in] bits Number of bits at which to align the get pointer.
    aligng(size_t bits) : m_bits(bits)
    {
        BOOST_ASSERT(bits > 0);
    }

    ///
    /// Overload for the () operator on this class.
    ///
    /// @param[in,out] ibs Reference to ibitstream on lhs of >> operator.
    /// @return Reference to ibitstream parameter.
    ibitstream &operator()(ibitstream &ibs) const
    {
        return ibs.aligng(m_bits);
    }

private:
    ///
    /// Number of bit at which to align the get pointer.
    size_t m_bits;
};

///
/// Manipulator for ibitstream that aligns the get pointer in the input stream.
///
/// @param[in,out] ibs Reference to ibitstream on left-hand side of operator.
/// @param[in] align Instance of aligng class.
/// @return Reference to ibitstream parameter.
inline ibitstream &operator>>(ibitstream &ibs, aligng align)
{
    return align(ibs);
}
//@}

// Operator overloads /////////////////////////////////////////////////////////

///
/// Get single bit from input stream and place in bool.
///
/// @param[in,out] ibs Reference to ibitstream on left-hand side of operator.
/// @param[out] b bool on right-hand side of operator.
/// @return Reference to ibitstream parameter.
inline ibitstream &operator>>(ibitstream &ibs, bool &b)
{
    // Extract bit; must be equal to b
    bitfield value;
    ibs.read(value, 1);
    b = value != 0;

    return ibs;
}

///
/// Get single bit from input stream that must be equal to bool.
///
/// @param[in,out] ibs Reference to ibitstream on left-hand side of operator.
/// @param[in] b bool on right-hand side of operator.
/// @return Reference to ibitstream parameter.
inline ibitstream &operator>>(ibitstream &ibs, const bool &b)
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

// Templates //////////////////////////////////////////////////////////////////

///
/// Get bit field from input stream and place in integral.
///
/// @param[in,out] ibs Reference to ibitstream on left-hand side of operator.
/// @param[out] b Integral on right-hand side of operator.
/// @return Reference to ibitstream parameter.
template <typename T>
ibitstream &operator>>(ibitstream &ibs, T &b)
{
    bitfield value;
    ibs.read(value, sizeof(T) * CHAR_BIT);
    b = static_cast<T>(value);

    return ibs;
}

///
/// Get bit field from input stream that must be equal to integral value.
///
/// @param[in,out] ibs Reference to ibitstream on left-hand side of operator.
/// @param[in] b Integral on right-hand side of operator.
/// @return Reference to ibitstream parameter.
template <typename T>
ibitstream &operator>>(ibitstream &ibs, const T &b)
{
    bitfield value;
    ibs.read(value, sizeof(T) * CHAR_BIT);
    if (b != value)
    {
        ibs.badbit();
    }

    return ibs;
}

///
/// Get bit fields from input stream and place in integral vector.
///
/// @note Starting with the first element, this function populates existing
/// elements in the vector with bit fields sequentially extracted from the
/// input stream. It does not increase the size of the vector, e.g., with
/// push_back(), because it relies on the existing size of the vector to know
/// how many bit fields to extract.
///
/// @param[in,out] ibs Reference to ibitstream on left-hand side of operator.
/// @param[out] v Integral vector on right-hand side of operator.
/// @return Reference to ibitstream parameter.
template <typename T>
ibitstream &operator>>(ibitstream &ibs, std::vector<T> &v)
{
    decltype(v.size()) vector_size = v.size();
    for (decltype(vector_size) i = 0; i < vector_size; ++i) {
        ibs >> v[i];
    }

    return ibs;
}

///
/// Get bit fields from input stream that must be equal to elements in integral
/// vector.
///
/// @see Size note for non-const version of this function.
///
/// @param[in,out] ibs Reference to ibitstream on left-hand side of operator.
/// @param[in] v Integral vector on right-hand side of operator.
/// @return Reference to ibitstream parameter.
template <typename T>
ibitstream &operator>>(ibitstream &ibs, std::vector<const T> &v)
{
    decltype(v.size()) vectorSize = v.size();
    for (decltype(vectorSize) i = 0; i < vectorSize; ++i) {
        ibs >> v[i];
    }

    return ibs;
}

///
/// Get bits from input stream and place in bitset.
///
/// @param[in,out] ibs Reference to ibitstream on left-hand side of operator.
/// @param[out] bs bitset on right-hand side of operator.
/// @return Reference to ibitstream parameter.
template <size_t N>
ibitstream &operator>>(ibitstream &ibs, std::bitset<N> &bs)
{
    decltype(bs.to_ulong()) value;
    ibs.read(value, N);
    bs = static_cast<long>(value);
    BOOST_ASSERT(bs.to_ulong() == value);

    return ibs;
}

///
/// Get bits from input stream that must be equal to bitset value.
///
/// @param[in,out] ibs Reference to ibitstream on left-hand side of operator.
/// @param[in] bs bitset on right-hand side of operator.
/// @return Reference to ibitstream parameter.
template <size_t N>
ibitstream &operator>>(ibitstream &ibs, const std::bitset<N> &bs)
{
    decltype(bs.to_ulong()) value;
    ibs.read(value, N);
    if (bs.to_ulong() != value)
    {
        ibs.badbit();
    }

    return ibs;
}

} // namespace bitstream

} // namespace boost

#endif
