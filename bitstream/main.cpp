// Copyright (C) 2013 Paul Long.
//
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/bstream for documentation.

#include "bstream.h"
#include <iostream>
using namespace std;
extern bool rtpTest();
int main(int argc, char *argv[])
{
    cout << rtpTest() << endl;
    system("pause");
    return 0;
}

