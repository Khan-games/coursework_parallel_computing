#include "IndexCreator.h"

// #define LOG_LISTED_FILES
#define LOG_FILES_PER_THREAD
#define LOG_FILES_FINISHED

// #define TEST_SAVE_LOAD

namespace fs = std::filesystem;

IndexCreator::IndexCreator(std::string root_path, int n_threads) :
	root_path(root_path)
{
	list_all_files(root_path); 
	int files_per_th = trunc(listed_files.size() / n_threads);
	th_index.resize(n_threads);

	// distibute files between threads
	for (int i = 0; i < n_threads; i++) { 
		threads.push_back( std::thread(&IndexCreator::indexator, this, std::ref(th_index[i]), files_per_th * i, 
			i == n_threads - 1 ? listed_files.size() - files_per_th * i : files_per_th) );
	}
}

void IndexCreator::indexator(index::Index& ind, int start_file_id, int files_count) {
	// LOGGING
	#ifdef LOG_FILES_PER_THREAD
	cons::print("[DIST] From " + std::to_string(start_file_id) + " to " 
		+ std::to_string(start_file_id + files_count - 1) + "; Thread id = " + thread_id_to_str());
	#endif

	for (int path_i = start_file_id; path_i < start_file_id + files_count; path_i++) {
		std::ifstream fin;
		// file opening
		try {
			fin.open(listed_files[path_i]);
		}
		catch (const std::ifstream::failure& e) {
			cons::print("[ERROR] File \"" + listed_files[path_i] + "\" failed to open.  Thread id = "
				+ thread_id_to_str(), RED);
		}

		/*cons::print("[FILE] File \"" + listed_files[path_i] + "\" opened.  Thread id = "
			+ thread_id_to_str(), YELLOW);*/

		

		// read file
		std::string str; // read one row
		int row_index = 0; // current row
		while (std::getline(fin, str)) {

			// parse row
			bool was_word = false;
			int start_pos = 0;
			std::string token;
			for (int end_pos = 0; end_pos < str.length(); end_pos++) {
				if (is_word(str[end_pos])) {
					if (!was_word) start_pos = end_pos;
					was_word = true;
				}
				else if (was_word) { // word found
					was_word = false;
					// create token
					std::string token = str.substr(start_pos, end_pos - start_pos);
					boost::to_lower(token);
					// add token to index
					index::word_pos wp = { path_i, row_index, start_pos };
					ind.add_to_index(token, wp);
				}
			}
			if (was_word) {
				// create token
				std::string token = str.substr(start_pos, str.size() - start_pos);
				boost::to_lower(token);
				// add token to index
				index::word_pos wp = { path_i, row_index, start_pos };
				ind.add_to_index(token, wp);
			}
			row_index++;

		}

		fin.close();

		// logging
		#ifdef LOG_FILES_FINISHED
		cons::print("[FILE] File \"" + listed_files[path_i] + "\" performing is done.  Thread id = "
			+ thread_id_to_str(), GREEN);
		#endif
	}

}

void IndexCreator::list_all_files(std::string path) {
	fs::path pathToShow(path);
	for (const auto& entry : fs::recursive_directory_iterator(path)) {
		// LOGGING
		#ifdef LOG_LISTED_FILES
		cons::print("[LIST] " + entry.path().string(), entry.is_directory() ? CYAN : WHITE);
		#endif
		
		// add files paths to vector
		if (!entry.is_directory()) {
			listed_files.push_back(entry.path().string());
		}
	}
}

void IndexCreator::join_all() {
	for (int i = 0; i < threads.size(); i++) {
		threads[i].join();
	}
	threads.clear();

	// unite all indices
	cons::print("[UNITE] Blocks merging started.", YELLOW);
	for (int i = 0; i < th_index.size(); i++) {
		cons::print("[UNITE] Connecting block number " + std::to_string(i) + ".", CYAN);
		result_index.concat(th_index[i]);
	}
	result_index.set_files_parameters(0, listed_files);
	cons::print("[UNITE] All blocks are merged into one.", GREEN);
}

void IndexCreator::save(std::string path) {
	cons::print("[SAVE] Saving process has started.", YELLOW);
	result_index.save_to_file(path);
	cons::print("[SAVE] Result index was saved.", GREEN);

	// testing save-load
	#ifdef TEST_SAVE_LOAD
	cons::print("[LOAD] Loading process has started.", YELLOW);
	index::Index loaded_index;
	loaded_index.load_from_file(path);
	cons::print("[LOAD] Result index was loaded.", YELLOW);
	if (result_index == loaded_index) {
		cons::print("[LOAD] Loaded index matches result index.", GREEN);
	}
	else {
		cons::print("[LOAD] Loaded index doesn't match result index.", RED);
	}
	#endif
}

std::string IndexCreator::thread_id_to_str() { // convert current thread's id to string
	std::stringstream ss;
	std::string s;
	ss << std::this_thread::get_id();
	ss >> s;
	return s;
}