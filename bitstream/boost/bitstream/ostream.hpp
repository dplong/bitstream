/** \file
    \brief Output bit-stream classes.
    \details This header file contains the output bit-stream classes.
    \see http://www.boost.org/ for latest version.
    \see http://www.boost.org/libs/bitstream for documentation.

    Use, modification, and distribution is subject to the Boost Software
        License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef BOOST_BITSTREAM_OSTREAM_HPP
#define BOOST_BITSTREAM_OSTREAM_HPP

#include <boost/bitstream/iob.hpp>
#include <boost/spirit/home/support/container.hpp>

namespace boost {

namespace bitstream {
/**
    Output stream objects of this class can write sequences of bits.
*/
class ostream : public iob
{
public:
    /**
        Constructor.
    */
    explicit ostream(bitbuf *bb) : iob(bb), m_repeat(0)
    {
        // Do nothing.
    }

	/**
		Get repeat value.

		\note If this value is 0, repetitive vector insertion occurs based on
		vector size. Otherwise, this many insertions are done.

		\return Repeat value.
	*/
	size_t repeat() const
	{
		return m_repeat;
	}

    /**
        Put one bit to stream.

        \param[in] value Bit to write to stream.
		\return This bit stream.
	*/
    ostream &put(bitfield value)
    {
		if (good() && !rdbuf()->sputb(value))
		{
			failbit();
		}

        return *this;
    }

	/**
		Ignore, or skip over, bits in stream.

		\param[in] bits Number of bits to ignore.
		\return This bit stream.
	*/
	ostream &ignore(std::streamsize bits = 1)
	{
		if (rdbuf()->pubseekoff(bits, std::ios_base::cur, std::ios_base::out) == std::streampos(-1))
		{
			eofbit();
		}

		return *this;
	}

    /**
        Align put pointer to next bit multiple if not already at one.

        \param[in] bit Bit multiple, such as 8 for byte alignment.
        \return This bit stream.
    */
    ostream &alignp(size_t bit)
    {
		if (good() && bit > 0)
		{
			seekp(((static_cast<size_t>(tellp()) + bit - 1) / bit) * bit);
		}

		return *this;
	}

	/**
		Set repeat count for subsequent vector insertions.

		\note This function does not insert anything to ostream. It merely
		saves a value that any subsequent vector insertions use to know how
		many bit fields to insert from the same number of vector elements.

		\param repeat Number of bit fields to extract to each subsequent
		vector.
		\return This bit stream.
	*/
	ostream &repeat(size_t repeat)
	{
		m_repeat = repeat;

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
		if (good() && !rdbuf()->sputn(value, bits))
		{
			badbit();
		}

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
		if (!fail() && rdbuf()->pubseekoff(offset, dir, std::ios_base::out) == std::streampos(-1))
		{
			failbit();
		}

        return *this;
    }

    /**
        Set position of put pointer.

        \param[in] position Bit position.
        \return This bit stream.
    */
    ostream &seekp(std::streampos position)
    {
		if (!fail() && rdbuf()->pubseekpos(position) == std::streampos(-1))
		{
			failbit();
		}
		
		return *this;
    }

    /**
        Get position of put pointer.

        \return Bit position of next bit to be written.
    */
    std::streampos tellp()
    {
		return fail() ? std::streampos(-1) :
			rdbuf()->pubseekoff(0, std::ios_base::cur, std::ios_base::out);
    }

    /**
        Synchronize output buffer with destination of bits.

		\note This currently does nothing because there are no intermediate
		buffers to write.

        \return This bit stream.
    */
    ostream &flush()
    {
        return *this;
    }

protected:
	/**
		Number of bit fields inserted from a container.
	*/
	size_t m_repeat;

private:
    /**
        Friend const functions for access to badbit().
    */
    ///@{
    template <size_t N> friend ostream &operator<<(ostream &obs,
        const std::bitset<N> &bs);
    friend ostream &operator<<(ostream &obs, const bool b);
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
    return obs.write(b != 0, 1);
}

// Templates //////////////////////////////////////////////////////////////////

// Templates for scalars //////////////////////////////////////////

/**
    Put bits from bitset to output stream.

    \param[in,out] obs Reference to ostream on left-hand side of operator.
    \param[out] bs bitset on right-hand side of operator.
    \return Reference to ostream parameter.
*/
template <size_t N>
ostream &operator<<(ostream &obs, std::bitset<N> &bs)
{
    return obs.write(bs.to_ulong(), N);
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
	return obs.write(bs.to_ulong(), N);
}

/**
	Put integral bit field to input stream.

	\param[in,out] obs Reference to ostream on left-hand side of operator.
	\param[out] b Integral on right-hand side of operator.
	\return Reference to ostream parameter.
*/
template <typename T>
typename boost::enable_if_c<
	!boost::spirit::traits::is_container<T>::value,
	ostream &
>::type
operator<<(ostream &obs, const T &b)
{
	return obs.write(static_cast<bitfield>(b), sizeof(T) * CHAR_BIT);
}

// Templates for sequence containers //////////////////////////////////////////

/**
	Put bit fields from containter to output stream.

	\param[in,out] obs Reference to ostream on left-hand side of operator.
	\param[in] c Container on right-hand side of operator.
	\return Reference to ostream parameter.
*/
template <typename C>
typename boost::enable_if_c<
	boost::spirit::traits::is_container<C>::value,
	ostream &
>::type
operator<<(ostream &obs, C &c)
{
	for (BOOST_AUTO_TPL(it, c.begin()); it != c.end(); ++it)
	{
		obs << *it;
	}

	return obs;
}

/**
	Put bit fields from containter to output stream.

	\param[in,out] obs Reference to ostream on left-hand side of operator.
	\param[in] c Container on right-hand side of operator.
	\return Reference to ostream parameter.
*/
template <typename C>
typename boost::enable_if_c<
	boost::spirit::traits::is_container<C>::value,
	ostream &
>::type
operator<<(ostream &obs, const C &c)
{
	for (BOOST_AUTO_TPL(it, c.begin()); it != c.end(); ++it)
	{
		obs << *it;
	}

	return obs;
}

} // namespace bitstream

} // namespace boost

#endif