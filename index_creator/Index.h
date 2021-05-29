#pragma once

#include <concurrent_unordered_map.h>
#include <algorithm>
#include <list>
#include <string>
#include <vector>

namespace index {

typedef struct {
	unsigned doc_id;
	unsigned row;
	unsigned pos_in_row;
} word_pos;

class Index
{
public:

	std::list<word_pos> search(std::string token); // search single word
	std::list<word_pos> cross_search(std::vector<std::string> tokens); // cross search results

	void add_to_index(const std::string& token, const word_pos& wp);
	void add_to_index_using_sort(const std::string& token, const word_pos& wp);

	void concat(const Index &ind); // add another index to current index in doc_id order

private:
	concurrency::concurrent_unordered_map< std::string, std::list<word_pos> > storage;

	void intersect(std::list<word_pos>& a, std::list<word_pos>& b); /* result of intersection would be written into 'a' list;
																			!! works only with sorted lists !!*/

};

}


