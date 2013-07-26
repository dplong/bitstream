// Copyright Paul Long 2013.
// Copyright Paul A. Bristow 2013

// Use, modification and distribution are subject to the
// Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// This file is written to be included from a Quickbook .qbk document.
// It can be compiled by the C++ compiler, and run. Any output can
// also be added here as comment or included or pasted in elsewhere.
// Caution: this file contains Quickbook markup as well as code
// and comments: don't change any of the special comment markups!

//[bitstream_tutorial_1

/*`All of the bitstream classes can be used by including the
`bstream.hpp` header file.*/

#include <boost/bitstream/bstream.hpp>

//] [/bitstream_tutorial_1]

// Note how the next bit will not be in the Quickbook docs.

#include <string>
#include <iostream>

int main()
{

//[bitstream_tutorial_2

/*`Since we are first going to write a bit stream,
we need an `obitstream` object to receive the stream.
*/
  boost::obitstream bout;

/*`Just as with the `std::ostringstream` class,
the `<<operator` writes data to the object.

In the following snippet, notice that one can write the value of a literal
and a variable. The number of bits written are implied by the type.
For example, a `bool` type is always a single bit, but an `int` is however
many bits are in an `int`, such as 32. By default, streams are written unaligned,
but there are ways to force alignment, which we won't go into now.
 */
          int i = 63;
          bout << true << i;
/*`Now that the `bout` `stream` contains our data, we are going to read it back out.
We define an `ibitstream` object from which we'll read the `stream`,
using a constructor that accepts the `stream` we just wrote.
*/

          boost::ibitstream bin(bout.data());

/*`Just as with the `std::istringstream` class,
the `>>operator` reads data from the object.
The following code reads the `bool` and `int` from the `stream`
and places their values in the corresponding variables.
 */
          bool whether;
          unsigned j;
          bin >> whether;
          bin >> j;
      }

/*[note The code reads the `int` as an `unsigned`, not a signed `int`.
 That's okay because there is no type information in the stream.]
*/

//] [/bitstream_tutorial_2]  // end of this snippet so int main() is not shown.

} // int main()

