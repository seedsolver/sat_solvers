/*
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


*/

#ifndef _CCA_H_
#define _CCA_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <sys/times.h> //these two h files are for linux
#include <unistd.h>

// mersenne twist
typedef struct ccanr_randgen ccanr_randgen;
struct ccanr_randgen
{
	unsigned mt[624];
	int 	 mti;
};


// Define a data structure for a literal in the SAT problem.
typedef struct lit lit;

struct lit {
    int clause_num;		//clause num, begin with 0
    int var_num;		//variable num, begin with 1
    int sense;			//is 1 for true literals, 0 for false literals.
};

typedef struct CCAnr CCAnr;

struct CCAnr{
    /*parameters of the instance*/
    int     num_vars;		//var index from 1 to num_vars 
    int     num_clauses;	//clause index from 0 to num_clauses-1
    int		max_clause_len;
    int		min_clause_len;
    int		formula_len;
    double	avg_clause_len;
    double 	ratio;
    int		ave_weight;   //significant score(sigscore) needed for aspiration
    int		delta_total_weight;
    int		threshold;
    float	p_scale;//w=w*p+ave_w*q
    float	q_scale;
    int		scale_ave;//scale_ave==ave_weight*q_scale
    int 	q_init;
    long long  mems_left;
    //cutoff
    int		max_tries;
    int		tries;
    int		max_flips;
    int		step;
    int* 	score_inc_vars;
    int* 	score_inc_flag;
    int 	score_inc_count;
    /* literal arrays */				
    lit**	var_lit;				//var_lit[i][j] means the j'th literal of var i.
    int*	var_lit_count;          //amount of literals of each var
    lit**	clause_lit;		        //clause_lit[i][j] means the j'th literal of clause i.
    int*	clause_lit_count; 	    // amount of literals in each clause		

    
    /* Information about the variables. */
    int*    score;		
    int*	time_stamp;
    int*	fix;
    int*	cscc;

    /* Information about the clauses */			
    int*     clause_weight;	
    int*     sat_count;		
    int*	 sat_var;

    //unsat clauses stack
    int*	unsat_stack;		 //store the unsat clause number
    int		unsat_stack_fill_pointer;
    int*	index_in_unsat_stack;//which position is a clause in the unsat_stack

    //variables in unsat clauses
    int*	unsatvar_stack;		
    int		unsatvar_stack_fill_pointer;
    int*	index_in_unsatvar_stack;
    int*	unsat_app_count;		//a varible appears in how many unsat clauses

    //configuration changed decreasing variables (score>0 and confchange=1)
    int*	goodvar_stack;		
    int		goodvar_stack_fill_pointer;
    int*	already_in_goodvar_stack;

    //unit clauses preprocess
    lit*	unitclause_queue;		
    int		unitclause_queue_beg_pointer;
    int     unitclause_queue_end_pointer;
    int*    clause_delete;

    /* Information about solution */
    char*    cur_soln;	//the current solution, with 1's for True variables, and 0's for False variables
    ccanr_randgen randgen;
	char*	best_soln;
	int 	best_cost;
	// confl_trans
	int*   conflict_ct;
    int    in_conflict_sz;
	int*   in_conflict;

    // preprocess
    int     fix_var_ct;
    int     del_cls_ct;
};


void 	init_CCAnr(CCAnr*);
void 	reinit_CCAnr(CCAnr*);
void 	confl_trans(CCAnr*);


bool    local_search(CCAnr*);
int     build_instance(CCAnr*,char *filename);
void    print_solution(CCAnr*);
int     verify_sol(CCAnr*);
void 	alloc_memory(CCAnr*);
void    free_memory(CCAnr*);
void    settings(CCAnr*, char*);
void    ccanr_merseene_init(CCAnr*, int);
void 	update_after_build(CCAnr*);


#endif

