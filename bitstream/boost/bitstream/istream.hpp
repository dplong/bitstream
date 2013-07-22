/** \file
    \brief Input bit-stream classes.
    \details This header file contains the input bit-stream classes.
    \copyright Copyright (C) 2013 Paul Long.
    \note Use, modification, and distribution is subject to the Boost Software
        License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
    \see http://www.boost.org/ for latest version.
    \see http://www.boost.org/libs/bitstream for documentation.
*/

#ifndef BOOST_BITSTREAM_ISTREAM_HPP
#define BOOST_BITSTREAM_ISTREAM_HPP

#include <boost/bitstream/iob.hpp>

namespace boost {

namespace bitstream {

/**
    Input stream objects of this class can read and interpret sequences of bits.

    \todo Once ostream is implemented, implement putback()
*/
class istream : public iob
{
public:
    /**
        Constructor.
    */
    explicit istream(bitbuf *bb) : iob(bb), m_gcount(0), m_gvalue(0), m_repeat(0)
    {
        // Do nothing.
    }

    /**
        Get number of bits extracted by last input operation.
    */
    std::streamsize gcount() const
    {
        return m_gcount;
    }

    /**
        Get value extracted by last input operation.

        \note The value returned by this function is only valid if gcount()
            returns a value greater than zero.

        \todo Is this an anaologue of a std::istream function? For use solely
            within class?

        \return Most recent extracted value.
    */
    bitfield gvalue() const
    {
        return m_gvalue;
    }

    /**
        Get one bit from stream.

        \todo Support the other get() overloads.
        \todo Should this function return value as int?

        \return Next bit from stream.
    */
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

    /**
        Ignore, or skip over, bits in stream.

        \param[in] bits Number of bits to ignore.
        \return This bit stream.
    */
    istream &ignore(std::streamsize bits = 1)
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

    /**
        Align get pointer to next bit multiple if not already at one.

        \param[in] bit Bit multiple, such as 8 for byte alignment.
        \return This bit stream.
    */
    istream &aligng(size_t bit)
    {
        BOOST_ASSERT(bit > 0);
        if (good() && bit > 0)
        {
            seekg(((static_cast<size_t>(tellg()) + bit - 1) / bit) * bit);
        }

        m_gcount = 0;

        return *this;
    }

    /**
        Set repeat count for subsequent vector extractions.

        \note This function does not extract anything from istream. It
        merely saves a value that any subsequent vector extractions use to know
        how many bit fields to extract into the same number of vector elements.

        \param repeat Number of bit fields to extract to each subsequent
        vector.
        \return This bit stream.
    */
    istream &repeat(size_t repeat)
    {
        m_repeat = repeat;

        return *this;
    }

    /**
        Determine whether get pointer is aligned to bit multiple.

        Example:
        \code
        static const bitset<2> version(0x2);
        bitset<4> csrcCount;
        bool marker;
        bitset<7> payloadType;
        DWORD timestamp, ssrcIdentifier;
        ibitstream bin(rtpHeader);
        bin >> version >> aligng(4) >> csrcCount;
        assert(bin.alignedg(8));
        // (aligng is redundant here because already at byte alignment)
        bin >> aligng(8) >> marker >> payloadType;
        bin >> timestamp >> ssrcIdentifier;
        \endcode

        \param[in] bit Bit multiple, such as 8 for byte alignment.
        \return Whether get pointer is bit-multiple aligned.
    */
    bool alignedg(size_t bit)
    {
        return tellg() % bit == 0;
    }

    /**
        Get next bit from stream without advancing get pointer.

        \return Next bit from stream.
    */
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

    /**
        Get bits from stream.

        \param[out] value Integral to receive bits from stream.
        \param[in] bits Number of bits to read.
        \return This bit stream.
    */
    istream &read(bitfield &value, std::streamsize bits)
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

    /**
        Get "some" bits from stream.

        \note This function is provided for completeness. A bit stream residing
        in memory does not possess the behavior that, unlike istringstream,
        would make readsome() behave any different from read().

        \param[out] value Integral to receive bits from stream.
        \param[in] bits Number of bits to read.
        \return This bit stream.
    */
    istream &readsome(bitfield &value, std::streamsize bits)
    {
        BOOST_ASSERT(bits >= 0);

        return read(value, bits);
    }

    /**
        Set position of get pointer relative to indicated internal pointer.

        \param[in] offset Relative offset from indicated pointer.
        \param[in] dir Bit pointer to which offset is applied.
        \return This bit stream.
    */
    istream &seekg(std::streamoff offset, std::ios_base::seek_dir dir)
    {
        if (rdbuf()->pubseekoff(offset, dir) ==
            static_cast<std::streampos>(bitbuf::npos))
        {
            failbit();
        }

        return *this;
    }

    /**
        Set position of get pointer.

        \param[in] position Bit position.
        \return This bit stream.
    */
    istream &seekg(std::streampos position)
    {
        BOOST_ASSERT(position >= 0);

        if (rdbuf()->pubseekpos(position) ==
            static_cast<std::streampos>(bitbuf::npos))
        {
            failbit();
        }

        return *this;
    }

    /**
        Synchronize input buffer with source of bits.

        \return 0 if buffered stream and successful; -1 otherwise.
    */
    int sync()
    {
        return rdbuf()->pubsync();
    }

    /**
        Get position of get pointer.

        \return Bit position of next bit to be read.
    */
    std::streampos tellg()
    {
        return rdbuf()->pubseekoff(0, std::ios_base::cur);
    }

    /**
        Move get pointer backwards and return bit at new position.

        \return This bit stream.
    */
    istream &unget()
    {
        m_gcount = 0;

        if (static_cast<decltype(bitbuf::eof)>(
            rdbuf()->pubseekoff(-1, std::ios_base::cur)) == bitbuf::eof)
        {
            failbit();
        }

        return *this;
    }

protected:
    /**
        Number of bits extracted by last input operation.
    */
    std::streamsize m_gcount;

    /**
        Value extracted by last input operation.
    */
    bitfield m_gvalue;

    /**
        Number of bit fields extracted to a container.
    */
    size_t m_repeat;

private:
    /**
        Friend const functions for access to badbit().
    */
    ///@{
    template <typename T> friend istream &operator>>(istream &ibs,
        const T &b);
    template <size_t N> friend istream &operator>>(istream &ibs,
        const std::bitset<N> &bs);
    friend istream &operator>>(istream &ibs, const bool &b);
    ///@}
};

// Operator overloads /////////////////////////////////////////////////////////

/**
    Get single bit from input stream and place in bool.

    \param[in,out] ibs Reference to istream on left-hand side of operator.
    \param[out] b bool on right-hand side of operator.
    \return Reference to istream parameter.
*/
inline istream &operator>>(istream &ibs, bool &b)
{
    // Extract bit
    bitfield value;
    ibs.read(value, 1);
    b = value != 0;

    return ibs;
}

/**
    Get single bit from input stream that must be equal to bool.

    \param[in,out] ibs Reference to istream on left-hand side of operator.
    \param[in] b bool on right-hand side of operator.
    \return Reference to istream parameter.
*/
inline istream &operator>>(istream &ibs, const bool &b)
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

/**
    Get bit field from input stream and place in integral.

    \param[in,out] ibs Reference to istream on left-hand side of operator.
    \param[out] b Integral on right-hand side of operator.
    \return Reference to istream parameter.
*/
template <typename T>
istream &operator>>(istream &ibs, T &b)
{
    bitfield value;
    ibs.read(value, sizeof(T) * CHAR_BIT);
    b = static_cast<T>(value);

    return ibs;
}

/**
    Get bit field from input stream that must be equal to integral value.

    \param[in,out] ibs Reference to istream on left-hand side of operator.
    \param[in] b Integral on right-hand side of operator.
    \return Reference to istream parameter.
*/
template <typename T>
istream &operator>>(istream &ibs, const T &b)
{
    bitfield value;
    ibs.read(value, sizeof(T) * CHAR_BIT);
    if (b != value)
    {
        ibs.badbit();
    }

    return ibs;
}

/**
    Get bit fields from input stream and place in integral vector.

    \note Starting with the first element, this function populates existing
    elements in the vector with bit fields sequentially extracted from the
    input stream. It does not increase the size of the vector, e.g., with
    push_back(), because it relies on the existing size of the vector to know
    how many bit fields to extract.

    \todo I thought this used m_repeat, not the size of the vector. Look into.

    \param[in,out] ibs Reference to istream on left-hand side of operator.
    \param[out] v Integral vector on right-hand side of operator.
    \return Reference to istream parameter.
*/
template <typename T>
istream &operator>>(istream &ibs, std::vector<T> &v)
{
    decltype(v.size()) vector_size = v.size();
    for (decltype(vector_size) i = 0; i < vector_size; ++i) {
        ibs >> v[i];
    }

    return ibs;
}

/**
    Get bit fields from input stream that must be equal to elements in integral
    vector.

    \see Size note for non-const version of this function.

    \todo I thought this used m_repeat, not the size of the vector. Look into.

    \param[in,out] ibs Reference to istream on left-hand side of operator.
    \param[in] v Integral vector on right-hand side of operator.
    \return Reference to istream parameter.
*/
template <typename T>
istream &operator>>(istream &ibs, std::vector<const T> &v)
{
    decltype(v.size()) vectorSize = v.size();
    for (decltype(vectorSize) i = 0; i < vectorSize; ++i) {
        ibs >> v[i];
    }

    return ibs;
}

/**
    Get bits from input stream and place in bitset.

    \param[in,out] ibs Reference to istream on left-hand side of operator.
    \param[out] bs bitset on right-hand side of operator.
    \return Reference to istream parameter.
*/
template <size_t N>
istream &operator>>(istream &ibs, std::bitset<N> &bs)
{
    decltype(bs.to_ulong()) value;
    ibs.read(value, N);
    bs = static_cast<long>(value);
    BOOST_ASSERT(bs.to_ulong() == value);

    return ibs;
}

/**
    Get bits from input stream that must be equal to bitset value.

    \param[in,out] ibs Reference to istream on left-hand side of operator.
    \param[in] bs bitset on right-hand side of operator.
    \return Reference to istream parameter.
*/
template <size_t N>
istream &operator>>(istream &ibs, const std::bitset<N> &bs)
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