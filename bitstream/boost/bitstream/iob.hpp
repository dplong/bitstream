/** \file
    \brief Base classes and types for bit streams.
    \details This header file contains base classes and types for the
        bit-stream hierarchy of classes.
    \details parametric manipulators for bit streams.
    \see http://www.boost.org/ for latest version.
    \see http://www.boost.org/libs/bitstream for documentation.

    Use, modification, and distribution is subject to the Boost Software
        License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef BOOST_BITSTREAM_IOB_HPP
#define BOOST_BITSTREAM_IOB_HPP

#include <boost/assert.hpp>
#include <iostream>
#include <ios>

namespace boost {

namespace bitstream {

// Typedefs ///////////////////////////////////////////////////////////////////

/** Integral type for bit-field values.
*/
typedef uintmax_t bitfield;

// bitbuf /////////////////////////////////////////////////////////////////////

/**
    This class represents contiguous memory, accessed as a sequence of bit
    fields.

    \note This class is based on but does not inherit from the standard class,
    stringbuf. The main difference is that this class provides access to bits
    whereas stringbuf provides access to characters.

    \note Currently, this class can only be used to read bits from a buffer;
    however, it could be easily be fleshed out for writing bits, too.
*/
class bitbuf
{
public:
    /**
        Constructor.

        \param[in] which Open mode.
    */
    explicit bitbuf(std::ios_base::openmode which =
        std::ios_base::in | std::ios_base::out) : m_buffer(NULL), m_gptr(0),
		m_egptr((std::numeric_limits<BOOST_TYPEOF(m_egptr)>::max)()),
		m_eback(0), m_pptr(0),
		m_epptr((std::numeric_limits<BOOST_TYPEOF(m_epptr)>::max)()),
		m_pbase(0)
    {
        // TBD Output not support, so can't append.
        BOOST_ASSERT((which & std::ios_base::app) == 0);
        // TBD Output not support, so can't append each time.
        BOOST_ASSERT((which & std::ios_base::ate) == 0);
        // TBD Truncate not currently supported.
        BOOST_ASSERT((which & std::ios_base::trunc) == 0);
    }

    /**
        Constructor.

        \param[in] buffer Pointer to char array to be accessed.
        \param[in] which Open mode.
        \param[in] size_ Number of accessible bits in char array.
    */
    bitbuf(const char *buffer,
		std::streamsize size_ = (std::numeric_limits<std::streamsize>::max)(),
        std::ios_base::openmode which =
        std::ios_base::in | std::ios_base::out) :
        m_buffer(reinterpret_cast<unsigned char *>(const_cast<char *>(
        buffer)))
    {
		if ((which & std::ios_base::in) != 0)
		{
			m_gptr = 0;
			m_egptr = size_;
			m_eback = 0;
		}

		if ((which & std::ios_base::out) != 0)
		{
			m_pptr = 0;
			m_epptr = size_;
			m_pbase = 0;
		}

        // Output not support, so can't append.
        BOOST_ASSERT((which & std::ios_base::app) == 0);
        // Output not support, so can't append each time.
        BOOST_ASSERT((which & std::ios_base::ate) == 0);
        // Truncate not currently supported.
        BOOST_ASSERT((which & std::ios_base::trunc) == 0);
    }

    /**
        Get pointer to char-array stream buffer.

        \note This and size_t size() const, together, are analogous to
		stringbuf::str().

        \return Pointer to stream buffer.
    */
    const char *data() const
    {
        return reinterpret_cast<char *>(m_buffer);
    }

	/**
		Get number of bits in char-array stream buffer returned by data().

		\note See note for const char *data();

		\return Number of bits in stream buffer.
	*/
	std::streamsize size() const
	{
		return egptr();
	}

    /**
        Set pointer to char-array stream buffer.

        \note This is analogous to stringbuf::str(x).

		\param[in] buffer Pointer to char array to be accessed.
		\param[in] size Number of accessible bits in char array.
	*/
    void data(char *buffer, std::streamsize size)
    {
		setbuf(reinterpret_cast<unsigned char *>(buffer), size);
    }

    /**
        Number of bits currently available to read.

        \return Number of readable bits.
    */
    std::streamsize in_avail() const
    {
        return egptr() - gptr();
    }

    /**
        Reposition get-next-bit pointer relative to current position.

        \param[in] offset Signed offset from current position for new position.
        \param[in] way From which pointer offset is applied for new position.
		\param[in] which Open mode.
		\return Position after offset applied.
    */
    std::streampos pubseekoff(std::streamoff offset,
        std::ios_base::seekdir way,
		std::ios_base::openmode which = std::ios_base::in | std::ios_base::out)
    {
        return seekoff(offset, way, which);
    }

    /**
        Reposition get-next-bit pointer.

        \param[in] position New bit position.
		\param[in] which Open mode.
		\return Position after repositioning.
    */
    std::streampos pubseekpos(std::streampos position,
		std::ios_base::openmode which = std::ios_base::in | std::ios_base::out)
    {
        return seekpos(position, which);
    }

    /**
        Set buffer to access.

        \param[in] buffer Pointer to char array to be accessed.
        \param[in] size Number of accessible bits in char array.
        \return Pointer to this object; NULL if error.
    */
    bitbuf *pubsetbuf(unsigned char *buffer, std::streamsize size)
    {
        return setbuf(buffer, size);
    }

    /**
        Synchronize stream buffer with input or output device.

        \return 0 if buffered stream and successful; -1 otherwise.
    */
    int pubsync()
    {
        return sync();
    }

    /**
        Get current bit and advance get pointer.

        \param[out] value Current bit before advancing pointer.
        \return Whether okay (eof has not been encountered).
    */
    bool sbumpb(bitfield &value)
    {
        return sgetn(value, 1) == 1;
    }

	/**
		Get current bit at get pointer.

		\param[out] value Current bit.
		\return Whether okay (eof has not been encountered).
	*/
	bool sgetb(bitfield &value)
	{
		bool okay = sbumpb(value);

		if (!okay)
		{
			pubseekoff(-1, std::ios_base::cur, std::ios_base::in);
		}

		return okay;
	}

    /**
        Get sequence of bits.

        \param[out] value Value of bit field.
        \param[in] size Number of bits in sequence of bits.
        \return Number of bits read from buffer or zero if error or eof.
    */
    std::streamsize sgetn(bitfield &value, std::streamsize size)
    {
        return xsgetn(value, size);
    }

    /**
        Advance get pointer and return next bit.

        \param[out] value Next bit.
        \return Whether okay (eof has not been encountered).
    */
    bool snextb(bitfield &value)
    {
		bitfield dummy;
		return sbumpb(dummy) && sgetb(value);
    }

	/**
		Put bit at get pointer.

		\param[out] b Bit to be put back.
		\return Whether okay (eof has not been encountered).
	*/
	bool sputbackb(bitfield b)
	{
		bool putback_succeeded;

		if (gptr() == std::streampos(-1) || gptr() == eback() ||
			b != atgptrb(-1))
		{
			putback_succeeded = pbackfail(b);
		}
		else
		{
			gbump(-1);
			putback_succeeded = gptr() != std::streampos(-1);
		}

		return putback_succeeded;
	}

	/**
		Put bit at put pointer.

		\param[out] b Current bit.
		\return Whether okay (eof has not been encountered).
	*/
	bool sputb(bitfield b)
	{
		bool put_succeeded = true;

		if (pptr() == std::streampos(-1) || pptr() == epptr())
		{
			put_succeeded = overflow(b);
		}
		else
		{
			const size_t intra_byte_bit_offset = m_pptr % CHAR_BIT;
			const size_t shift_amount = (CHAR_BIT - ((1 + intra_byte_bit_offset) % CHAR_BIT)) % CHAR_BIT;
			const bitfield mask = ~(1 << shift_amount);

			unsigned char * const byte_pointer = current_put_byte();
			const unsigned char destination_with_cleared_bits = *byte_pointer & mask;
			const unsigned char source_with_shifted_bits = (b & 1) << shift_amount;
			*byte_pointer = destination_with_cleared_bits | source_with_shifted_bits;

			pbump(1);
		}

		return put_succeeded;
	}

	/**
		Put sequence of bits.

		\param[out] value Value of bit field.
		\param[in] size Number of bits in sequence of bits.
		\return Number of bits written to buffer or zero if error or eof.
	*/
	std::streamsize sputn(bitfield &value, std::streamsize size)
	{
		return xsputn(value, size);
	}

    /**
        Move get pointer backwards and return bit at new position.

        \param[out] value Bit before position prior to call.
        \return Whether okay (eof has not been encountered).
    */
    bool sungetb(bitfield &b)
    {
		bool unget_succeeded;

		if (gptr() == std::streampos(-1) || gptr() == eback())
		{
			bitfield dummy;
			unget_succeeded = pbackfail(dummy);
		}
		else
		{
			gbump(-1);
			unget_succeeded = atgptrb(b);
		}

		return unget_succeeded;
    }

protected:
	// Input sequence (get) ///////////////////////////////////////////////////

	/**
        Returns first bit position in accessible input sequence.

        \return Position of first bit.
    */
    std::streampos eback() const
    {
        return m_eback;
    }

	/**
		Returns bit position within accessible input sequence of next bit to be
		read.

		\note The "get pointer."

		\return Next bit position.
	*/
	std::streampos gptr() const
	{
		return m_gptr;
	}

	/**
		Returns bit position just past last bit in accessible input sequence.

		\return Position after last bit.
	*/
	std::streampos egptr() const
	{
		return m_egptr;
	}

	/**
		Advances the get pointer by specified number of bit positions.

		\param[in] offset Value by which to increase the get pointer.
	*/
	void gbump(std::streamoff offset)
	{
		seekoff(offset, std::ios_base::cur, std::ios_base::in);
	}

	/**
        Set pointer and offsets that define boundaries of and position within
        accessible input sequence.

        \param[in] buffer Pointer to char array to be accessed.
        \param[in] gbeg Position of first accessible bit in char array.
        \param[in] gnext Position of next bit to read.
        \param[in] gend Position of bit immediately after last accessible bit
        in char array.
    */
    void setg(unsigned char *buffer, std::streampos gbeg, std::streampos gnext,
        std::streampos gend)
    {
        m_buffer = buffer;
        m_eback = gbeg;
        m_gptr = gnext;
        m_egptr = gend;
    }

	// Output sequence (put) //////////////////////////////////////////////////

	/**
		Returns first bit position in accessible output sequence.

		\return Position of first bit.
	*/
	std::streampos pbase() const
	{
		return 0;
	}

	/**
		Returns bit position within accessible output sequence of next bit to
		be read.

		\note The "put pointer."

		\return Next bit position.
	*/
	std::streampos pptr() const
	{
		return m_pptr;
	}

	/**
		Returns bit position just past last bit in accessible input sequence.

		\return Position after last bit.
	*/
	std::streampos epptr() const
	{
		return m_epptr;
	}

	/**
		Advances the put pointer by specified number of bit positions.

		\param[in] offset Value by which to decrease the get pointer.
	*/
	void pbump(std::streamoff offset)
	{
		seekoff(offset, std::ios_base::cur, std::ios_base::out);
	}

	/**
		Set pointer and offsets that define boundaries of and position within
		accessible output sequence.

		\param[in] buffer Pointer to char array to be accessed.
		\param[in] gbeg Position of first accessible bit in char array.
		\param[in] gend Position of bit immediately after last accessible bit
		in char array.
	*/
	void setp(unsigned char *buffer, std::streampos pbeg, std::streampos pend)
	{
		// TBD I can't let both setg and setp set m_buffer. How to resolve?

		m_buffer = buffer;
		m_pbase = m_pptr = pbeg;
		m_epptr = pend;
	}

	// Virutal buffer management and positioning //////////////////////////////

	/**
		Set buffer to access.

		\param[in] buffer Pointer to char array containing bits to access.
		\param[in] size Number of accessible bits in char array.
		\return Pointer to this object; NULL if error.
	*/
	virtual bitbuf *setbuf(unsigned char *buffer, std::streamsize size)
	{
		setg(buffer, 0, 0, size);
		setp(buffer, 0, size);

		return this;
	}

	/**
		Set get pointer relative to current position.

		\param[in] offset Amount by which get pointer is adjusted.
		\param[in] way From which pointer offset is applied for new position.
		\param[in] which Open mode.
		\return New position after get pointer modified.
	*/
	virtual std::streampos seekoff(std::streamoff offset,
		std::ios_base::seekdir way,
		std::ios_base::openmode which = std::ios_base::in | std::ios_base::out)
	{
		std::streampos new_position = std::streampos(-1);

		// TBD What does it mean to seekoff for both which's? Invalid for cur?

		if ((which & std::ios_base::in) != 0)
		{
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
				break;
			}
		}

		if ((which & std::ios_base::out) != 0)
		{
			switch (way)
			{
			case std::ios_base::beg:
				new_position = assure_valid_put_pointer(pbase() + offset);
				break;

			case std::ios_base::cur:
				new_position = assure_valid_put_pointer(pptr() + offset);
				break;

			case std::ios_base::end:
				new_position = assure_valid_put_pointer(epptr() + offset);
				break;

			default:
				break;
			}
		}

		// TBD What position do I return if both which's selected?

		return new_position;
	}

    /**
        Set get pointer to absolute position.

        \param[in] position New absolute position for get pointer.
		\param[in] which Open mode.
		\return New position after get pointer modified or std::streampos(-1)
		if error.
    */
    virtual std::streampos seekpos(std::streampos position,
		std::ios_base::openmode which = std::ios_base::in | std::ios_base::out)
    {
		std::streampos new_position = std::streampos(-1);

		// TBD What does it mean to seekpos for both which's?

		if ((which & std::ios_base::in) != 0)
		{
			new_position = assure_valid_get_pointer(position);
		}

		if ((which & std::ios_base::out) != 0)
		{
			new_position = assure_valid_put_pointer(position);
		}

		// TBD What position do I return if both which's selected?

		return new_position;
	}

	/**
		Synchronize stream buffer with input or output device.

		\note Since we do not currently have an I/O device with which to
		synchronize (we are unbuffered) this function always returns in error
		with -1.

		\return 0 if buffered stream and successful; -1 otherwise.
	*/
	virtual int sync()
	{
		return -1;
	}

	// Virtual input functions(get) ///////////////////////////////////////////

	/**
		Get number of bits available.

		\return (No information).
	*/
	virtual std::streamsize showmanyb()
	{
		// (Default behavior.)
		return 0;
	}

	/**
		Get sequence of bits.

		\param[out] value Value of bit field.
		\param[in] size Number of bits in sequence of bits.
		\return Number of bits read from buffer or zero if error or eof.
	*/
	virtual std::streamsize xsgetn(bitfield &value, std::streamsize size)
	{
		std::streamsize bits_read = xsgetn_nobump(value, size);

		// Advance bit pointer past field in preparation for next access.
		gbump(bits_read);

		return bits_read;
	}

	/**
		Get bit without changing current position.

		\param[out] value Bit at the current position.
		\return Whether there are more bits to read.
	*/
	bool underflow(bitfield &value)
	{
		return xsgetn_nobump(value, 1) != 0;
	}

	/**
		Get bit and advance position.

		\param[out] value Bit at the current position.
		\return Whether there are more bits to read.
	*/
	bool uflow(bitfield &value)
	{
		const bool gotBit = underflow(value);
		if (gotBit)
		{
			gbump(1);
		}

		return gotBit;
	}

	/**
		Put a bit back into input sequence and decrease the position.

		\param[out] value Bit to be put back.
		\return Whether there are more bits to read.
	*/
	bool pbackfail(const bitfield &value)
	{
		// Default behavior.
		return false;
	}

	// Virtual output functions(put) //////////////////////////////////////////

	/**
		Put sequence of bits.

		\param[out] value Value of bit field.
		\param[in] size Number of bits in sequence of bits.
		\return Number of bits written to buffer or zero if error or eof.
	*/
	virtual std::streamsize xsputn(bitfield value, std::streamsize size)
	{
		std::streamsize bits_read = xsputn_nobump(value, size);

		// Advance bit pointer past field in preparation for next access.
		pbump(bits_read);

		return bits_read;
	}

	/**
		Put bit without changing current position.

		\param[in] value Bit to be put.
		\return Whether the bit was successully put.
	*/
	virtual bool overflow(bitfield b)
	{
		// (Default behavior.)
		return false;
	}

private:
	/**
		Get bit at offset relative to gptr().

		\param[in] offset Offset from gptr() to get bit.
		\return Value of bit at offset from gptr().
	*/
	bitfield atgptrb(std::streamoff offset)
	{
		bitfield b = 0;

		if (seekoff(offset, std::ios_base::cur, std::ios_base::in) !=
			std::streampos(-1))
		{
			xsgetn_nobump(b, 1);
			seekoff(-offset, std::ios_base::cur, std::ios_base::in);
		}

		return b;
	}

	/**
		Get bit at offset relative to gptr().

		\param[in] offset Offset from gptr() to get bit.
		\param[out] b Value of bit at offset from gptr().
		\return Whether a bit is returned (eof not encountered).
	*/
	bool atgptrb(std::streamoff offset, bitfield &b)
	{
		bool got_bit = false;

		if (seekoff(offset, std::ios_base::cur, std::ios_base::in) !=
			std::streampos(-1))
		{
			got_bit = xsgetn_nobump(b, 1) == 1;
			seekoff(-offset, std::ios_base::cur, std::ios_base::in);
		}

		return got_bit;
	}

	/**
		Get bit at gptr().

		\param[out] b Value of bit at gptr().
		\return Whether a bit is returned (eof not encountered).
	*/
	bool atgptrb(bitfield &b)
	{
		return xsgetn_nobump(b, 1) == 1;
	}

	/**
		Assure that position is within bounds of accessible input sequence.

		\note If bit position is within bounds, use as internal get pointer.

		\param[in] position Candidate for new get position.
		\return position if valid; otherwise, std::streampos(-1).
	*/
	std::streampos assure_valid_get_pointer(std::streampos position)
	{
		std::streampos new_position;

		if (position < eback() || position > egptr())
		{
			new_position = std::streampos(-1);
		}
		else
		{
			m_gptr = new_position = position;
		}

		BOOST_ASSERT(new_position == std::streampos(-1) ||
			(new_position >= eback() && new_position <= egptr()));

		return new_position;
	}

	/**
		Assure that position is within bounds of accessible output sequence.

		\note If bit position is within bounds, use as internal put pointer.

		\param[in] position Candidate for new put position.
		\return position if valid; otherwise, std::streampos(-1).
		*/
	std::streampos assure_valid_put_pointer(std::streampos position)
	{
		std::streampos new_position;

		if (position < pbase() || position > epptr())
		{
			new_position = std::streampos(-1);
		}
		else
		{
			m_pptr = new_position = position;
		}

		BOOST_ASSERT(new_position == std::streampos(-1) ||
			(new_position >= pbase() && new_position <= epptr()));

		return new_position;
	}

	/**
        Get pointer to current input byte.

        \return Pointer to byte containing current input bit position (the next
		bit to read).
    */
    unsigned char *current_get_byte() const
    {
        return &m_buffer[m_gptr / CHAR_BIT];
    }

	/**
	Get pointer to current output byte.

	\return Pointer to byte containing current output bit position (the next
	bit to write).
	*/
	unsigned char *current_put_byte() const
	{
		return &m_buffer[m_pptr / CHAR_BIT];
	}

	/**
		Process sequence of bits.

		\note This is the typedef for a callback function called exclusively by
		xsprocessn_nobump().

		\param[in] value Value of bit field.
		\param[in] mask Mask to isolate bit field in stream, e.g., 0000000111111000.
		\param[in] shift_amount Number of bit positions mask had to be shifted left, e.g., 4.
		\param[in] byte_count Number of bytes to be processed for this bit field.
		\param[in] byte_pointer Pointer to byte containing current bit position.
	*/
	typedef void (*process_bits)(bitfield &value, bitfield mask,
		size_t shift_amount, size_t byte_count, unsigned char *byte_pointer);

	/**
		Get sequence of bits.

		\note This is a callback function called exclusively by xsprocessn_nobump().

		\param[in] value Value of bit field.
		\param[in] mask Mask to isolate bit field in stream, e.g., 0000000111111000.
		\param[in] shift_amount Number of bit positions mask had to be shifted left, e.g., 4.
		\param[in] byte_count Number of bytes to be read to get this bit field.
		\param[in] byte_pointer Pointer to byte containing current bit position.
	*/
	static void get_bits_callback(bitfield &value, bitfield mask, size_t shift_amount,
		size_t byte_count, unsigned char *byte_pointer)
	{
		bitfield pre_mask_value = 0;
		for (size_t i = 0; i < byte_count; ++i)
		{
			pre_mask_value <<= static_cast<unsigned>(CHAR_BIT);
			pre_mask_value |= byte_pointer[i];
		}

		value = pre_mask_value & mask;
		value >>= shift_amount;
	}

	/**
		Process sequence of bits without advancing pointer.

		\note The get/put pointer is not advanced. Use xsgetn()/xsputn() for that.

		\param[out] value Value of bit field.
		\param[in] size Number of bits in sequence of bits.
		\param[in] ptr Position of next bit to process.
		\param[in] eptr Position of bit after last bit in stream.
		\param[in] byte_pointer Pointer to byte containing current bit position.
		\param[in] process_bits_ Callback to process, i.e., get or put, bit field.
		\return Number of bits processed from buffer or zero if error or eof.
		*/
	std::streamsize xsprocessn_nobump(bitfield &value, std::streamsize size,
		std::streampos ptr, std::streampos eptr, unsigned char *byte_pointer,
		process_bits process_bits_)
	{
		std::streamsize bits_processed = 0;

		if (size > 0 && size <= eptr - ptr)
		{
			// Generate "right-justified" mask, e.g., 0000000000111111.
			bitfield mask = -1;
			// (Had to break up shift into two parts because shifting the width
			// of the integral all at once had no effect.)
			mask <<= size - 1;
			mask <<= 1;
			mask = ~mask;

			// Shift left so mask appears at correct location within integral,
			// e.g., 0000000111111000.
			const size_t intra_byte_bit_offset = ptr % CHAR_BIT;
			const size_t shift_amount = (CHAR_BIT - ((size + intra_byte_bit_offset) % CHAR_BIT)) % CHAR_BIT;
			mask <<= shift_amount;

			// Number of bytes to be processed for this bit field.
			const size_t byte_count = (static_cast<size_t>(size) + shift_amount + CHAR_BIT - 1) / CHAR_BIT;

			process_bits_(value, mask, shift_amount, byte_count, byte_pointer);

			bits_processed = size;
		}

		return bits_processed;
	}

	/**
		Get sequence of bits without advancing pointer.

		\note The get pointer is not advanced. Use xsgetn() for that.

		\param[out] value Value of bit field.
		\param[in] size Number of bits in sequence of bits.
		\return Number of bits read from buffer or zero if error or eof.
	*/
	std::streamsize xsgetn_nobump(bitfield &value, std::streamsize size)
	{
		return xsprocessn_nobump(value, size, gptr(), egptr(),
			current_get_byte(), get_bits_callback);
	}

	/**
		Put sequence of bits.

		\note This is a callback function called exclusively by xsprocessn_nobump().

		\param[in] value Value of bit field.
		\param[in] mask Mask to isolate bit field in stream, e.g., 0000000111111000.
		\param[in] shift_amount Number of bit positions mask had to be shifted left, e.g., 4.
		\param[in] byte_count Number of bytes to be written to get this bit field.
		\param[in] byte_pointer Pointer to byte containing current bit position.
	*/
	static void put_bits_callback(bitfield &value, bitfield mask, size_t shift_amount,
		size_t byte_count, unsigned char *byte_pointer)
	{
		mask = ~mask;
		value <<= shift_amount;
		for (size_t i = byte_count - 1;; --i, mask >>= CHAR_BIT, value >>= CHAR_BIT)
		{
			byte_pointer[i] &= mask;
			byte_pointer[i] |= value;
			if (i == 0)
			{
				break;
			}
		}
	}

	/**
		Put sequence of bits without advancing pointer.

		\note The put pointer is not advanced. Use xsputn() for that.

		\param[out] value Value of bit field.
		\param[in] size Number of bits in sequence of bits.
		\return Number of bits written to buffer or zero if error or eof.
	*/
	std::streamsize xsputn_nobump(bitfield value, std::streamsize size)
	{
		return xsprocessn_nobump(value, size, pptr(), epptr(),
			current_put_byte(), put_bits_callback);
	}

	/**
        Current bit position in buffer.

        \note Position is zero-based, starting with MSB in byte pointed to by
        m_buffer, progressing to LSB of that byte, then MSB of next byte, etc.

        \note This is analogous to streambuf::gptr, except it "points" to a
        bit position rather than a character position.
    */
    std::streampos m_gptr;

    /**
        End of accessible input sequence.

        \note Points just past last bit.
    */
    std::streampos m_egptr;

    /**
        Beginning of accessible input sequence.

        \note Not currently used. Always 0, which is first bit in first byte.
    */
    std::streampos m_eback;

	/**
		Current bit position in buffer.

		\note Position is zero-based, starting with MSB in byte pointed to by
		m_buffer, progressing to LSB of that byte, then MSB of next byte, etc.

		\note This is analogous to streambuf::pptr, except it "points" to a
		bit position rather than a character position.
	*/
	std::streampos m_pptr;

	/**
		End of accessible output sequence.

		\note Points just past last bit.
	*/
	std::streampos m_epptr;

	/**
		Beginning of accessible output sequence.

		\note Not currently used. Always 0, which is first bit in first byte.
	*/
	std::streampos m_pbase;

    /**
        Pointer to first byte of char array containing the bits.
    */
    unsigned char *m_buffer;
};

/**
    Base class for streams (type-dependent components).
*/
class iob
{
public:
    /**
        Constructor.

        \param[in] bb Pointer to a bitbuf object.
    */
    explicit iob(bitbuf *bb)
    {
        init(bb);
    }

    /**
        Destructor.
    */
    virtual ~iob()
    {
        // Do nothing.
    }

    /**
        Evaluate stream object for success.

        This function returns null if the internal failbit or badbit are set
        for this stream; otherwise, it returns a non-zero pointer.

        \return Whether 0 if failbit or badbit are set; non-zero otherwise.
    */
    operator void *() const
    {
        static int dummy;
        return fail() ? 0 : reinterpret_cast<void *>(&dummy);
    }

    /**
        Check if bitstream is good for continued operation.

        \return Whether any of the error flags are set.
    */
    bool good() const
    {
        return rdstate() == 0;
    }

    /**
        Check if error flag, eofbit, is set.

        \note eofbit is set when an operation attempts to access a bit position
        outside of the bit stream, e.g., reading past end of bitstream.

        \return Whether previous input operation set eofbit.
    */
    bool eof() const
    {
        return (rdstate() & std::ios_base::eofbit) != 0;
    }

    /**
        Check if either error flag, failbit or badbit, is set.

        \note failbit is set when there is an error with the internal logic of
        an operation.

        \see Note for bad().

        \return Whether previous input operation set failbit or badbit.
    */
    bool fail() const
    {
        return (rdstate() & std::ios_base::failbit) != 0 || bad();
    }

    /**
        Check if error flag, badbit, is set.

        \note badbit is set if the integrity of the stream is lost, e.g.,
        encountered unexpected value.

        \return Whether previous input operation set badbit.
    */
    bool bad() const
    {
        return (rdstate() & std::ios_base::badbit) != 0;
    }

	/**
        Evaluate stream object for success.

        This function returns whether the internal failbit and badbit are not
        set for this ibitstream.

        \note Same as calling !fail(). !bin.fail() is the same as bin.

        \return Whether the failbit and badbit are not set.
    */
    operator bool() const
    {
        return !fail();
    }

    /**
        Get error state flags.

        \return Error state flags.
    */
    std::ios_base::iostate rdstate() const
    {
        return m_state;
    }

    /**
        Set error state flags.

        \note This function sets states additively (no state is cleared).

        \param[in] state Error state flags.
    */
    void setstate(std::ios_base::iostate state)
    {
		// TBD If no stream buffer, set badbit flag.
        m_state |= state;
    }

    /**
        Set error state flags.

        \param[in] state Error state flags.
    */
    void clear(std::ios_base::iostate state = std::ios_base::goodbit)
    {
        m_state = state;
    }

    /**
        Get the bitbuf object currently associated with the stream.

        \return A pointer to the bitbuf object associated with the stream.
    */
    bitbuf *rdbuf() const
    {
        return m_bitbuf;
    }

    /**
        Set the bitbuf object associated with the stream.

        \return Pointer to bitbuf object previously associated with stream.
    */
    bitbuf *rdbuf(bitbuf *bb)
    {
        bitbuf *previous_bitbuf = m_bitbuf;
		init(bb);
        return previous_bitbuf;
    }

#if 0
    /**
        Get tied stream.

        \return Pointer to stream object tied before call; if none, NULL.
    */
    ostream *tie() const
    {
        return NULL;
    }

    /**
        Tie to tieb.

        \return Pointer to stream object tied before call; if none, NULL.
    */
    ostream *tie(ostream *tieb)
    {
        return NULL;
    }
#endif

protected:
    /*
        Constructor.

        \note Consumer must call init() after calling this constructor.
    */
    iob()
    {
        // Do nothing.
    }

    /**
        Initialize member variables.

        \param[in] bb Pointer to a bitbuf object.
    */
    void init(bitbuf *bb)
    {
        m_bitbuf = bb;
        m_state = bb == 0 ? std::ios_base::badbit : std::ios_base::goodbit;
    }

    /**
        Set badbit error flag.
    */
    void badbit()
    {
        m_state |= std::ios_base::badbit;
    }

    /**
        Set failbit error flag.
    */
    void failbit()
    {
        m_state |= std::ios_base::failbit;
    }

    /**
        Set eofbit error flag.
    */
    void eofbit()
    {
        m_state |= std::ios_base::eofbit;
    }

private:
    /**
        Buffer from which this class serially extracts bits.
    */
    bitbuf *m_bitbuf;

    /**
        State of input stream.

        \note This member variable is a bit mask; therefore it represents
        multiple states including eof and failure.
    */
    std::ios_base::iostate m_state;
};

} // namespace bitstream

} // namespace boost

#endif