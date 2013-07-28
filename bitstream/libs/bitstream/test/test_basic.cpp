/** \file
    \brief Basic regression tests for input bit-stream class.
    \details This header file contains basic tests for regression of the
        input bit-stream class.
    \see http://www.boost.org/ for latest version.
    \see http://www.boost.org/libs/bitstream for documentation.

    Use, modification, and distribution is subject to the Boost Software
        License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/

#define BOOST_TEST_MAIN  "bitstream Unit Tests"

#ifdef _MSC_VER
#  pragma warning(disable : 4224)
#endif

#include <boost/test/included/unit_test.hpp>

#include <boost/bitstream/bstream.hpp>
#include <boost/bitstream/iomanip.hpp>

#include <cstdint>

BOOST_AUTO_TEST_CASE(test_i_construct_with_null)
{
    boost::bitstream::ibitstream ibs(NULL);
    BOOST_CHECK(ibs.rdbuf()->data() == NULL);
}

BOOST_AUTO_TEST_CASE(test_i_construct_with_empty_string)
{
    static const char c[] = "";
    boost::bitstream::ibitstream ibs(c);
    BOOST_CHECK(ibs.rdbuf()->data() == c);
    BOOST_CHECK(strlen(ibs.rdbuf()->data()) == 0);
    BOOST_CHECK(strcmp(ibs.rdbuf()->data(), c) == 0);
}

BOOST_AUTO_TEST_CASE(test_i_construct_with_non_empty_string)
{
    static const char c[] = "bananas";
    boost::bitstream::ibitstream ibs(c);
    BOOST_CHECK(ibs.rdbuf()->data() == c);
    BOOST_CHECK(strcmp(ibs.rdbuf()->data(), c) == 0);
}

BOOST_AUTO_TEST_CASE(test_o_construct_with_null)
{
    boost::bitstream::obitstream obs(NULL);
    BOOST_CHECK(obs.rdbuf()->data() == NULL);
}

BOOST_AUTO_TEST_CASE(test_o_construct_with_empty_string)
{
    static const char c[] = "";
    boost::bitstream::obitstream obs(c);
    BOOST_CHECK(obs.rdbuf()->data() == c);
    BOOST_CHECK(strlen(obs.rdbuf()->data()) == 0);
    BOOST_CHECK(strcmp(obs.rdbuf()->data(), c) == 0);
}

BOOST_AUTO_TEST_CASE(test_o_construct_with_non_empty_string)
{
    static const char c[] = "bananas";
    boost::bitstream::obitstream obs(c);
    BOOST_CHECK(obs.rdbuf()->data() == c);
    BOOST_CHECK(strcmp(obs.rdbuf()->data(), c) == 0);
}
