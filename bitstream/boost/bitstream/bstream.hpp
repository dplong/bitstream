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
        m_bitbuf(which), ibstream(&m_bitbuf), m_gvalue(0), m_repeat(0)
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

// I don't think there needs to be non-const-char versions of the constructors.
#if 0
    ///
    /// Constructor.
    ///
    /// @param[in] buffer Pointer to char array to be accessed.
    /// @param[in] size Number of accessible bits in char array.
    /// @param[in] which Open mode.
    explicit ibitstream(char *buffer, std::streamsize size = INT_MAX,
        std::ios_base::openmode which = std::ios_base::in) :
        m_bitbuf(buffer, size, which), ibstream(&m_bitbuf), m_gvalue(0), m_repeat(0)
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
        m_bitbuf(buffer, size, which), ibstream(&m_bitbuf), m_gvalue(0), m_repeat(0)
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
        m_bitbuf(buffer, size, which), ibstream(&m_bitbuf), m_gvalue(0), m_repeat(0)
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
#endif

    ///
    /// Constructor.
    ///
    /// @param[in] buffer Pointer to char array to be accessed.
    /// @param[in] size Number of accessible bits in char array.
    /// @param[in] which Open mode.
    explicit ibitstream(const char *buffer, std::streamsize size = INT_MAX,
        std::ios_base::openmode which = std::ios_base::in) :
        m_bitbuf(buffer, size, which), ibstream(&m_bitbuf), m_gvalue(0), m_repeat(0)
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
        m_bitbuf(buffer, size, which), ibstream(&m_bitbuf), m_gvalue(0), m_repeat(0)
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
        m_bitbuf(buffer, size, which), ibstream(&m_bitbuf), m_gvalue(0), m_repeat(0)
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
    /// Get one bit from stream.
    ///
    /// @return Next bit from stream.
    bitfield get()
    {
        if (!rdbuf()->sbumpb(m_gvalue))
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

        if (rdbuf()->pubseekoff(bits, std::ios_base::cur) ==
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
        if (!rdbuf()->sgetb(m_gvalue))
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

        decltype(bits) bits_read = rdbuf()->sgetn(value, bits);
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
        if (rdbuf()->pubseekoff(offset, dir) ==
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
        return rdbuf()->pubsync();
    }

    ///
    /// Get position of get pointer.
    ///
    /// @return Bit position of next bit to be read.
    std::streampos tellg()
    {
        return rdbuf()->pubseekoff(0, std::ios_base::cur);
    }

    ///
    /// Set position of get pointer.
    ///
    /// @param[in] position Bit position.
    /// @return This bit stream.
    ibitstream &seekg(std::streampos position)
    {
        BOOST_ASSERT(position >= 0);

        if (rdbuf()->pubseekpos(position) ==
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
            rdbuf()->pubseekoff(-1, std::ios_base::cur)) == bitbuf::eof)
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
