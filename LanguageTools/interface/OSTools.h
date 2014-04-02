#ifndef LT_OSTOOL_H
#define LT_OSTOOL_H

// helper functions for OS operations (uses c++ name conventions)
// ------------------------------------------------------------------//

#include <string>
#include <vector>

namespace lt
{
    // format the float to a string 
    std::string fmt(const float value, const std::string& format = "1.2f");

    // get an environment variable
    std::string getenv(const std::string& name);

    // copy a file
    bool copy_file(const std::string& source, const std::string& destination);

    // move a file
    bool move_file(const std::string& source, const std::string& destination);

    // remove a file (force is the equivalent of POSIX option "-r")
    void remove_file(const std::string& file_name, const bool force = false);

    // file exists
    bool file_exists(const std::string& file_name); 

    // create a folder (force is the equivalent of POSIX option "-p")
    bool mkdir(const std::string& path_name, const bool force = false);

    // create a folder from the file name if it doesn't exist -- returns the file name
    // throws if it can't make the dir
    std::string mkdir_from_filename(const std::string& file_name, const bool force = true);

    // get a vector of strings for all the files in path 
    std::vector<std::string> get_list_of_files(const std::string &path, const bool show_hidden_files = false);

    // simple ls function
    std::vector<std::string> ls(const std::string& mask);

    // get the base name of file from a full path (i.e. /path/to/file.txt --> file) 
    std::string basename(const std::string& full_name);

    // get the extension  name of file from a full path (i.e. /path/to/file.txt --> .txt) 
    std::string extension(const std::string& full_name);

    // get the file name from a full path (i.e. /path/to/file.txt --> file.txt) 
    std::string filename(const std::string& full_name);

    // get the filename file without extension from a full path (i.e. /path/to/file.txt --> /path/to/file) 
    std::string filestem(const std::string& full_name);

    // get the path from a full path (i.e. /path/to/file.txt --> /path/to) 
    std::string dirname(const std::string& full_name);

} // namespace lt

#endif // LT_OSTOOLS_H
