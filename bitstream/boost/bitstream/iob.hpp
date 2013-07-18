// Boost general library iob.hpp header file //////////////////////////////////
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

#ifndef BOOST_BITSTREAM_IOB_HPP
#define BOOST_BITSTREAM_IOB_HPP

namespace boost {

namespace bitstream {

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
    /// Get pointer to char-array stream buffer.
    ///
    /// @note This is analogous to stringbuf::str().
    ///
    /// @return Pointer to stream buffer.
    // TBD - Should this return const?
    const char *data() const
    {
        return reinterpret_cast<char *>(m_buffer);
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

class iob
{
public:
    ///
    /// Constructor.
    ///
    /// @param[in] Pointer to a bitbuf object.
    explicit iob(bitbuf *bb)
    {
        init(bb);
    }

    ///
    /// Destructor.
    ///
    /// @param[in] Pointer to a bitbuf object.
    virtual ~iob()
    {
        // Do nothing.
    }

    // TBD - This might conflict with the operator bool() const definition.
    ///
    /// Evaluate stream object for success.
    ///
    /// This function returns null if the internal failbit or badbit are set
    /// for this stream; otherwise, it returns a non-zero pointer.
    ///
    /// @return Whether 0 if failbit or badbit are set; non-zero otherwise.
    operator void *() const
    {
        static int dummy;
        return fail() ? 0 : reinterpret_cast<void *>(&dummy);
    }

    ///
    /// Check if bitstream is good for continued operation.
    ///
    /// @return Whether any of the error flags are set.
    bool good() const
    {
        return rdstate() == 0;
    }

    ///
    /// Check if error flag, eofbit, is set.
    ///
    /// @note eofbit is set when an operation attempts to access a bit position
    /// outside of the bit stream, e.g., reading past end of bitstream.
    ///
    /// @return Whether previous input operation set eofbit.
    bool eof() const
    {
        return (rdstate() & std::ios_base::eofbit) != 0;
    }

    ///
    /// Check if either error flag, failbit or badbit, is set.
    ///
    /// @note failbit is set when there is an error with the internal logic of
    /// an operation.
    ///
    /// @see Note for bad().
    ///
    /// @return Whether previous input operation set failbit or badbit.
    bool fail() const
    {
        return (rdstate() & std::ios_base::failbit) != 0 || eof();
    }

    ///
    /// Check if error flag, badbit, is set.
    ///
    /// @note badbit is set if the integrity of the stream is lost, e.g.,
    /// encountered unexpected value.
    ///
    /// @return Whether previous input operation set badbit.
    bool bad() const
    {
        return (rdstate() & std::ios_base::badbit) != 0;
    }

    ///
    /// Evaluate stream object for failure.
    ///
    /// This function returns whether the internal failbit or badbit has been
    /// set for this ibitstream.
    ///
    /// @note Same as calling fail(). bin.fail() is the same as !bin.
    ///
    /// @return Whether the failbit or badbit has been set.
    bool operator!() const
    {
        return fail();
    }

    ///
    /// Evaluate stream object for success.
    ///
    /// This function returns whether the internal failbit and badbit are not
    /// set for this ibitstream.
    ///
    /// @note Same as calling !fail(). !bin.fail() is the same as bin.
    ///
    /// @return Whether the failbit and badbit are not set.
    operator bool() const
    {
        return !fail();
    }

    ///
    /// Get error state flags.
    ///
    /// @return Error state flags.
    std::ios_base::iostate rdstate() const
    {
        return m_state;
    }

    ///
    /// Set error state flags.
    ///
    /// @note This function sets states additively--no state is cleared.
    ///
    /// @param[in] state Error state flags.
    void setstate(std::ios_base::iostate state)
    {
        m_state |= state;
    }

    ///
    /// Set error state flags.
    ///
    /// @param[in] state Error state flags.
    void clear(std::ios_base::iostate state = std::ios_base::goodbit)
    {
        m_state = state;
    }

    ///
    /// Get the bitbuf object currently associated with the stream.
    ///
    /// @return A pointer to the bitbuf object associated with the stream.
    bitbuf *rdbuf() const
    {
        return m_bitbuf;
    }

    ///
    /// Set the bitbuf object associated with the stream.
    ///
    /// @return A pointer to the bitbuf object previously associated with the stream.
    bitbuf *rdbuf(bitbuf *bb)
    {
        bitbuf *previous_bitbuf = m_bitbuf;
        m_bitbuf = bb;
        m_state = bb == 0 ? std::ios_base::badbit : std::ios_base::goodbit;
        return previous_bitbuf;
    }

// TBD - Consider implementing the tie functionality after obstream is implemented.
#if 0
    ///
    /// Get tied stream.
    ///
    /// @return Pointer to stream object tied before call; if none, null pointer.
    obstream *tie() const
    {
        return 0;
    }

    ///
    /// Tie to tieb.
    ///
    /// @return Pointer to stream object tied before call; if none, null pointer.
    obstream *tie(obstream *tieb)
    {
        return 0;
    }
#endif

protected:
    ///
    /// Constructor.
    ///
    /// @note Consumer must call init() after calling this constructor.
    iob()
    {
        // Do nothing.
    }

    ///
    /// Initialize member variables.
    ///
    /// @param[in] Pointer to a bitbuf object.
    void init(bitbuf *bb)
    {
        m_bitbuf = bb;
        m_state = bb == 0 ? std::ios_base::badbit : std::ios_base::goodbit;
    }

    ///
    /// Set badbit error flag.
    void badbit()
    {
        m_state |= std::ios_base::badbit;
    }

    ///
    /// Set failbit error flag.
    void failbit()
    {
        m_state |= std::ios_base::failbit;
    }

    ///
    /// Set eofbit error flag.
    void eofbit()
    {
        m_state |= std::ios_base::eofbit;
    }

private:
    ///
    /// Buffer from which this class serially extracts bits.
    bitbuf *m_bitbuf;

    ///
    /// State of input stream.
    ///
    /// @note This member variable is a bit mask; therefore it represents
    /// multiple states including eof and failure.
    std::ios_base::iostate m_state;
};

} // namespace bitstream

} // namespace boost

#endif