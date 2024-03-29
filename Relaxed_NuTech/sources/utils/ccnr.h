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
#ifndef CCNR_H
#define CCNR_H

//heads
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <string.h>
#include <stdio.h>
#include <ctime>
#include <vector>
//these two h files are for timing under linux
#include <sys/times.h> 
#include <unistd.h>
using namespace std;
//---------------------

namespace CCNR {

//--------------------------
//functions in basis.h & basis.cpp
struct lit {
	unsigned char sense:1;	//is 1 for true literals, 0 for false literals.
	int clause_num:31;		//clause num, begin with 0
	int var_num;			//variable num, begin with 1
	lit(int the_lit, int the_clause) {
		var_num = abs(the_lit);
		clause_num = the_clause;
		sense = the_lit>0 ? 1: 0;
		}
	struct lit& operator^=(const struct lit &l) {
		sense ^= l.sense;
		clause_num ^= l.clause_num;
		var_num ^= l.var_num;
		return *this;
	}
	void reset(void) {
		sense = 0;
		clause_num = 0;
		var_num = 0;
	}
	bool operator==(const struct lit &l) const {
		return sense == l.sense && clause_num == l.clause_num && var_num == l.var_num;
	}
	bool operator!=(const struct lit &l) const {
		return !(*this == l);
	}
};
struct variable {
	vector<lit> literals;
	vector<int>	neighbor_var_nums;	
	long long	score;
	long long	last_flip_step;
	int			unsat_appear;	
	bool		cc_value;
	bool		is_in_ccd_vars;
};
struct clause {
	vector<lit> 	literals;	
	int		sat_count;
	int		sat_var;	
	long long	weight;
};
//timing
// static struct tms _start_time;	
// static double get_runtime() {
// 	struct tms stop;
// 	times(&stop);
// 	return (double) (stop.tms_utime - _start_time.tms_utime +stop.tms_stime - _start_time.tms_stime) / sysconf(_SC_CLK_TCK);
// }
// static void  start_timing() { times(&_start_time);}
//;;;;;;;


//---------------------------
//functions in mersenne.h & mersenne.cpp

class Mersenne {
  static const int N = 624;
  unsigned int mt[N];
  int mti;
public:
  Mersenne();             // seed with time-dependent value
  Mersenne(int seed);     // seed with int value; see comments for the seed() method
  Mersenne(unsigned int *array, int count); // seed with array
  Mersenne(const Mersenne &copy);
  Mersenne &operator=(const Mersenne &copy);
  void seed(int s);
  void seed(unsigned int *array, int len);
  unsigned int next32();  // generates random integer in [0..2^32-1]
  int next31();           // generates random integer in [0..2^31-1]
  double nextClosed();    // generates random float in [0..1], 2^53 possible values
  double nextHalfOpen();  // generates random float in [0..1), 2^53 possible values
  double nextOpen();      // generates random float in (0..1), 2^53 possible values
  int next(int bound);    // generates random integer in [0..bound), bound < 2^31
};

class ls_solver {
public:
	ls_solver();
	bool 	parse_arguments(int argc, char ** argv);
	bool	build_instance(string inst);
	bool	local_search( const vector<char>* init_solution = 0);	
	void 	print_solution(bool need_verify=0);
	void	simple_print();
	int		get_best_cost() {return _best_found_cost;}	
	void	run(int argc, char ** argv);	
//private:
	//formula
	string				_inst_file;
	vector<variable> 	_vars;
	vector<clause>  	_clauses;
	int		_num_vars;
	int		_num_clauses;
	int		_additional_len;
	//data structure used
	vector<int>		_unsat_clauses;
	vector<int>		_index_in_unsat_clauses;
	vector<int>		_unsat_vars;
	vector<int>		_index_in_unsat_vars;
	vector<int>		_ccd_vars;
	//solution information
	vector<char> 	_solution;
	vector<char> 	_best_solution;
	int		_best_found_cost;
	double	_best_cost_time;
	long long 	_step;
	long long 	_mems;
	long long   _max_mems;
	long long	_max_steps;
	int			_max_tries;
	// int			_time_limit;
	//aiding data structure
	Mersenne		_random_gen;		//random generator
	int				_random_seed;
	//algorithmic parameters
	///////////////////////////
	bool 	_aspiration_active;
	int 	_aspiration_score;
	//clause weighting
	int   	_swt_threshold;
	float 	_swt_p;//w=w*p+ave_w*q
	float 	_swt_q;
	int		_avg_clause_weight;
	long long		_delta_total_clause_weight;
	//main functions
	void	initialize(const vector<char>* init_solution = 0);
	void	initialize_variable_datas();
	void	clear_prev_data();
	int		pick_var();
	void	flip(int flipv);
	void	update_cc_after_flip(int flipv);
	void 	update_clause_weights();
	void	smooth_clause_weights();
	//funcitons for basic operations
	void	sat_a_clause(int the_clause);
	void	unsat_a_clause(int the_clause);
	//functions for buiding data structure
	bool	make_space();
	void	build_neighborhood();
	int		get_cost() {return _unsat_clauses.size();}
	//--------------------
	int 	_init_unsat_nums;
	float 	_prob_p;
	vector<int> conflict_ct;
	// struct tms _start_time;	
	// double get_runtime() {
	// 	struct tms stop;
	// 	times(&stop);
	// 	return (double) (stop.tms_utime - _start_time.tms_utime +stop.tms_stime - _start_time.tms_stime) / sysconf(_SC_CLK_TCK);
	// }
	// void  start_timing() { times(&_start_time);}
	// long long _end_step;
	//===================	
};

}
#endif
