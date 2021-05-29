#include "IndexCreator.h"

// #define LOG_LISTED_FILES
#define FILES_PER_THREAD

namespace fs = std::filesystem;

IndexCreator::IndexCreator(std::string root_path, int n_threads) :
	root_path(root_path)
{
	list_all_files(root_path); 
	int files_per_th = trunc(listed_files.size() / n_threads);
	for (int i = 0; i < n_threads; i++) { // distibute files between threads
		threads.push_back( std::thread(&IndexCreator::indexator, this, files_per_th * i, 
			i == n_threads - 1 ? listed_files.size() - files_per_th * i : files_per_th) );
	}
}

void IndexCreator::indexator(int start_file_id, int files_count) {
	// LOGGING
	#ifdef FILES_PER_THREAD
	cons::print("[DIST] From " + std::to_string(start_file_id) + " to " 
		+ std::to_string(start_file_id + files_count - 1) + "; Thread id = " + thread_id_to_str());
	#endif

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
}

std::string IndexCreator::thread_id_to_str() { // convert current thread's id to string
	std::stringstream ss;
	std::string s;
	ss << std::this_thread::get_id();
	ss >> s;
	return s;
}