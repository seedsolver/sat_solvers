/****************************************************************************************[System.h]
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

#ifndef Minisat_System_h
#define Minisat_System_h

#if defined(__linux__)
#include <fpu_control.h>
#endif

#include "mtl/IntTypes.h"

//-------------------------------------------------------------------------------------------------

namespace Minisat {

static inline double cpuTime(void); // CPU-time in seconds.
extern double memUsed();            // Memory in mega bytes (returns 0 for unsupported architectures).
extern double memUsedPeak();        // Peak-memory in mega bytes (returns 0 for unsupported architectures).

}

//-------------------------------------------------------------------------------------------------
// Implementation of inline functions:

#if defined(_MSC_VER) || defined(__MINGW32__)
#include <time.h>

static inline double Minisat::cpuTime(void) { return (double)clock() / CLOCKS_PER_SEC; }

#else
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

static inline double Minisat::cpuTime(void) {
    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    return (double)ru.ru_utime.tv_sec + (double)ru.ru_utime.tv_usec / 1000000; }

#endif

#endif
