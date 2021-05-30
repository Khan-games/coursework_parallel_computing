#pragma once

#include <concurrent_unordered_map.h>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/list.hpp>
#include <fstream>
#include <algorithm>
#include <list>
#include <string>
#include <vector>
#include <map>

#include "../index_creator/t_output.h"

namespace index {

typedef struct {
	unsigned doc_id;
	unsigned row;
	unsigned pos_in_row;
} word_pos;

class Index
{
public:
	
	Index();
	Index(std::map< std::string, std::list<word_pos> > &storage, 
		int start_doc_id, std::vector<std::string> &paths);
	
	void set_files_parameters(int set_start_doc_id, std::vector<std::string>& set_paths);

	// search
	std::list<word_pos> search(std::string &token); // search single word
	std::vector<std::pair<std::string, std::list<word_pos> > > cross_search(std::vector<std::string> &tokens); // cross search results

	// add
	void add_to_index(const std::string& token, const word_pos& wp);
	void add_to_index_using_sort(const std::string& token, const word_pos& wp);

	// file maintain
	void save_to_file(std::string path);
	void load_from_file(std::string path);

	void concat(Index &ind); // add another index to current index in doc_id order

	// get
	int get_start_doc_id();
	std::map< std::string, std::list<word_pos> >* get_storage();
	std::vector<std::string>* get_paths();

	bool operator == (const Index& ind) const;

private:

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar & storage;
		ar & start_doc_id;
		ar & paths;
	}

	std::map< std::string, std::list<word_pos> > storage;

	int start_doc_id;
	std::vector<std::string> paths;
	
	void intersect(std::list<word_pos>& a, std::list<word_pos>& b); /* result of intersection would be written into 'a' list;
																			!! works only with sorted lists !!*/
	void intersect(std::vector<std::pair<std::string, std::list<word_pos> > >& a, std::list<word_pos>& b);
};

// serialization for word_pos
template<class Archive>
inline void serialize(
	Archive& ar,
	word_pos& wp,
	const unsigned int file_version
) {
	ar & wp.doc_id;
	ar & wp.row;
	ar & wp.pos_in_row;
}

// comparison word_pos
inline bool operator==(const word_pos& s1, const word_pos& s2)
{
	if (s1.doc_id == s2.doc_id && s1.row == s2.row && s1.pos_in_row == s2.pos_in_row) {
		return true;
	}
	else {
		return false;
	}	
}

// index compare overload
inline bool Index::operator== (const Index& ind) const {
	if (storage == storage && start_doc_id == start_doc_id && paths == paths) {
		return true;
	}
	else {
		return false;
	}
}

}


