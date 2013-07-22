/** \file
    \brief Output bit-stream classes.
    \details This header file contains the output bit-stream classes.
    \copyright Copyright (C) 2013 Paul Long.
    \note Use, modification, and distribution is subject to the Boost Software
        License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
    \see http://www.boost.org/ for latest version.
    \see http://www.boost.org/libs/bitstream for documentation.
*/

#ifndef BOOST_BITSTREAM_OSTREAM_HPP
#define BOOST_BITSTREAM_OSTREAM_HPP

#include <boost/bitstream/iob.hpp>

namespace boost {

namespace bitstream {
/**
    Output stream objects of this class can write sequences of bits.

    \todo Once implemented, implement putback()
*/
class ostream : public iob
{
public:
    /**
        Constructor.
    */
    explicit ostream(bitbuf *bb) : iob(bb)
    {
        // Do nothing.
    }

    /**
        Put one bit to stream.

        \param[in] value Value to write to stream.
        \return Reference to this object?
    */
    ostream &put(bitfield value)
    {
        // TBD
        return *this;
    }

    /**
        Align put pointer to next bit multiple if not already at one.

        \param[in] bit Bit multiple, such as 8 for byte alignment.
        \return This bit stream.
    */
    ostream &alignp(size_t bit)
    {
        // TBD
        return *this;
    }

    /**
        Determine whether put pointer is aligned to bit multiple.

        \param[in] bit Bit multiple, such as 8 for byte alignment.
        \return Whether put pointer is bit-multiple aligned.
    */
    bool alignedp(size_t bit)
    {
        return tellp() % bit == 0;
    }

    /**
        Write bits to stream.

        \param[out] value Integral from which to write bits to stream.
        \param[in] bits Number of bits to write.
        \return This bit stream.
    */
    ostream &write(bitfield value, std::streamsize bits)
    {
        // TBD
        return *this;
    }

    /**
        Set position of put pointer relative to indicated internal pointer.

        \param[in] offset Relative offset from indicated pointer.
        \param[in] dir Bit pointer to which offset is applied.
        \return This bit stream.
    */
    ostream &seekp(std::streamoff offset, std::ios_base::seek_dir dir)
    {
        // TBD
        return *this;
    }

    /**
        Set position of put pointer.

        \param[in] position Bit position.
        \return This bit stream.
    */
    ostream &seekp(std::streampos position)
    {
        // TBD
        return *this;
    }

    /**
        Get position of put pointer.

        \return Bit position of next bit to be written.
    */
    std::streampos tellp()
    {
        // TBD
        return 0;
    }

    /**
        Synchronize output buffer with destination of bits.

        \return This bit stream.
    */
    ostream &flush()
    {
        // TBD
        return *this;
    }

private:
    /**
        Friend const functions for access to badbit().
    */
    ///@{
    template <typename T> friend ostream &operator<<(ostream &obs,
        T b);
    template <size_t N> friend ostream &operator<<(ostream &obs,
        const std::bitset<N> &bs);
    friend ostream &operator<<(ostream &obs, bool b);
    ///@}
};

// Operator overloads /////////////////////////////////////////////////////////

/**
    Put single bit as bool to output stream.

    \param[in,out] obs Reference to ostream on left-hand side of operator.
    \param[out] b bool on right-hand side of operator.
    \return Reference to ostream parameter.
*/
inline ostream &operator<<(ostream &obs, bool b)
{
    // TBD - Is cast necessary?
    return obs.write(static_cast<bitfield>(b != 0), 1);
}

// Templates //////////////////////////////////////////////////////////////////

/**
    Put integral bit field to input stream.

    \param[in,out] obs Reference to ostream on left-hand side of operator.
    \param[out] b Integral on right-hand side of operator.
    \return Reference to ostream parameter.
*/
template <typename T>
ostream &operator<<(ostream &obs, T b)
{
    // TBD - Is cast necessary?
    return obs.write(static_cast<bitfield>(b), sizeof(T) * CHAR_BIT);
}

/**
    Put integral vector to output stream.

    \param[in,out] obs Reference to ostream on left-hand side of operator.
    \param[out] v Integral vector on right-hand side of operator.
    \return Reference to ostream parameter.
*/
template <typename T>
ostream &operator<<(ostream &obs, const std::vector<T> &v)
{
    BOOST_FOREACH(const T &datum, v)
    {
        obs << datum;
    }

    return obs;
}

/**
    Put bits from bitset to output stream.

    \param[in,out] obs Reference to ostream on left-hand side of operator.
    \param[out] bs bitset on right-hand side of operator.
    \return Reference to ostream parameter.
*/
template <size_t N>
ostream &operator<<(ostream &obs, const std::bitset<N> &bs)
{
    // TBD - Is cast necessary?
    return obs.write(static_cast<bitfield &>(bs.to_ulong()), N);
}

} // namespace bitstream

} // namespace boost

#endif