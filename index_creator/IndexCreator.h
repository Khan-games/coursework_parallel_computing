#pragma once

#include <filesystem>
#include <sstream>

#include "t_output.h"
#include "Index.h"

class IndexCreator
{
public:
	IndexCreator(std::string root_path, int n_threads = 8);

	// indexing
	void indexator(int start_file_id, int files_count); // run in separate thread;  processes given paths into single index block

	// threads 
	void join_all();

	// get
	std::string thread_id_to_str();
	

private:
	std::string root_path; // directory for file search
	std::vector<std::string> listed_files;

	void list_all_files(std::string path);

	// threads vars
	std::vector<std::thread> threads;

	
	
};

