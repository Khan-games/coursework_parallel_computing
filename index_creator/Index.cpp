#include "Index.h"

using namespace index;

Index::Index() {

}

Index::Index(std::map< std::string, std::list<word_pos> >& storage,
	int start_doc_id, std::vector<std::string>& paths) :
	storage(storage),
	start_doc_id(start_doc_id),
	paths(paths)
{

}

void Index::set_files_parameters(int set_start_doc_id, std::vector<std::string>& set_paths) {
	start_doc_id = set_start_doc_id;
	paths = set_paths;
}

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

std::list<word_pos> Index::search(std::string &token) {
	return storage[token];
}

void Index::intersect(std::vector<std::pair<std::string, 
	std::list<word_pos> > >& a, std::list<word_pos>& b) { /* result of intersection would be written into 'a' list;
														  !! works only with sorted lists !!*/

	int a_index = 0;
	std::list<word_pos>::iterator a_iter = a[a_index].second.begin();
	std::list<word_pos>::iterator b_iter = b.begin();

	// intersecting in one-pass
	while (a_index < a.size() && b_iter != b.end()) {
		if (a_iter->doc_id == b_iter->doc_id) {
			a_iter++;
			b_iter++;
		}
		else if (a_iter->doc_id > b_iter->doc_id) {
			b_iter++;
		}
		else { // a_iter->doc_id < b_iter->doc_id
			a_iter = a[a_index].second.erase(a_iter);
		}
		// next 'a' token
		if (a_iter == a[a_index].second.end()) {
			a_index++;
			a_iter = a[a_index].second.begin();
		}
	}

	// after work
	if (b_iter == b.end()) {
		while (a_iter != a[a_index].second.end()) {
			a_iter = a[a_index].second.erase(a_iter);
		}
		a.erase(a.begin() + a_index, a.end());
	}

	// delete empty tokens
	std::vector<std::pair<std::string, std::list<word_pos> > > temp_a;
	for (int i = 0; i < a.size(); i++) {
		if (!a[i].second.empty()) {
			temp_a.push_back(a[i]);
		}
	}
	a = temp_a;
}

std::vector<std::pair<std::string, std::list<word_pos> > > Index::cross_search(std::vector<std::string> &tokens) {
	// typedefs
	typedef std::vector<std::pair<std::string, std::list<word_pos> > > return_type;
	typedef std::pair<std::string, std::list<word_pos> > pair_type;
	typedef std::vector< std::list<word_pos> > search_type;

	// preparation
	if (tokens.size() == 0) return return_type();
	search_type search_results(tokens.size()); // prevent additional calls
	return_type temp_result;

	// add first search result
	search_results[0] = search(tokens[0]);
	temp_result.push_back( pair_type(tokens[0], search_results[0]) );

	// pairwise intersect
	for (int i = 1; i < tokens.size(); i++) {
		search_results[i] = search(tokens[i]); // search next token
		//temp_result.push_back( pair_type(tokens[i], std::list<word_pos>()) ); // append temp result
		intersect( temp_result[0].second, search_results[i] ); // return would be in result var
	}

	// temp_result now contains all doc_id with first word
	// adding another words to result
	for (int i = 1; i < search_results.size(); i++) {
		temp_result.push_back(pair_type(tokens[i], std::list<word_pos>())); // append temp result
		for (auto& wp : search_results[i]) {
			for (auto& tr : temp_result[0].second) {
				if (wp.doc_id == tr.doc_id) {
					temp_result[i].second.push_back(wp);
					break;
				}
				else if (wp.doc_id < tr.doc_id) {
					break;
				}
			}
		}
	}

	return temp_result;
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

void Index::concat(Index& ind) {
	// iterate over given index storage
	auto stor_iter = ind.storage.begin();
	while (stor_iter != ind.storage.end()) {
		// iterate over list with positions
		for (const auto& i : stor_iter->second) {
			//add_to_index_using_sort(stor_iter->first, i);
			add_to_index(stor_iter->first, i);
		}
		stor_iter++;
	}
}

int Index::get_start_doc_id() {
	return start_doc_id;
}

std::map< std::string, std::list<word_pos> >* Index::get_storage() {
	return &storage;
}

std::vector<std::string>* Index::get_paths() {
	return &paths;
}

void Index::save_to_file(std::string path) {
	std::ofstream ofs(path);
	{
		boost::archive::text_oarchive oa(ofs);
		oa << (*this); // save itself
	}
	ofs.close();
}

void Index::load_from_file(std::string path) {
	std::ifstream ifs(path);
	boost::archive::text_iarchive ia(ifs);
	ia >> (*this); // load itself
}

