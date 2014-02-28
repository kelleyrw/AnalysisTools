#include "AnalysisTools/LanguageTools/interface/OSTools.h"

// helper functions for OS operations (uses c++ name conventions)
// ------------------------------------------------------------------//

// c++ includes
#include <iostream>
#include <cstdlib>

// ROOT includes
#include "TString.h"
#include "TSystem.h"

// boost includes
#define BOOST_FILESYSTEM_NO_DEPRECATED //  we don't want to use any deprecated features
#include "boost/filesystem.hpp"
#include "boost/regex.hpp" 

namespace lt
{
    // format the float to a string
    std::string fmt(const float value, const std::string& format)
    {
        std::string format_string = "%" + format;
        return std::string(Form(format_string.c_str(), value));
    }

    // get an environment variable
    std::string getenv(const std::string& name)
    {
        if (name.empty())
        {
            std::cerr << "[lt::getenv] Warning: rt::genenv: input is NULL.  Did you forget to set the env var?" << std::endl;
            return "";
        }

        // need the :: to access getenv in global namespace (from cstdlib)
        std::string result(::getenv(name.c_str()));
        return result;
    }

    // copy a file 
    bool copy_file(const std::string& source, const std::string& destination)
    {
        // TODO --> use boost to ensure error is write
        return gSystem->Exec(("cp " + source + " " + destination).c_str()) == 0;
    }

    // move a file
    bool move_file(const std::string& source, const std::string& destination)
    {
        // TODO --> use boost to ensure error is write
        return gSystem->Exec(("mv " + source + " " + destination).c_str()) == 0;
    }

    // remove a file 
    void remove_file(const std::string& file_name, const bool force)
    {
        namespace fs = boost::filesystem;
        fs::path p = fs::path(file_name);
        if (force)
        {
            fs::remove_all(p);
        }
        else
        {
            fs::remove(p);
        }
        return;
    }

    // does file exist?
    bool file_exists(const std::string& file_name) 
    {
        namespace fs = boost::filesystem;
        return fs::exists(fs::path(file_name));
    }

    // create a folder (force is the equivalent of POSIX option "-p")
    bool mkdir(const std::string& path_name, bool force)
    {
        // if pathname emty, do nothing...
        if (path_name.empty())
        {
            return true;
        }

        namespace fs = boost::filesystem;
        fs::path p = fs::path(path_name);
        if (!fs::exists(p))
        {
            try
            {
                if (force)
                {
                    return fs::create_directories(p);
                }
                else
                {
                    return fs::create_directory(p);
                }
            }
            catch (std::exception& e)
            {
                std::cout << e.what() << std::endl;
                return false;
            }
        }
        else
        {
            if (!force)
            {
                std::cout << "[lt::mkdir] Warning: " << path_name << " already exists!" << std::endl;
            }
            return false;
        }
    }

    // create a folder from the file_name 
    std::string mkdir_from_filename(const std::string& file_name, const bool force)
    {
        std::string path = lt::dirname(file_name);
        if (path.empty())
        {
            return file_name;
        }
        if (!lt::file_exists(path))
        {
            if (!lt::mkdir(path, force))
            {
                throw std::runtime_error(std::string("[lt::mkdir_from_file] Error: '") + path + "' cannot be created.  Aborting!");
            }
        }
        return file_name;
    }

    // Get a vector of the filenames in a directory
    std::vector<std::string> get_list_of_files(const std::string &path, const bool show_hidden_files)
    {
        namespace fs = boost::filesystem;

        std::vector<std::string> result;
        if (fs::exists(path))
        {
            if (fs::is_directory(path))
            {
                // for each filesystem object in this directory
                // (directories, files, other)
                for (fs::directory_iterator dir_itr(path), dir_end; dir_itr != dir_end; ++dir_itr)
                {
                    // if its a file, add it to our list
                    if (fs::is_regular_file(dir_itr->status()))
                    {
                        std::string file_name = dir_itr->path().filename().string();
                        // kludge until i figure out the more general way
                        if (show_hidden_files)  
                        {
                            result.push_back(path + "/" + file_name);
                        }
                        else
                        {
                            if (file_name.at(0) != '.')  
                            {
                                result.push_back(path + "/" + file_name);
                            }
                        }
                    }
                }
            }
            else // not a directory, must be a file
            {
                result.push_back(path);
            }
        }
        return result;
    }

    // helper function for below
    std::string to_regex_copy(std::string mask)
    {
        std::string rv = boost::regex_replace(boost::regex_replace(boost::regex_replace(mask, boost::regex( "\\." ), "\\\\."), boost::regex("\\?"), "\\."), boost::regex("\\*"), "\\.*");
        return rv;
    }

    // simple ls function
    std::vector<std::string> ls(const std::string& pcszMask)
    {
        namespace fs = boost::filesystem;
        boost::filesystem::path path;
        boost::regex mask;
        if (fs::is_directory(pcszMask))
        {
            path = pcszMask;
            mask = ".*" ;
        }
        else
        {
            path = fs::path(pcszMask).remove_filename();
            mask = to_regex_copy(fs::path(pcszMask).filename().string());
        }

        std::vector<std::string> rv;
        try
        {
            for (fs::directory_iterator dir_itr(path), dir_end; dir_itr != dir_end; ++dir_itr)
            {
                std::string file_name = dir_itr->path().filename().string();
                if (boost::regex_match(file_name, mask))
                {
                    rv.push_back(dir_itr->path().string());
                }
            }
        }
        catch(...)
        {
            // do nothing
        }

        return rv;
    }

    // get the base name of file from a full path (i.e. /path/to/file.txt --> file) 
    std::string basename(const std::string& full_name)
    {
        namespace fs = boost::filesystem;
        return fs::path(full_name).stem().string();
    }

    // get the extension  name of file from a full path (i.e. /path/to/file.txt --> .txt) 
    std::string extension(const std::string& full_name)
    {
        namespace fs = boost::filesystem;
        return fs::path(full_name).extension().string();
    }

    // get the filename file from a full path (i.e. /path/to/file.txt --> file.txt) 
    std::string filename(const std::string& full_name)
    {
        namespace fs = boost::filesystem;
        return fs::path(full_name).filename().string();
    }

    // get the path from a full path (i.e. /path/to/file.txt --> /path/to) 
    std::string dirname(const std::string& full_name)
    {
        namespace fs = boost::filesystem;
        return fs::path(full_name).parent_path().string();
    }

} // namespace lt
