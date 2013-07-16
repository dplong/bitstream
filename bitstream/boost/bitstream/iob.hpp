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

class iob
{
public:
    ///
    /// Constructor.
    explicit iob() : m_state(0)
    {
        // Do nothing.
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

protected:
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