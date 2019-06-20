#include <boost/unordered_set.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/functional/hash.hpp>

#include "obmolopener.h"
#include "box.h"
#include "tee.h"

#ifndef SMINA_FLEXINFO_H
#define SMINA_FLEXINFO_H


namespace boost {  namespace tuples {
	inline size_t hash_value(const boost::tuple<char,int,char>& t)
	{
		size_t seed = 0;
		boost::hash_combine(seed, t.get<0>());
		boost::hash_combine(seed, t.get<1>());
		boost::hash_combine(seed, t.get<2>());
		return seed;
	}
  
	inline bool operator==(const boost::tuple<char,int,char> &a, const boost::tuple<char, int, char> &b)
  {
		return a.get<0>() == b.get<0>() && a.get<1>() == b.get<1>() && a.get<2>() == b.get<2>();
	}

	inline bool operator<(const boost::tuple<char,int,char> &a, const boost::tuple<char, int, char> &b)
  {
		if(a.get<0>() < b.get<0>()) return true;
		else if(a.get<0>() > b.get<0>()) return false;
		
		if(a.get<1>() < b.get<1>()) return true;
		else if(a.get<1>() > b.get<1>()) return false;
		
		if(a.get<2>() < b.get<2>()) return true;
		return false;
	}
  
}
}

/* Store information for identifying flexible residues in receptor and
 * provide routines for extracting these residues as needed.
 */
class FlexInfo
{
	double flex_dist;
	boost::unordered_set< boost::tuple<char, int, char> > residues;
	OpenBabel::OBMol distligand;
	tee& log;
public:
	FlexInfo(const std::string& flexres, double flexdist, const std::string& ligand, tee& l);
	bool hasContent() const
	{
		return residues.size() >0 || flex_dist > 0;
	}

	void sanitizeFlexres(OpenBabel::OBMol& receptor);

	void extractFlex(OpenBabel::OBMol& receptor, OpenBabel::OBMol& rigid, std::string& flexpdbqt);

};

#endif /* SMINA_FLEXINFO_H */
