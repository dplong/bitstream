/** \file
    \brief Regression tests for input bit-stream class.
    \details This source file causes a main() function to be generated for
        regression testing the input bit-stream class.
    \copyright Copyright (C) 2013 Paul Long.
    \note Use, modification, and distribution is subject to the Boost Software
        License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
    \see http://www.boost.org/ for latest version.
    \see http://www.boost.org/libs/bitstream for documentation.
*/

#define BOOST_TEST_MAIN bitstream Unit Tests
#include <boost/test/included/unit_test.hpp>

#include "boost/bstream.hpp"
#include "boost/bitstream/iobmanip.hpp"
#include <cstdint>

using namespace std;
using namespace boost::bitstream;

#include "test_basic.hpp"
#include "test_rtp.hpp"

/*
Output:

Running 1 test case...

*** No errors detected
Press any key to continue . . .

*/