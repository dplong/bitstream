/** \file
    \brief Bit-stream classes.
    \details This header file contains the bit-stream classes.
    \see http://www.boost.org/ for latest version.
    \see http://www.boost.org/libs/bitstream for documentation.

    Use, modification, and distribution is subject to the Boost Software
        License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef BOOST_BITSTREAM_BSTREAM_HPP
#define BOOST_BITSTREAM_BSTREAM_HPP

// Includes ///////////////////////////////////////////////////////////////////

#include <bitset>
#include <boost/typeof/typeof.hpp>
#include <boost/bitstream/istream.hpp>
#include <boost/bitstream/ostream.hpp>

// This code uses BOOST_TYPEOF(). BOOST only knows about C++'s core types, so
// we have to tell it about streampos.
#include BOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()
BOOST_TYPEOF_REGISTER_TYPE(std::streampos)

namespace boost {

namespace bitstream {

// Constants //////////////////////////////////////////////////////////////////

// Typedefs ///////////////////////////////////////////////////////////////////

// ibitstream /////////////////////////////////////////////////////////////////

/**
    This class provides an interface to manipulate bits as an input stream.

    \note This class is based on but does not inherit from the standard class,
    istringstream. The main difference is that this class provides access to
    bits whereas istringstream provides access to characters.
*/
class ibitstream : public istream
{
public:
    /**
        Constructor.

		\note The assumption is that a buffer will be provided later via
		data(char *buffer, std::streamsize size).

        \param[in] which Open mode.
    */
    explicit ibitstream(std::ios_base::openmode which = std::ios_base::in) :
        m_bitbuf(which), istream(&m_bitbuf)
    {
        // Append to input? Huh?
        BOOST_ASSERT((which & std::ios_base::app) == 0);
        // Input at end of stream? Huh?
        BOOST_ASSERT((which & std::ios_base::ate) == 0);
        // Truncate not currently supported.
        BOOST_ASSERT((which & std::ios_base::trunc) == 0);
    }

    /**
        Constructor.

        \param[in] buffer Pointer to char array to be accessed.
        \param[in] size_ Number of accessible bits in char array.
        \param[in] which Open mode.
    */
    explicit ibitstream(const char *buffer,
		std::streamsize size_ = (std::numeric_limits<std::streamsize>::max)(),
        std::ios_base::openmode which = std::ios_base::in) :
        m_bitbuf(buffer, size_, which), istream(&m_bitbuf)
    {
        BOOST_ASSERT(buffer != NULL);
        BOOST_ASSERT(size_ >= 0);
        // Append to input? Huh?
        BOOST_ASSERT((which & std::ios_base::app) == 0);
        // Input at end of stream? Huh?
        BOOST_ASSERT((which & std::ios_base::ate) == 0);
        // Truncate not currently supported.
        BOOST_ASSERT((which & std::ios_base::trunc) == 0);
    }

    /**
        Get the bitbuf object associated with the stream upon construction.

        \return A pointer to the bitbuf object associated with the stream.
    */
    bitbuf *rdbuf() const
    {
        return const_cast<BOOST_TYPEOF(rdbuf())>(&m_bitbuf);
    }

    /**
        Get pointer to current contents of the stream.

        \note This is analogous to istringstream::str().

        \return Pointer to stream buffer.
    */
    const char *data() const
    {
        return iob::rdbuf()->data();
    }

	/**
		Set contents of the stream.

		\note This is analogous to istreagstream::str(const string &s).

		\param[in] buffer Pointer to new char array to be accessed.
		\param[in] size_ Number of accessible bits in new char array.
		*/
	void data(char *buffer,
		std::streamsize size_ = (std::numeric_limits<BOOST_TYPEOF(size_)>::max)())
	{
		m_bitbuf.data(buffer, size_);
	}

private:
    /**
        Buffer from which this class serially extracts bits.
    */
    bitbuf m_bitbuf;
};

// obitstream /////////////////////////////////////////////////////////////////

/**
    This class provides an interface to manipulate bits as an output stream.

    \note This class is based on but does not inherit from the standard class,
    ostringstream. The main difference is that this class provides access to
    bits whereas ostringstream provides access to characters.
*/
class obitstream : public ostream
{
public:
    /**
        Constructor.

		\note The assumption is that a buffer will be provided later via
		data(char *buffer, std::streamsize size).

        \param[in] which Open mode.
    */
    explicit obitstream(std::ios_base::openmode which = std::ios_base::out) :
        m_bitbuf(which), ostream(&m_bitbuf)
    {
        // Append to input? Huh?
        BOOST_ASSERT((which & std::ios_base::app) == 0);
        // Input at end of stream? Huh?
        BOOST_ASSERT((which & std::ios_base::ate) == 0);
        // Truncate not currently supported.
        BOOST_ASSERT((which & std::ios_base::trunc) == 0);
    }

    /**
        Constructor.

        \param[in] buffer Pointer to char array to be accessed.
        \param[in] size_ Number of accessible bits in char array.
        \param[in] which Open mode.
    */
    explicit obitstream(const char *buffer,
		std::streamsize size_ = (std::numeric_limits<std::streamsize>::max)(),
        std::ios_base::openmode which = std::ios_base::out) :
        m_bitbuf(buffer, size_, which), ostream(&m_bitbuf)
    {
        BOOST_ASSERT(buffer != NULL);
        BOOST_ASSERT(size_ >= 0);
        // Append to input? Huh?
        BOOST_ASSERT((which & std::ios_base::app) == 0);
        // Input at end of stream? Huh?
        BOOST_ASSERT((which & std::ios_base::ate) == 0);
        // Truncate not currently supported.
        BOOST_ASSERT((which & std::ios_base::trunc) == 0);
    }

    /**
        Get pointer to current contents of the stream.

        \note This is analogous to ostringstream::str().

        \return Pointer to stream buffer.
    */
    const char *data() const
    {
        return iob::rdbuf()->data();
    }

	/**
		Set contents of the stream.

		\note This is analogous to ostringstream::str(const string &s).

		\param[in] buffer Pointer to new char array to be accessed.
		\param[in] size_ Number of accessible bits in new char array.
	*/
	void data(char *buffer,
		std::streamsize size_ = (std::numeric_limits<BOOST_TYPEOF(size_)>::max)())
	{
		m_bitbuf.data(buffer, size_);
	}

private:
    /**
        Buffer to which this class serially writes bits.
    */
    bitbuf m_bitbuf;
};

} // namespace bitstream

} // namespace boost

#endif
