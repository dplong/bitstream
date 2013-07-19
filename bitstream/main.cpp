/** \file
    \brief Display results of input bit-stream regression test.
    \details This source file contains the main() function which runs a single
        regression test in the input bit-stream class.
    \copyright Copyright (C) 2013 Paul Long.
    \note Use, modification, and distribution is subject to the Boost Software
        License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
    \see http://www.boost.org/ for latest version.
    \see http://www.boost.org/libs/bitstream for documentation.
*/

#include <iostream>
using namespace std;
extern bool rtpTest();

/**
    Display whether the input bit-stream class passed its regression test.

    This function calls a function to perform a regression test on the input
    bit-stream class.
    
    \note This function displays "1" on the standard output device if the test
    passed; "0", if not.

    \return 0.
*/
int main()
{
    cout << rtpTest() << endl;
    system("pause");
    return 0;
}

