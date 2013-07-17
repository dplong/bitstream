// Boost general library ibstream.hpp header file /////////////////////////////
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

#ifndef BOOST_BITSTREAM_IBSTREAM_HPP
#define BOOST_BITSTREAM_IBSTREAM_HPP

#include "boost/bitstream/iob.hpp"

namespace boost {

namespace bitstream {

class ibstream : public iob
{
public:
    ///
    /// Constructor.
    explicit ibstream(bitbuf *bb) : iob(bb), m_gcount(0)
    {
        // Do nothing.
    }

    ///
    /// Get number of bits extracted by last input operation.
    std::streamsize gcount() const
    {
        return m_gcount;
    }

    // TBD - Move from bstream.h: get(), ignore(), peek(), read(), readsome(), seekg(), sync(), tellg(), unget(), operator>>()
    // TBD - Once obstream is implemented, implement putback()
protected:
    ///
    /// Number of bits extracted by last input operation.
    std::streamsize m_gcount;
};

} // namespace bitstream

} // namespace boost

#endif