/** \file
    \brief Parametric manipulators for bit streams.
    \details This header file contains parametric manipulators for bit streams.
    \see http://www.boost.org/ for latest version.
    \see http://www.boost.org/libs/bitstream for documentation.

    Use, modification, and distribution is subject to the Boost Software
        License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef BOOST_BITSTREAM_IOMANIP_HPP
#define BOOST_BITSTREAM_IOMANIP_HPP

#include <boost/bitstream/istream.hpp>
#include <boost/bitstream/ostream.hpp>

namespace boost {

namespace bitstream {

///@{
/**
    This class represents the setrepeat bit-stream manipulator.

    \note This is a roundabout but necessary way of implementing manipulators
    that take parameters, such as the std::setprecision() manipulator.
*/
class setrepeat
{
public:
    /**
        Constructor.

        \param[in] repeat Number of bit fields to extract/insert to each
		subsequent bit-field container.
    */
    setrepeat(size_t repeat) : m_repeat(repeat)
    {
        // Do nothing.
    }

    /**
        Overload for the (istream &) operator on this class.

        \param[in,out] ibs Reference to istream on lhs of >> operator.
        \return Reference to istream parameter.
    */
    istream &operator()(istream &ibs) const
    {
        return ibs.repeat(m_repeat);
    }

	/**
		Overload for the (ostream &) operator on this class.

		\param[in,out] obs Reference to ostream on lhs of >> operator.
		\return Reference to ostream parameter.
	*/
	ostream &operator()(ostream &obs) const
	{
		return obs.repeat(m_repeat);
	}

private:
    /**
        Number of bit fields to extract/insert per container.
    */
    size_t m_repeat;
};

/**
    Manipulator for istream that sets repeat count for subsequent container
    extractions.

    \param[in,out] ibs Reference to istream on left-hand side of operator.
    \param[in] repeat Instance of setrepeat class.
    \return Reference to istream parameter.
*/
inline istream &operator>>(istream &ibs, setrepeat repeat)
{
    return repeat(ibs);
}

/**
	Manipulator for ostream that sets repeat count for subsequent container
	insertions.

	\param[in,out] obs Reference to ostream on left-hand side of operator.
	\param[in] repeat Instance of setrepeat class.
	\return Reference to ostream parameter.
*/
inline ostream &operator>>(ostream &obs, setrepeat repeat)
{
	return repeat(obs);
}
///@}

///@{
/**
    This class represents the ignore bit-stream manipulator.

    \see Implementation note for setrepeat manipulator.
*/
class ignore
{
public:
    /**
        Constructor.

        \param[in] bits Number of bits to ignore in bit stream.
    */
    ignore(size_t bits) : m_bits(bits)
    {
        // Do nothing.
    }

    /**
        Overload for the () operator on this class.

        \param[in,out] ibs Reference to istream on lhs of >> operator.
        \return Reference to istream parameter.
    */
    istream &operator()(istream &ibs) const
    {
        return ibs.ignore(m_bits);
    }

private:
    /**
        Number of bit fields to ignore in bit stream.
    */
    size_t m_bits;
};

/**
    Manipulator for istream that ignores bits.

    \param[in,out] ibs Reference to istream on left-hand side of operator.
    \param[in] repeat Instance of ignore class.
    \return Reference to istream parameter.
*/
inline istream &operator>>(istream &ibs, ignore skip)
{
    return skip(ibs);
}
///@}

///@{
/**
    This class represents the aligng bit-stream manipulator.

    \see Implementation note for setrepeat manipulator.

    Examples:
    \code
    // Advance get pointer to next nibble.
    static const bitset<2> version(0x2);
    bitset<4> csrcCount;
    bool marker;
    bitset<7> payloadType;
    DWORD timestamp, ssrcIdentifier;
    ibitstream bin(rtpHeader);
    bin >> version >> aligng(4) >> csrcCount >> marker >> payloadType;
    bin >> timestamp >> ssrcIdentifier;
    \endcode

    \code
    // Advance get pointer to next word.
    bitset<16> sequenceNumber;
    ibitstream(rtpHeader).aligng(sizeof(WORD) * std::numeric_limits<unsigned char>::digits) >> sequenceNumber;
    \endcode

    \code
    // Advance get pointer to next double word.
    bool b;
    static const bitset<2> version(0x2);
    bitset<4> csrcCount;
    bitset<7> payloadType;
    DWORD timestamp;
    ibitstream bin(rtpHeader);
    bin >> version >> b >> b >> csrcCount >> b >> payloadType
        >> aligng(32) >> timeStamp;
    \endcode
*/
class aligng
{
public:
    /**
        Constructor.

        \param[in] bits Number of bits at which to align the get pointer.
    */
    aligng(size_t bits) : m_bits(bits)
    {
        BOOST_ASSERT(bits > 0);
    }

    /**
        Overload for the () operator on this class.

        \param[in,out] ibs Reference to istream on lhs of >> operator.
        \return Reference to istream parameter.
    */
    istream &operator()(istream &ibs) const
    {
        return ibs.aligng(m_bits);
    }

private:
    /**
        Number of bit at which to align the get pointer.
    */
    size_t m_bits;
};

/**
    Manipulator for istream that aligns the get pointer in the input stream.

    \param[in,out] ibs Reference to istream on left-hand side of operator.
    \param[in] align Instance of aligng class.
    \return Reference to istream parameter.
*/
inline istream &operator>>(istream &ibs, aligng align)
{
    return align(ibs);
}
///@}

} // namespace bitstream

} // namespace boost

#endif