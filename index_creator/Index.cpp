#include "Index.h"

using namespace index;

void Index::intersect(std::list<word_pos>& a, std::list<word_pos>& b) { /* result of intersection would be written into 'a' list;
																    !! works only with sorted lists !!*/
	std::list<word_pos>::iterator a_iter = a.begin();
	std::list<word_pos>::iterator b_iter = b.begin();

	// intersecting in one-pass
	while (a_iter != a.end() && b_iter != b.end()) {
		if (a_iter->doc_id == b_iter->doc_id) {
			a_iter++;
			b_iter++;
		}
		else if (a_iter->doc_id > b_iter->doc_id) {
			b_iter++;
		}
		else { // a_iter->doc_id < b_iter->doc_id
			a_iter = a.erase(a_iter);
		}
	}

	// after work
	if (b_iter == b.end()) {
		while (a_iter != a.end()) {
			a_iter = a.erase(a_iter);
		}
	}
}

std::list<word_pos> Index::search(std::string token) {
	return storage[token];
}

std::list<word_pos> Index::cross_search(std::vector<std::string> tokens) {
	if (tokens.size() == 0) return std::list<word_pos>();
	std::vector< std::list<word_pos> > search_results(tokens.size()); // prevent additional calls
	search_results[0] = search(tokens[0]);
	std::list<word_pos> temp_result = search_results[0];

	// pairwise intersect
	for (int i = 1; i < tokens.size(); i++) {
		search_results[i] = search(tokens[i]);
		intersect(temp_result, search_results[i]); // return would be in result var
	}

	// temp_result now contains all doc_id with first word
	// adding another words to result
	std::list<word_pos> result;
	for (int i = 0; i < search_results.size(); i++) {
		for (auto& wp : search_results[i]) {
			for (auto& tr : temp_result) {
				if (wp.doc_id == tr.doc_id) {
					result.push_back(wp);
					break;
				}
				else if (wp.doc_id > tr.doc_id) {
					break;
				}
			}
		}
	}

	return result;
}

void Index::add_to_index(const std::string& token, const word_pos& wp) {
	storage[token].push_back(wp);
}

void Index::add_to_index_using_sort(const std::string &token, const word_pos &wp) {
	std::list<word_pos>& stg = storage[token]; // save link to prevent additional calls
	
	// sort insert in order: doc_id, row, pos_in_row
	std::list<word_pos>::iterator stg_iter = stg.begin();
	while (stg_iter != stg.end()) {
		if (wp.doc_id == stg_iter->doc_id) {
			if (wp.row == stg_iter->row) {
				if (wp.pos_in_row < stg_iter->pos_in_row) {
					stg.insert(stg_iter, wp);
					break;
				} 
			}
			else if (wp.row < stg_iter->row) {
				stg.insert(stg_iter, wp);
				break;
			}
		}
		else if (wp.doc_id < stg_iter->doc_id) {
			stg.insert(stg_iter, wp);
			break;
		}
		stg_iter++;
	}
	
	// insert to the end
	if (stg_iter == stg.end()) {
		stg.insert(stg_iter, wp);
	}
		
}

void Index::concat(const Index& ind) {
	// iterate over given index storage
	auto stor_iter = ind.storage.begin();
	while (stor_iter != ind.storage.end()) {
		// iterate over list with positions
		for (const auto& i : stor_iter->second) {
			add_to_index_using_sort(stor_iter->first, i);
		}
		stor_iter++;
	}
}
