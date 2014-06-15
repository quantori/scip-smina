/*
 * MinimizationQuery.h
 *
 *  Created on: Jun 11, 2014
 *      Author: dkoes
 */

#ifndef MINIMIZATIONQUERY_H_
#define MINIMIZATIONQUERY_H_

#include <vector>

#include "Reorienter.h"
#include "server_common.h"
#include "model.h"
#include <boost/lockfree/queue.hpp>
#include "parse_pdbqt.h"
#include "parsing.h"
#include "custom_terms.h"
#include "weighted_terms.h"
#include "precalculate.h"
#include "naive_non_cache.h"

//store various things that only have to be initialized once for any minimization
struct MinimizationParameters
{
	minimization_params minparms;
	custom_terms t;
	weighted_terms *wt;
	precalculate *prec;
	precalculate_exact *exact_prec;
	naive_non_cache *nnc; //for scoring
	unsigned nthreads;

	MinimizationParameters();
	~MinimizationParameters();
};

//criteria for filtering and  sorting the data
struct MinimizationFilters
{
	double maxScore;
	double maxRMSD;

	unsigned start; //were to start
	unsigned num; //how many to include, if zero then all

	enum SortType { Score=0, RMSD=1, OrigPos=2};
	SortType sort;
	bool reverseSort;

	bool unique;
	MinimizationFilters() :
			maxScore(HUGE_VAL), maxRMSD(HUGE_VAL), sort(Score), start(0), num(0), reverseSort(false), unique(false)
	{
	}

	void read(istream& in)
	{
		in >> maxRMSD;
		in >> maxScore;
		in >> start;
		in >> num;
		unsigned val = 0;
		in >> val;
		sort = (SortType)val;
		in >> reverseSort;
		in >> unique;
	}
};

class MinimizationQuery
{


private:
	class ResultsSorter;

	const MinimizationParameters& minparm;
	bool valid;
	bool stopQuery; //cancelled
	time_t lastAccessed; //last time accessed

	unsigned chunk_size; //how many ligands to process at a time
	bool readAllData; //try after we have consumed all the ligands
	bool hasReorient; //try if ligand data is prefaced by rotation/translation
	model initm;

	stream_ptr io;
	unsigned io_position; //guarded by io_mutex as well
	boost::mutex io_mutex; //protects io

	//holds the result of minimization
	struct Result
	{
		double score;
		double rmsd;
		string name;
		string sdf;
		unsigned position; //location in allResults
		unsigned orig_position; //location in input stream

		Result(): score(0), rmsd(0), position(0),orig_position(0) {}
	};

	Result* minimize(model& m); //return new result

	vector<Result*> allResults; //order doesn't change, minimizers add to this

	boost::shared_mutex results_mutex; //protects allResults

	boost::thread *minimizationSpawner; //manages thread_group of minimization threads

	static void thread_startMinimization(MinimizationQuery* q);
	static void thread_minimize(MinimizationQuery* q);

	void checkThread();

	//this is what is read from the user
	struct LigandData
	{
		Reorienter reorient;
		unsigned numtors;
		parsing_struct p;
		context c;
		unsigned origpos;
	};

	//reads into ligands
	//returns false iff there is no more data to read
	bool thread_safe_read(vector<LigandData>& ligands);

	unsigned loadResults(const MinimizationFilters& filter, vector<Result*>& results);
public:

	MinimizationQuery(const MinimizationParameters& minp, const string& recstr, stream_ptr data,
			bool hasR, unsigned chunks = 10) : minparm(minp),
			valid(true), stopQuery(false), lastAccessed(time(NULL)),
					chunk_size(chunks), readAllData(false), hasReorient(hasR),
					io(data), io_position(0), minimizationSpawner(NULL)
	{
		//create the initial model
		stringstream rec(recstr);
		initm = parse_receptor_pdbqt("rigid.pdbqt", rec);
	}

	~MinimizationQuery();

	bool isValid() const
	{
		return valid;
	}

	void execute(bool block = false);

	//all of the result/output functions can be called while an asynchronous
	//query is running

	//return all current results

	void outputData(const MinimizationFilters& dp, ostream& out);
	//write out all results in sdf.gz format
	void outputMols(const MinimizationFilters& dp, ostream& out);
	//output single mol in sdf format; referenced using current position in processed results array
	void outputMol(unsigned pos, ostream& out);

	//attempt to cancel,
	void cancel() { stopQuery = true; }
	bool finished(); //done minimizing
	bool cancelled()
	{
		return stopQuery;
	} //user no longer cares
	void access()
	{
		lastAccessed = time(NULL);
		stopQuery = false;
	}
	const time_t idle()
	{
		return time(NULL) - lastAccessed;
	} //time since last access

};

#endif /* MINIMIZATIONQUERY_H_ */
