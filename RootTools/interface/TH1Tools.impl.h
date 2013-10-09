// helper and functions for ROOT TH1 related operations (uses ROOT name conventions)
// -------------------------------------------------------------------------------------------------//

// templated function definitions

// c++ includes
#include <memory>
#include <stdexcept>

// namespace rt --> root tools
namespace rt
{
    // get a single histgram from a root file (client is the owner)
    template <typename TH1Type>
    TH1Type* GetHistFromRootFile(const std::string& file_name, const std::string& hist_name)
    {
        std::auto_ptr<TFile> file(rt::OpenRootFile(file_name));
        TH1Type* hist_ptr = dynamic_cast<TH1Type*>(file->Get(hist_name.c_str()));
        if (!hist_ptr)
        {
            throw std::runtime_error(std::string("[rt::GetHistFromRootFile] Error: '") + hist_name + "' not found.  Aborting!");
        }
        hist_ptr->SetDirectory(0);
        file->Close();
        return hist_ptr;
    }

    // only works on compile time arrays (e.g. float bins[] = {1,2,3};)
    template <int N> unsigned int find_bin(const float value, const float (&bins)[N])
    {
        int index = 0;
        for (size_t i=0; i != N-1; i++)
        {
            if (bins[i] < value && value < bins[i+1]) 
            {
                index=i;
                break;
            }
        }
        return index;
    }

    template <int N> unsigned int find_bin(const double value, const double (&bins)[N])
    {
        int index = 0;
        for (size_t i=0; i != N-1; i++)
        {
            if (bins[i] < value && value < bins[i+1]) 
            {
                index=i;
                break;
            }
        }
        return index;
    }

} // namespace rt
