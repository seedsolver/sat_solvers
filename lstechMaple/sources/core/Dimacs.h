/****************************************************************************************[Dimacs.h]
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

#ifndef Minisat_Dimacs_h
#define Minisat_Dimacs_h

#include <stdio.h>

#include "utils/ParseUtils.h"
#include "core/SolverTypes.h"

namespace Minisat {

//=================================================================================================
// DIMACS Parser:

template<class B, class Solver>
static void readClause(B& in, Solver& S, vec<Lit>& lits) {
    int     parsed_lit, var;
    lits.clear();
    for (;;){
        parsed_lit = parseInt(in);
        if (parsed_lit == 0) break;
        var = abs(parsed_lit)-1;
        while (var >= S.nVars()) S.newVar();
        lits.push( (parsed_lit > 0) ? mkLit(var) : ~mkLit(var) );
    }
}

template<class B, class Solver>
static void parse_DIMACS_main(B& in, Solver& S) {
    vec<Lit> lits;
    int vars    = 0;
    int clauses = 0;
    int cnt     = 0;
    for (;;){
        skipWhitespace(in);
        if (*in == EOF) break;
        else if (*in == 'p'){
            if (eagerMatch(in, "p cnf")){
                vars    = parseInt(in);
                clauses = parseInt(in);
                // SATRACE'06 hack
                // if (clauses > 4000000)
                //     S.eliminate(true);
            }else{
                printf("PARSE ERROR! Unexpected char: %c\n", *in), exit(3);
            }
        } else if (*in == 'c' || *in == 'p')
            skipLine(in);
        else{
            cnt++;
            readClause(in, S, lits);
            S.addClause_(lits); }
    }
    if (vars != S.nVars())
        fprintf(stderr, "WARNING! DIMACS header mismatch: wrong number of variables.\n");
    if (cnt  != clauses)
        fprintf(stderr, "WARNING! DIMACS header mismatch: wrong number of clauses.\n");
}

// Inserts problem into solver.
//
template<class Solver>
static void parse_DIMACS(gzFile input_stream, Solver& S) {
    StreamBuffer in(input_stream);
    parse_DIMACS_main(in, S); }

//=================================================================================================

template<class B, class Solver>
static void simple_readClause(B& in, Solver& S, vec<Lit>& lits) {
    int     parsed_lit, var;
    lits.clear();
    for (;;){
        parsed_lit = parseInt(in);
        if (parsed_lit == 0) break;
        var = abs(parsed_lit)-1;
        lits.push( (parsed_lit > 0) ? mkLit(var) : ~mkLit(var) );
    }
}

template<class B, class Solver>
static void check_solution_DIMACS_main(B& in, Solver& S) {
    vec<Lit> lits;
    int vars    = 0;
    int clauses = 0;
    int cnt     = 0;
    bool ok=true;
    for (;;){
        skipWhitespace(in);
        if (*in == EOF) break;
        else if (*in == 'p'){
            if (eagerMatch(in, "p cnf")){
                vars    = parseInt(in);
                clauses = parseInt(in);
                // SATRACE'06 hack
                // if (clauses > 4000000)
                //     S.eliminate(true);
            }else{
                printf("c PARSE ERROR! Unexpected char: %c\n", *in), exit(3);
            }
        } else if (*in == 'c' || *in == 'p')
            skipLine(in);
        else{
            cnt++;
            int parsed_lit, var;
            bool ok=false;
            for(;;) {
                parsed_lit = parseInt(in);
                if (parsed_lit == 0) break; //{printf("\n"); break;}
                var = abs(parsed_lit)-1;
                // printf("%d ", parsed_lit);
                if ((parsed_lit>0 && S.model[var]==l_True) ||
                        (parsed_lit<0 && S.model[var]==l_False))
                    ok=true;
            }
            if (!ok) {
                printf("c clause %d is not satisfied\n", cnt);
                ok=false;
                // break;
            }
        }
    }
    if (cnt  != clauses)
        printf("c WARNING! DIMACS header mismatch: wrong number of clauses.%d %d\n", cnt, clauses);
    else if (ok)
        printf("c solution checked against the original DIMACS file\n");
}

template<class Solver>
static void check_solution_DIMACS(gzFile input_stream, Solver& S) {
    StreamBuffer in(input_stream);
    check_solution_DIMACS_main(in, S); }

//=================================================================================================
}

#endif

