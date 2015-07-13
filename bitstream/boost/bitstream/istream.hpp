/** \file
    \brief Input bit-stream classes.
    \details This header file contains the input bit-stream classes.
    \see http://www.boost.org/ for latest version.
    \see http://www.boost.org/libs/bitstream for documentation.

    Use, modification, and distribution is subject to the Boost Software
        License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef BOOST_BITSTREAM_ISTREAM_HPP
#define BOOST_BITSTREAM_ISTREAM_HPP

#include <boost/bitstream/iob.hpp>
#include <boost/spirit/home/support/container.hpp>

namespace boost {

namespace bitstream {

/**
    Input stream objects of this class can read and interpret sequences of bits.
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
		Get repeat value.

		\note If this value is 0, repetitive vector extraction occurs based on
		vector size. Otherwise, this many extractions are done.

		\return Repeat value.
	*/
	size_t repeat() const
	{
		return m_repeat;
	}

    /**
        Get one bit from stream.

        \return Next bit from stream.
    */
    int get()
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

        return static_cast<int>(m_gvalue);
    }

	/**
		Get one bit from stream.

		\param[out] value Integral to receive bit from stream.
		\return This bit stream.
		*/
	istream &get(bitfield &value)
	{
		value = get();

		return *this;
	}

	/**
		Get bits from stream.

		\param[out] value Integral to receive bits from stream.
		\param[in] bits Number of bits to read.
		\return This bit stream.
	*/
	istream &get(bitfield &value, std::streamsize n)
	{
		return read(value, n);
	}

	/**
		Reads available bits and inserts them to the bitbuf object.

		\note Bits are extracted and inserted into bb until end of file
		condition occurs in the input sequence or inserting into the output
		sequence fails (in which case the bit that could not be inserted, is
		not extracted).

		\param[in] bb bitbuf object into which bits are inserted.
		\return This bit stream.
	*/
	istream &get(bitbuf &bb)
	{
		// TBD Implement get().
		return *this;
	}

    /**
        Ignore, or skip over, bits in stream.

        \param[in] bits Number of bits to ignore.
        \return This bit stream.
    */
    istream &ignore(std::streamsize bits = 1)
    {
        if (rdbuf()->pubseekoff(bits, std::ios_base::cur, std::ios_base::in) == std::streampos(-1))
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
        if (good() && bit > 0)
        {
            seekg(((static_cast<size_t>(tellg()) + bit - 1) / bit) * bit);
        }

        m_gcount = 0;

        return *this;
    }

    /**
        Set repeat count for subsequent vector extractions.

        \note This function does not extract anything from istream. It merely
		saves a value that any subsequent vector extractions use to know how
		many bit fields to extract into the same number of vector elements.

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
		std::streamsize bits_read = rdbuf()->sgetn(value, bits);
        if (bits_read != bits)
        {
			// This read failed. Was it because there aren't enough available bits?
			if (rdbuf()->in_avail() < bits)
			{
				eofbit();
			}
			failbit();
            m_gcount = 0;
            value = 0;
        }
        else
        {
			// This read succeeded, but have we reached eof (without going past it)?
			if (rdbuf()->in_avail() == 0)
			{
				eofbit();
			}
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
        return read(value, bits);
    }

	/**
		Put bit back.

		\note Put bit back in stream so that it can be read.

		\param[in] value Bit to put back in stream.
		\return This bit stream.
	*/
	istream& putback(bitfield value)
	{
// http://en.cppreference.com/w/cpp/io/basic_istream/putback says do this:
#if 0
		clear(rdstate() & ~std::ios_base::eofbit);
		if (rdbuf() == NULL || !rdbuf()->sputbackb(value))
		{
			setstate(std::ios_base::badbit);
		}
// while http://www.cplusplus.com/reference/istream/basic_istream/putback/ says do this this:
#else
		if (eof())
		{
			failbit();
		}
		else
		{
			if (!rdbuf()->sputbackb(value))
			{
				badbit();
			}
		}
#endif
		m_gcount = 0;

		return *this;
	}

    /**
        Set position of get pointer relative to indicated internal pointer.

        \param[in] offset Relative offset from indicated pointer.
        \param[in] dir Bit pointer to which offset is applied.
        \return This bit stream.
    */
    istream &seekg(std::streamoff offset, std::ios_base::seek_dir dir)
    {
		if (eof() || rdbuf()->pubseekoff(offset, dir, std::ios_base::in) == std::streampos(-1))
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
        if (eof() || rdbuf()->pubseekpos(position, std::ios_base::in) == std::streampos(-1))
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
        return rdbuf()->pubseekoff(0, std::ios_base::cur, std::ios_base::in);
    }

    /**
        Move get pointer backwards and return bit at new position.

        \return This bit stream.
    */
    istream &unget()
    {
        m_gcount = 0;

        if (rdbuf()->pubseekoff(-1, std::ios_base::cur, std::ios_base::in) == std::streampos(-1))
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
	template <size_t N> friend istream &operator>>(istream &ibs,
        const std::bitset<N> &bs);
	friend istream &operator>>(istream &ibs, const bool &b);
    ///@}
};

/**
	The value member of this class expresses whether the type has the member
	function, identified by name and signature.

	\param[in] traitsName Desired name for this new trait type.
	\param[in] funcName Name of function member to look for.
	\param[in] funcSig Signature of function member to look for.
	\param[out] value Whether the function member is present in the class.
*/
#define DEFINE_HAS_SIGNATURE(traitsName, funcName, funcSig)                 \
    template <typename U>                                                   \
    class traitsName                                                        \
    {                                                                       \
        typedef boost::uint8_t yes; typedef boost::uint16_t no;             \
        template<typename T, T> struct helper;                              \
        template<typename T> static yes check(helper<funcSig, &funcName>*); \
        template<typename T> static no check(...);                          \
    public:                                                                 \
        static const bool value = sizeof check<U>(0) == sizeof(yes);        \
    }

/**
	The value member of the defined class expresses whether the type has the
	member function, void resize(size_type).

	\note This is the signature of resize() added in C++11.

	\pre Type is a container class
	(boost::spirit::traits::is_container<C>::value == true).

	\param[out] value Whether resize() is present in the class.
*/
DEFINE_HAS_SIGNATURE(has_resize_1, T::resize, void (T::*)(typename T::size_type));

/**
	The value member of the defined class expresses whether the type has the
	member function, void resize(size_type).

	\note This is the signature of resize() that should be present in all STL
	versions.

	\pre Type is a container class
	(boost::spirit::traits::is_container<C>::value == true).

	\param[out] value Whether resize() is present in the class.
*/
DEFINE_HAS_SIGNATURE(has_resize_2, T::resize, void (T::*)(typename T::size_type, \
	typename T::value_type));

/**
	Deterine whether this type has a resize function with at least one of two
	of its signatures.

	\pre Type is a container class
	(boost::spirit::traits::is_container<C>::value == true).

	\param[out] value Whether member function, resize(), is present in type.
*/
template <typename U>
struct has_resize
{
	// Need to check both because the single-parameter signature was added in C++11.
	static const bool value = has_resize_1<U>::value || has_resize_2<U>::value;
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
	bool value;
	ibs >> value;
	if (b != value)
    {
        ibs.failbit();
    }

    return ibs;
}

// Templates //////////////////////////////////////////////////////////////////

// Templates for scalars //////////////////////////////////////////

/**
	Get bits from input stream and place in bitset.

	\param[in,out] ibs Reference to istream on left-hand side of operator.
	\param[out] bs bitset on right-hand side of operator.
	\return Reference to istream parameter.
*/
template <size_t N>
istream &operator>>(istream &ibs, std::bitset<N> &bs)
{
	bitfield value;
	ibs.read(value, N);
	bs = value;

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
	BOOST_TYPEOF(bs) value;
	ibs >> value;
	if (bs != value)
	{
		ibs.failbit();
	}

	return ibs;
}

/**
	Get bit field from input stream and place in integral.

	\param[in,out] ibs Reference to istream on left-hand side of operator.
	\param[out] b Integral on right-hand side of operator.
	\return Reference to istream parameter.
*/
template <typename T>
typename boost::enable_if_c<
	!boost::spirit::traits::is_container<T>::value,
	istream &>::type
	operator>>(istream &ibs, T &b)
{
	bitfield value;
	ibs.read(value, sizeof b * CHAR_BIT);
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
typename boost::enable_if_c<
	!boost::spirit::traits::is_container<T>::value,
	istream &>::type
	operator>>(istream &ibs, const T &b)
{
	BOOST_TYPEOF(b) value;
	ibs >> value;
	if (b != value)
	{
		ibs.failbit();
	}

	return ibs;
}

// Templates for sequence containers //////////////////////////////////////////

/**
	Get bit fields from input stream that must be equal to elements in
	container.

	\see Unlike the non-const version of this operator overload,
	istream::repeat() does not affect this overload. The container must already
	be sized and populated with the values it requires.

	\param[in,out] ibs Reference to istream on left-hand side of operator.
	\param[in] v Container on right-hand side of operator.
	\return Reference to istream parameter.
*/
template <typename C>
typename boost::enable_if_c<
	boost::spirit::traits::is_container<C>::value,
	istream &
>::type
operator>>(istream &ibs, const C &c)
{
	for (BOOST_AUTO_TPL(it, c.begin()); it != c.end(); ++it)
	{
		ibs >> *it;
	}

	return ibs;
}

// Templates for variable-size sequence containers ////////////////////////////

/**
	Get bit fields from input stream and place in container.

	\note If istream::repeat() returns 0, repetitive extraction occurs based on
	container size. Otherwise, this many bit fields are added to the container,
	making the container the same size as the repeat value.

	\param[in,out] ibs Reference to istream on left-hand side of operator.
	\param[out] v Container on right-hand side of operator.
	\return Reference to istream parameter.
*/
template <typename C>
typename boost::enable_if_c<
	boost::spirit::traits::is_container<C>::value && has_resize<C>::value,
	istream &
>::type
operator>>(istream &ibs, C &c)
{
	c.resize(ibs.repeat() == 0 ? c.size() : ibs.repeat());
	for (BOOST_AUTO_TPL(it, c.begin()); it != c.end(); ++it)
	{
		// (This is a workaround specifcally for std::vector<bool>. Otherwise,
		// I'd extract directly into the dereferenced iterator, i.e.,
		// ibs >> *it.)
		C::value_type v;
		ibs >> v;
		*it = v;
	}

	return ibs;
}

// Templates for fixed-size sequence containers ///////////////////////////////

/**
	Get bit fields from input stream and place in container.

	\note If istream::repeat() returns 0, repetitive extraction occurs based on
	container size. Otherwise, this many bit fields are added to the container,
	making the container the same size as the repeat value.

	\param[in,out] ibs Reference to istream on left-hand side of operator.
	\param[out] v Container on right-hand side of operator.
	\return Reference to istream parameter.
*/
template <typename C>
typename boost::enable_if_c<
	boost::spirit::traits::is_container<C>::value && !has_resize<C>::value,
	istream &
>::type
operator>>(istream &ibs, C &c)
{
	for (BOOST_AUTO_TPL(it, c.begin()); it != c.end(); ++it)
	{
		ibs >> *it;
	}

	return ibs;
}

} // namespace bitstream

} // namespace boost

#endif