/************************************************************************************[ParseUtils.h]
Relaxed, lstech -- Copyright (c) 2019-2021, Shaowei Cai, Xindi Zhang: using local search called CCAnr to improve CDCL.
Reference: Shaowei Cai and Xindi Zhang. "Deep Cooperation of CDCL and Local Search for SAT." In SAT 2021, pp 64-81. "best paper award"

MapleLCMDistChronoBT-DL -- Copyright (c) 2019, Stepan Kochemazov, Oleg Zaikin, Victor Kondratiev, Alexander Semenov:
The solver was augmented with heuristic that moves duplicate learnt clauses into the core/tier2 tiers depending on a number of parameters.

MapleLCMDistChronoBT -- Copyright (c) 2018, Alexander Nadel, Vadim Ryvchin: "Chronological Backtracking" in SAT-2018, pp. 111-121.

Maple_LCM -- Copyright (c) 2017, Mao Luo, Chu-Min LI, Fan Xiao: implementing a learnt clause minimisation approach
Maple_LCM_Dist-- Copyright (c) 2017, Fan Xiao, Chu-Min LI, Mao Luo: using a new branching heuristic called Distance at the beginning of search
Reference: M. Luo, C.-M. Li, F. Xiao, F. Manya, and Z. L. , “An effective learnt clause minimization approach for cdcl sat solvers,” in IJCAI-2017, 2017, pp. to–appear.

MapleSAT -- Copyright (c) 2016, Jia Hui Liang, Vijay Ganesh

Maple_COMPS -- Copyright (c) 2015, Chanseok Oh: Chanseok Oh's MiniSat Patch Series 

MiniSat -- Copyright (c) 2003-2006, Niklas Een, Niklas Sorensson
           Copyright (c) 2007-2010  Niklas Sorensson

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


**************************************************************************************************/

#ifndef Minisat_ParseUtils_h
#define Minisat_ParseUtils_h

#include <stdlib.h>
#include <stdio.h>

#include <zlib.h>

namespace Minisat {

//-------------------------------------------------------------------------------------------------
// A simple buffered character stream class:

static const int buffer_size = 1048576;


class StreamBuffer {
    gzFile        in;
    unsigned char buf[buffer_size];
    int           pos;
    int           size;

    void assureLookahead() {
        if (pos >= size) {
            pos  = 0;
            size = gzread(in, buf, sizeof(buf)); } }

public:
    explicit StreamBuffer(gzFile i) : in(i), pos(0), size(0) { assureLookahead(); }

    int  operator *  () const { return (pos >= size) ? EOF : buf[pos]; }
    void operator ++ ()       { pos++; assureLookahead(); }
    int  position    () const { return pos; }
};


//-------------------------------------------------------------------------------------------------
// End-of-file detection functions for StreamBuffer and char*:


static inline bool isEof(StreamBuffer& in) { return *in == EOF;  }
static inline bool isEof(const char*   in) { return *in == '\0'; }

//-------------------------------------------------------------------------------------------------
// Generic parse functions parametrized over the input-stream type.


template<class B>
static void skipWhitespace(B& in) {
    while ((*in >= 9 && *in <= 13) || *in == 32)
        ++in; }


template<class B>
static void skipLine(B& in) {
    for (;;){
        if (isEof(in)) return;
        if (*in == '\n') { ++in; return; }
        ++in; } }


template<class B>
static int parseInt(B& in) {
    int     val = 0;
    bool    neg = false;
    skipWhitespace(in);
    if      (*in == '-') neg = true, ++in;
    else if (*in == '+') ++in;
    if (*in < '0' || *in > '9') fprintf(stderr, "PARSE ERROR! Unexpected char: %c\n", *in), exit(3);
    while (*in >= '0' && *in <= '9')
        val = val*10 + (*in - '0'),
        ++in;
    return neg ? -val : val; }


// String matching: in case of a match the input iterator will be advanced the corresponding
// number of characters.
template<class B>
static bool match(B& in, const char* str) {
    int i;
    for (i = 0; str[i] != '\0'; i++)
        if (in[i] != str[i])
            return false;

    in += i;

    return true; 
}

// String matching: consumes characters eagerly, but does not require random access iterator.
template<class B>
static bool eagerMatch(B& in, const char* str) {
    for (; *str != '\0'; ++str, ++in)
        if (*str != *in)
            return false;
    return true; }


//=================================================================================================
}

#endif
