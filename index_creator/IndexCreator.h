#pragma once

#include <filesystem>
#include <sstream>
#include <fstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/algorithm/string.hpp>

#include "t_output.h"
#include "Index.h"

class IndexCreator
{
public:
	index::Index result_index;

	IndexCreator(std::string root_path, int n_threads = 8);

	// indexing
	void indexator(index::Index& ind, int start_file_id, int files_count); // run in separate thread;  processes given paths into single index block
	void join_all();

	void save(std::string path);

	std::string thread_id_to_str();
	


private:
	std::string root_path; // directory for file search
	std::vector<std::string> listed_files;

	void list_all_files(std::string path);

	// threads vars
	std::vector<std::thread> threads;
	std::vector<index::Index> th_index;

	bool is_word(const char ch); // true if letter or digit
};

inline bool IndexCreator::is_word(const char ch) { // true if letter or digit
	if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9')) {
		return true; // is word
	}
	return false;
}