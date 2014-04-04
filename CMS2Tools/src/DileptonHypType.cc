#include "AnalysisTools/CMS2Tools/interface/DileptonHypType.h"
#include <stdexcept> 
#include <cmath> 

using std::abs;

namespace at 
{
    DileptonHypType::value_type hyp_typeToHypType (const int hyp_type)
    {
        switch (hyp_type) 
        {
            case 0: return DileptonHypType::MUMU;
            case 1: return DileptonHypType::EMU;
            case 2: return DileptonHypType::EMU;
            case 3: return DileptonHypType::EE;
            default:
                throw std::domain_error("ERROR: at::hyp_typeToHypType: hyp_type out of bounds -- must be less than 4.");
        };

        return DileptonHypType::ALL;
    }

    DileptonHypType::value_type PdgidToHypType (int id1, int id2)
    {
        id1 = abs(id1);
        id2 = abs(id2);

        if      (id1 == 13 && id2 == 13) {return DileptonHypType::MUMU;}
        else if (id1 == 13 && id2 == 11) {return DileptonHypType::EMU; }
        else if (id1 == 11 && id2 == 13) {return DileptonHypType::EMU; }
        else if (id1 == 11 && id2 == 11) {return DileptonHypType::EE;  }
        else                             {return DileptonHypType::ALL; }
    }


    std::string GetDileptonHypTypeName(const DileptonHypType::value_type& hyp_type)
    {
        switch (hyp_type) 
        {
            case DileptonHypType::ALL:  return "ll";
            case DileptonHypType::MUMU: return "mm";
            case DileptonHypType::EMU:  return "em";
            case DileptonHypType::EE:   return "ee";
            default:
                throw std::domain_error("ERROR: at::GetDileptonHypeName: hyp_type out of bounds");
        };
    }

    std::string GetDileptonHypTypeName(const int hyp_type)
    {
		return GetDileptonHypTypeName(static_cast<DileptonHypType::value_type>(hyp_type));
    }

    std::string GetDileptonHypTypeTitle(const DileptonHypType::value_type& hyp_type)
    {
        switch (hyp_type) 
        {
            case DileptonHypType::ALL:  return "ll";
            case DileptonHypType::MUMU: return "#mu#mu";
            case DileptonHypType::EMU:  return "e#mu";
            case DileptonHypType::EE:   return "ee";
            default:
                throw std::domain_error("ERROR: at::GetDileptonHypeTitle: hyp_type out of bounds");
        };
    }

    std::string GetDileptonHypTypeLatex(const DileptonHypType::value_type& hyp_type)
    {
        switch (hyp_type) 
        {
            case DileptonHypType::ALL:  return "\\ell\\ell";
            case DileptonHypType::MUMU: return "\\mu\\mu";
            case DileptonHypType::EMU:  return "e\\mu";
            case DileptonHypType::EE:   return "ee";
            default:
                throw std::domain_error("ERROR: at::GetDileptonHypeTitle: hyp_type out of bounds");
        };
    }

} // namespace at

