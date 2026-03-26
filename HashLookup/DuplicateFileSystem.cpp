#include <iostream>
#include <set>
using namespace std;

void parsePath(string path, std::unordered_map<std::string, std::set<std::string>>& state) {
    size_t start = 0;
    string directory_path;
    size_t found = path.find_first_of(" ", start);
    if (found != std::string::npos) {
        directory_path = path.substr(start, found-start) + "/";
        start = found+1; // reaches the first char of the file
    }

    while (start < path.size()) {
        found = path.find_first_of("(", start);
        string fileName;
        if (found != std::string::npos) {
            fileName = path.substr(start, found-start);
            start = found+1; // reaches the first char of the file content
        }
        found = path.find_first_of(")", start);
        string content;
        if (found != std::string::npos) {
            content = path.substr(start, found-start);
            start = found+2; // either reaches the next file or end of the path string
        }

        state[content].insert(directory_path + fileName);
    }
}

/**
 * 
 * Group them by content. Reading Content requires intensive I/O.
 * We can also group them by INODE.
 * 
 * https://prachub.com/interview-questions/design-duplicate-file-detection-using-size
 * 
 * Stream directory partition - do not construct the adjacency list for 1 Million directory consuming memory
 * DFS(currentDir):
    resolvedDir = realpath(currentDir)
    if visited_dirs.count(resolvedDir) → return   // skip directory
    visited_dirs.insert(resolvedDir)

    opendir(currentDir)
    for each entry: // This is basically a map of <directory> : <List of sub-directory>
        if directory:
            DFS(entry)
        if file:
            resolvedFile = realpath(entry)
            inodeToFiles[inode].insert(resolvedFile)          // guaranteed new
    closedir(currentDir)
 * 
 * https://prachub.com/interview-questions/find-duplicate-files-by-size
 * https://prachub.com/coding-questions/delete-duplicate-files-via-dfs
 */
void findDuplicate(vector<string>& paths) {
    std::unordered_map<std::string, std::set<std::string>> state;
    for (string path : paths) {
        parsePath(path, state);
    }
    
    std::vector<vector<string>> filesToBeDeleted; // Return the list of files to be deleted to have 1 file per content
    for (std::pair<std::string, std::set<std::string>> KeyValue : state) {
        if (KeyValue.second.size() > 1) {
            std::vector<std::string> fileToBeDeleted;
            for (auto itr = KeyValue.second.begin(); itr != KeyValue.second.end(); itr++) {
                if (itr != KeyValue.second.begin()) {
                    fileToBeDeleted.push_back(*itr);
                }
            }
            filesToBeDeleted.push_back(fileToBeDeleted);
        }
    }
    

    // Print filesToBeDeleted
    cout << "Files to be deleted:" << endl;
    for (const vector<string>& group : filesToBeDeleted) {
        for (const string& file : group) {
            cout << "  " << file << endl;
        }
    }

    // Print duplicateFileResults
    cout << "\nDuplicate file groups:" << endl;
    for (std::pair<std::string, std::set<std::string>> KeyValue : state) {
        cout << "  Content: " << KeyValue.first << " : ";
        for (const string& file : KeyValue.second) {
            cout << file << "  ";
        }
        cout << endl;
    }
}

int main() {
    std::vector<std::string> paths {"root/a 1.txt(abcd) 2.txt(efgh)", "root/c 3.txt(abcd)", "root/c/d 4.txt(efgh)", "root 4.txt(efgh)"};
    findDuplicate(paths);
}