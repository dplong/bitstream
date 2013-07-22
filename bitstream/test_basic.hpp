/** \file
    \brief Basic regression tests for input bit-stream class.
    \details This header file contains basic tests for regression of the
        input bit-stream class.
    \copyright Copyright (C) 2013 Paul Long.
    \note Use, modification, and distribution is subject to the Boost Software
        License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
    \see http://www.boost.org/ for latest version.
    \see http://www.boost.org/libs/bitstream for documentation.
*/

BOOST_AUTO_TEST_CASE(test_i_construct_with_null)
{
    ibitstream ibs(NULL);
    BOOST_CHECK(ibs.rdbuf()->data() == NULL);
}

BOOST_AUTO_TEST_CASE(test_i_construct_with_empty_string)
{
    static const char c[] = "";
    ibitstream ibs(c);
    BOOST_CHECK(ibs.rdbuf()->data() == c);
    BOOST_CHECK(strlen(ibs.rdbuf()->data()) == 0);
    BOOST_CHECK(strcmp(ibs.rdbuf()->data(), c) == 0);
}

BOOST_AUTO_TEST_CASE(test_i_construct_with_non_empty_string)
{
    static const char c[] = "bananas";
    ibitstream ibs(c);
    BOOST_CHECK(ibs.rdbuf()->data() == c);
    BOOST_CHECK(strcmp(ibs.rdbuf()->data(), c) == 0);
}

BOOST_AUTO_TEST_CASE(test_o_construct_with_null)
{
    obitstream obs(NULL);
    BOOST_CHECK(obs.rdbuf()->data() == NULL);
}

BOOST_AUTO_TEST_CASE(test_o_construct_with_empty_string)
{
    static const char c[] = "";
    obitstream obs(c);
    BOOST_CHECK(obs.rdbuf()->data() == c);
    BOOST_CHECK(strlen(obs.rdbuf()->data()) == 0);
    BOOST_CHECK(strcmp(obs.rdbuf()->data(), c) == 0);
}

BOOST_AUTO_TEST_CASE(test_o_construct_with_non_empty_string)
{
    static const char c[] = "bananas";
    obitstream obs(c);
    BOOST_CHECK(obs.rdbuf()->data() == c);
    BOOST_CHECK(strcmp(obs.rdbuf()->data(), c) == 0);
}