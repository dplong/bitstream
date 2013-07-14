///
/// This file contains classes for manipulating bits as input streams.
///
/// @note The ibitstream class is analogous to the C++ IOstream class,
/// istringstream, except it manipulates bits, not characters.
///
/// Example:
/// @code
/// // Full parsing of an RTP header, including the often-overlooked
/// // variable-length fields.
/// 
/// struct RtpHeader {
///     bool padding, marker;
///     bitset<7> payloadType;
///     WORD sequenceNumber;
///     DWORD timestamp, ssrcIdentifier;
///     vector<DWORD> csrcIdentifier;
///     struct {
///         bool present;
///         WORD identifier;
///         vector<BYTE> contents;
///     } extension;
/// };
/// 
/// bool ParseRtpHeader(const BYTE *buffer, RtpHeader &rtp)
/// {
///     ibitstream bin(buffer);
///     static const bitset<2> version(0x2);
///     bitset<4> csrcCount;
///     WORD extensionLength;
/// 
///     bin >> version >> rtp.padding >> rtp.extension.present
///         >> csrcCount >> rtp.marker >> rtp.payloadType
///         >> rtp.sequenceNumber >> rtp.timestamp >> rtp.ssrcIdentifier
///         >> setrepeat(csrcCount.to_ulong()) >> rtp.csrcIdentifier;
/// 
///     if (rtp.extension.present) {
///         bin >> rtp.extension.identifier >> extensionLength
///             >> setrepeat(extensionLength * sizeof(DWORD))
///             >> rtp.extension.contents;
///     }
/// 
///     return bin.good();
/// }
/// @endcode

#pragma once

#pragma region Includes
////////////////////////////////////////////////////////////////////////////////

#include <bitset>
#include <vector>
#include <cassert>
#include "boost/typeof/typeof.hpp"

// This code uses decltype, which is defined in terms of BOOST's type_of. BOOST
// only knows about C++'s core types, so we have to tell it about streampos.
#include BOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()
BOOST_TYPEOF_REGISTER_TYPE(std::streampos)

#pragma endregion
#pragma region Constants
////////////////////////////////////////////////////////////////////////////////

#pragma endregion
#pragma region Typedefs
////////////////////////////////////////////////////////////////////////////////

///
/// Integral type for bit-field values.
typedef decltype(std::bitset<0>().to_ulong()) bitfield;

#pragma endregion
#pragma region bitbuf
////////////////////////////////////////////////////////////////////////////////

///
/// This class represents contiguous memory, accessed as a sequence of bit fields.
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
    explicit bitbuf(std::ios_base::openmode which = std::ios_base::in | std::ios_base::out);

    ///
    /// Constructor.
    ///
    /// @param[in] buffer Pointer to char array to be accessed.
    /// @param[in] which Open mode.
    /// @param[in] size Number of accessible bits in char array.
    bitbuf(signed char *buffer, std::streamsize size = INT_MAX,
        std::ios_base::openmode which = std::ios_base::in | std::ios_base::out);

    ///
    /// Constructor.
    ///
    /// @param[in] buffer Pointer to char array to be accessed.
    /// @param[in] which Open mode.
    /// @param[in] size Number of accessible bits in char array.
    bitbuf(unsigned char *buffer, std::streamsize size = INT_MAX,
        std::ios_base::openmode which = std::ios_base::in | std::ios_base::out);

    ///
    /// Constructor.
    ///
    /// @param[in] buffer Pointer to char array to be accessed.
    /// @param[in] which Open mode.
    /// @param[in] size Number of accessible bits in char array.
    bitbuf(const signed char *buffer, std::streamsize size = INT_MAX,
        std::ios_base::openmode which = std::ios_base::in | std::ios_base::out);

    ///
    /// Constructor.
    ///
    /// @param[in] buffer Pointer to char array to be accessed.
    /// @param[in] which Open mode.
    /// @param[in] size Number of accessible bits in char array.
    bitbuf(const unsigned char *buffer, std::streamsize size = INT_MAX,
        std::ios_base::openmode which = std::ios_base::in | std::ios_base::out);

    ///
    /// Destructor.
    ~bitbuf();

    ///
    /// Number of bits currently available to read.
    ///
    /// @return Number of readable bits.
    std::streamsize in_avail() const;

    ///
    /// Reposition get-next-bit pointer relative to current position.
    ///
    /// @param[in] offset Signed offset from current position for new position.
    /// @param[in] way From which pointer the offset is applied for the new position.
    /// @return Position after offset applied.
    std::streampos pubseekoff(std::streamoff offset, std::ios_base::seekdir way);

    ///
    /// Reposition get-next-bit pointer.
    ///
    /// @param[in] position New bit position.
    /// @return Position after repositioning.
    std::streampos pubseekpos(std::streampos position);

    ///
    /// Set buffer to access.
    ///
    /// @param[in] buffer Pointer to char array to be accessed.
    /// @return Pointer to this object; NULL if error.
    bitbuf *pubsetbuf(unsigned char *buffer);

    ///
    /// Set buffer to access.
    ///
    /// @param[in] buffer Pointer to char array to be accessed.
    /// @param[in] size Number of accessible bits in char array.
    /// @param[in] position Offset of first accessible bit in char array.
    /// @return Pointer to this object; NULL if error.
    bitbuf *pubsetbuf(unsigned char *buffer, std::streamsize size,
        std::streampos position = 0);

    ///
    /// Synchronize stream buffer with input or output device.
    ///
    /// @return 0 if buffered stream and successful; -1 otherwise.
    int pubsync();

    ///
    /// Get current bit and advance get pointer.
    ///
    /// @param[out] value Current bit before advancing pointer.
    /// @return Whether okay--eof has not been encountered.
    bool sbumpb(bitfield &value);

    ///
    /// Get current bit at get pointer.
    ///
    /// @param[out] value Current bit.
    /// @return Whether okay--eof has not been encountered.
    bool sgetb(bitfield &value);

    ///
    /// Get sequence of bits.
    ///
    /// @param[out] value Value of bit field.
    /// @param[in] size Number of bits in sequence of bits.
    /// @return Number of bits read from buffer or zero if error or eof.
    std::streamsize sgetn(bitfield &value, std::streamsize size);

    ///
    /// Advance get pointer and return next bit.
    ///
    /// @param[out] value Next bit.
    /// @return Whether okay--eof has not been encountered.
    bool snextb(bitfield &value);

    ///
    /// Move get pointer backwards and return bit at new position.
    ///
    /// @param[out] value Bit before position prior to call.
    /// @return Whether okay--eof has not been encountered.
    bool sungetb(bitfield &value);

    ///
    /// Represents both an unbounded number of bits and an attempt to move past bit-stream bounds.
    ///
    /// @note A consumer might encounter this value where member functions of
    /// this class return a value of streamsize or streampos type.
    static const std::streampos npos;

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
    /// Returns bit position within accessible input sequence of next bit to be read.
    ///
    /// @note The "get pointer."
    ///
    /// @return Next bit position.
    std::streampos gptr() const;

    ///
    /// Returns bit position just past last bit in accessible input sequence.
    ///
    /// @return Position after last bit.
    std::streampos egptr() const;

    ///
    /// Returns first bit position in accessible input sequence.
    ///
    /// @return Position of first bit.
    std::streampos eback() const;

    ///
    /// Set pointer and offsets that define boundaries of and position within accessible input sequence.
    ///
    /// @param[in] buffer Pointer to char array to be accessed.
    /// @param[in] gbeg Position of first accessible bit in char array.
    /// @param[in] gnext Position of next bit to read.
    /// @param[in] gend Position of bit immediately after last accessible bit in char array.
    void setg(unsigned char *buffer, std::streampos gbeg, std::streampos gnext, std::streampos gend);

    ///
    /// Advances the get pointer by specified number of bit positions.
    ///
    /// @param[in] Value by which to increase the get pointer.
    void gbump(std::streamoff offset);

    ///
    /// Set get pointer to absolute position.
    ///
    /// @param[in] position New absolute position for get pointer.
    /// @return New position after get pointer modified or npos if error.
    virtual std::streampos seekpos(std::streampos position);

    ///
    /// Set get pointer relative to current position.
    ///
    /// @param[in] offset Amount by which get pointer is adjusted.
    /// @param[in] way From which pointer the offset is applied for the new position.
    /// @return New position after get pointer modified.
    virtual std::streampos seekoff(std::streamoff offset, std::ios_base::seekdir way);

    ///
    /// Assure that position is within bounds of accessible input sequence.
    ///
    /// If bit position is within bounds, use as internal get pointer and
    /// return it; otherwise, return npos.
    ///
    /// @param[in] position Candidate for new current bit position, or get pointer.
    std::streampos AssureValidGetPointer(std::streampos position);

    ///
    /// Set buffer to access.
    ///
    /// @param[in] buffer Pointer to char array containing bits to access.
    /// @param[in] size Number of accessible bits in char array.
    /// @param[in] position Offset of first accessible bit in char array.
    /// @return Pointer to this object; NULL if error.
    virtual bitbuf *setbuf(unsigned char *buffer, std::streamsize size, std::streampos position);

    ///
    /// Synchronize stream buffer with input or output device.
    ///
    /// @note Since we do not currently have an I/O device with which to
    /// synchronize--we are unbuffered--this function always returns in error
    /// with -1.
    ///
    /// @return 0 if buffered stream and successful; -1 otherwise.
    virtual int sync();

    ///
    /// Get sequence of bits.
    ///
    /// @param[out] value Value of bit field.
    /// @param[in] size Number of bits in sequence of bits.
    /// @return Number of bits read from buffer or zero if error or eof.
    virtual std::streamsize xsgetn(bitfield &value, std::streamsize size);

private:
    ///
    /// Get pointer to current byte.
    ///
    /// @return Pointer to byte containing current bit position (the next bit to read).
    unsigned char *currentByte() const;

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

#pragma endregion
#pragma region ibitstream
////////////////////////////////////////////////////////////////////////////////

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
class ibitstream
{
public:
    ///
    /// Constructor.
    ///
    /// @param[in] which Open mode.
    explicit ibitstream(std::ios_base::openmode which = std::ios_base::in);

    ///
    /// Constructor.
    ///
    /// @param[in] buffer Pointer to char array to be accessed.
    /// @param[in] size Number of accessible bits in char array.
    /// @param[in] which Open mode.
    explicit ibitstream(signed char *buffer, std::streamsize size = INT_MAX,
        std::ios_base::openmode which = std::ios_base::in);

    ///
    /// Constructor.
    ///
    /// @param[in] buffer Pointer to char array to be accessed.
    /// @param[in] size Number of accessible bits in char array.
    /// @param[in] which Open mode.
    explicit ibitstream(unsigned char *buffer, std::streamsize size = INT_MAX,
        std::ios_base::openmode which = std::ios_base::in);

    ///
    /// Constructor.
    ///
    /// @param[in] buffer Pointer to char array to be accessed.
    /// @param[in] size Number of accessible bits in char array.
    /// @param[in] which Open mode.
    explicit ibitstream(const signed char *buffer, std::streamsize size = INT_MAX,
        std::ios_base::openmode which = std::ios_base::in);

    ///
    /// Constructor.
    ///
    /// @param[in] buffer Pointer to char array to be accessed.
    /// @param[in] size Number of accessible bits in char array.
    /// @param[in] which Open mode.
    explicit ibitstream(const unsigned char *buffer, std::streamsize size = INT_MAX,
        std::ios_base::openmode which = std::ios_base::in);

    ///
    /// Destructor.
    ~ibitstream();

    ///
    /// Get one bit from stream.
    ///
    /// @return Next bit from stream.
    bitfield get();

    ///
    /// Ignore, or skip over, bits in stream.
    ///
    /// @param[in] bits Number of bits to ignore.
    /// @return This bit stream.
    ibitstream &ignore(std::streamsize bits = 1);

    ///
    /// Align get pointer to next bit multiple if not already at one.
    ///
    /// @param[in] bit Bit multiple, such as 8 for byte alignment.
    /// @return This bit stream.
    ibitstream &aligng(size_t bit);

    ///
    /// Get next bit from stream without advancing get pointer.
    ///
    /// @return Next bit from stream.
    bitfield peek();

    ///
    /// Get bits from stream.
    ///
    /// @note Unlike get(), this function returns the bit value in an integral,
    /// not a bitset.
    ///
    /// @param[out] Integral to receive bits from stream.
    /// @param[in] bits Number of bits to read.
    /// @return This bit stream.
    ibitstream &read(bitfield &value, std::streamsize bits);

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
    ibitstream &readsome(bitfield &value, std::streamsize bits);

    ///
    /// Set position of get pointer.
    ///
    /// @param[in] position Bit position.
    /// @return This bit stream.
    ibitstream &seekg(std::streampos position);

    ///
    /// Set position of get pointer relative to indicated internal pointer.
    ///
    /// @param[in] offset Relative offset from indicated pointer.
    /// @param[in] dir Bit pointer to which offset is applied.
    /// @return This bit stream.
    ibitstream &seekg(std::streamoff offset, std::ios_base::seek_dir dir);

    ///
    /// Synchronize input buffer with source of bits.
    ///
    /// @return 0 if buffered stream and successful; -1 otherwise.
    int sync();

    ///
    /// Get position of get pointer.
    ///
    /// @return Bit position of next bit to be read.
    std::streampos tellg();

    ///
    /// Move get pointer backwards and return bit at new position.
    ///
    /// @return This bit stream.
    ibitstream &unget();

    ///
    /// Get error state flags.
    ///
    /// @return Error state flags.
    std::ios_base::iostate rdstate() const;

    ///
    /// Set error state flags.
    ///
    /// @param[in] state Error state flags.
    void clear(std::ios_base::iostate state = std::ios_base::goodbit);

    ///
    /// Set error state flags.
    ///
    /// @note This function sets states additively--no state is cleared.
    ///
    /// @param[in] state Error state flags.
    void setstate(std::ios_base::iostate state);

    ///
    /// Check if error flag, badbit, is set.
    ///
    /// @note badbit is set if the integrity of the stream is lost, e.g.,
    /// encountered unexpected value.
    ///
    /// @return Whether previous input operation set badbit.
    bool bad() const;

    ///
    /// Check if either error flag, failbit or badbit, is set.
    ///
    /// @note failbit is set when there is an error with the internal logic of
    /// an operation.
    ///
    /// @see Note for bad().
    ///
    /// @return Whether previous input operation set failbit or badbit.
    bool fail() const;

    ///
    /// Check if error flag, eofbit, is set.
    ///
    /// @note eofbit is set when an operation attempts to access a bit position
    /// outside of the bit stream, e.g., reading past end of bitstream.
    ///
    /// @return Whether previous input operation set eofbit.
    bool eof() const;

    ///
    /// Check if bitstream is good for continued operation.
    ///
    /// @return Whether any of the error flags are set.
    bool good() const;

    ///
    /// Get number of bits extracted by last input operation.
    std::streamsize gcount() const;

    ///
    /// Get value extracted by last input operation.
    ///
    /// @note The value returned by this function is only valid if gcount()
    /// returns a value greater than zero.
    ///
    /// @return Most recent extracted value.
    bitfield gvalue() const;

    ///
    /// Evaluate stream object.
    ///
    /// This function returns whether the internal failbit or badbit has been
    /// set for this ibitstream.
    ///
    /// @note Same as calling fail(). bin.fail() is the same as !bin.
    ///
    /// @return Whether the failbit or badbit has been set.
    bool operator!() const;

    ///
    /// Set repeat count for subsequent vector extractions.
    ///
    /// @note This function does not extract anything from ibitstream. It
    /// merely saves a value that any subsequent vector extractions use to know
    /// how many bit fields to extract into the same number of vector elements.
    ///
    /// @param repeat Number of bit fields to extract to each subsequent vector.
    /// @return This bit stream.
    ibitstream &setrepeat(size_t repeat);

private:
    ///
    /// Set badbit error flag.
    void badbit();

    ///
    /// Friend const functions for access to badbit().
    //@{
    template <typename T> friend ibitstream &operator>>(ibitstream &ibs, const T &b);
    template <size_t N> friend ibitstream &operator>>(ibitstream &ibs, const std::bitset<N> &bs);
    friend ibitstream &operator>>(ibitstream &ibs, const bool &b);
    //@}

    ///
    /// Set failbit error flag.
    void failbit();

    ///
    /// Set eofbit error flag.
    void eofbit();

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
    bool alignedg(size_t bit);

    ///
    /// Buffer from which this class serially extracts bits.
    bitbuf m_bitbuf;

    ///
    /// State of input stream.
    ///
    /// @note This member variable is a bit mask; therefore it represents
    /// multiple states including eof and failure.
    std::ios_base::iostate m_state;

    ///
    /// Number of bits extracted by last input operation.
    std::streamsize m_gcount;

    ///
    /// Value extracted by last input operation.
    bitfield m_gvalue;

    ///
    /// Number of bit fields extracted to a container.
    size_t m_repeat;
};

#pragma endregion
#pragma region Manipulators
////////////////////////////////////////////////////////////////////////////////

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
    /// @param[in] repeat Number of bit fields to extract to each subsequent integral container.
    setrepeat(size_t repeat);

    ///
    /// Overload for the () operator on this class.
    ///
    /// @param[in,out] ibs Reference to ibitstream on left-hand side of >> operator.
    /// @return Reference to ibitstream parameter.
    ibitstream &operator()(ibitstream &ibs) const;

private:
    ///
    /// Number of bit fields to extract per container.
    size_t m_repeat;
};

///
/// Manipulator for ibitstream that sets the repeat count for subsequent container extractions.
///
/// @param[in,out] ibs Reference to ibitstream on left-hand side of operator.
/// @param[in] repeat Instance of setrepeat class.
/// @return Reference to ibitstream parameter.
ibitstream &operator>>(ibitstream &ibs, setrepeat repeat);
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
    ignore(size_t bits);

    ///
    /// Overload for the () operator on this class.
    ///
    /// @param[in,out] ibs Reference to ibitstream on left-hand side of >> operator.
    /// @return Reference to ibitstream parameter.
    ibitstream &operator()(ibitstream &ibs) const;

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
ibitstream &operator>>(ibitstream &ibs, ignore skip);
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
    aligng(size_t bits);

    ///
    /// Overload for the () operator on this class.
    ///
    /// @param[in,out] ibs Reference to ibitstream on left-hand side of >> operator.
    /// @return Reference to ibitstream parameter.
    ibitstream &operator()(ibitstream &ibs) const;

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
ibitstream &operator>>(ibitstream &ibs, aligng align);
//@}

#pragma endregion
#pragma region Operator overloads
////////////////////////////////////////////////////////////////////////////////

///
/// Get single bit from input stream and place in bool.
///
/// @param[in,out] ibs Reference to ibitstream on left-hand side of operator.
/// @param[out] b bool on right-hand side of operator.
/// @return Reference to ibitstream parameter.
ibitstream &operator>>(ibitstream &ibs, bool &b);

///
/// Get single bit from input stream that must be equal to bool.
///
/// @param[in,out] ibs Reference to ibitstream on left-hand side of operator.
/// @param[in] b bool on right-hand side of operator.
/// @return Reference to ibitstream parameter.
ibitstream &operator>>(ibitstream &ibs, const bool &b);

#pragma endregion
#pragma region Templates
////////////////////////////////////////////////////////////////////////////////

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
/// elements in the vector with bit fields sequentially extracted from the input
/// stream. It does not increase the size of the vector, e.g., with
/// push_back(), because it relies on the existing size of the vector to know
/// how many bit fields to extract.
///
/// @param[in,out] ibs Reference to ibitstream on left-hand side of operator.
/// @param[out] v Integral vector on right-hand side of operator.
/// @return Reference to ibitstream parameter.
template <typename T>
ibitstream &operator>>(ibitstream &ibs, std::vector<T> &v)
{
    decltype(v.size()) vectorSize = v.size();
    for (decltype(vectorSize) i = 0; i < vectorSize; ++i) {
        ibs >> v[i];
    }

    return ibs;
}

///
/// Get bit fields from input stream that must be equal to elements in integral vector.
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
    assert(bs.to_ulong() == value);

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

#pragma endregion

