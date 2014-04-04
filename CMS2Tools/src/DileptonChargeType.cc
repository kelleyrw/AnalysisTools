#include "AnalysisTools/CMS2Tools/interface/DileptonChargeType.h"
#include <stdexcept> 
#include "TString.h"
#include "AnalysisTools/LanguageTools/interface/LanguageTools.h"

namespace at 
{
    std::string GetDileptonChargeTypeName(const DileptonChargeType::value_type& charge_type)
    {
        switch (charge_type) 
        {
            case DileptonChargeType::SS: return "ss";
            case DileptonChargeType::SF: return "sf";
            case DileptonChargeType::DF: return "df";
            case DileptonChargeType::OS: return "os";
            default:
                throw std::domain_error("ERROR: at::GetDileptonChargeName: charge_type out of bounds");
        };
    }

    std::string GetDileptonChargeTypeTitle(const DileptonChargeType::value_type& charge_type)
    {
        switch (charge_type) 
        {
            case DileptonChargeType::SS: return "SS";
            case DileptonChargeType::SF: return "SF";
            case DileptonChargeType::DF: return "DF";
            case DileptonChargeType::OS: return "OS";
            default:
                throw std::domain_error("ERROR: at::GetDileptonChargeTitle: charge_type out of bounds");
        };
    }
    
    std::string GetDileptonChargeTypeLatex(const DileptonChargeType::value_type& charge_type)
    {
        switch (charge_type) 
        {
            case DileptonChargeType::SS: return "SS";
            case DileptonChargeType::SF: return "SF";
            case DileptonChargeType::DF: return "DF";
            case DileptonChargeType::OS: return "OS";
            default:
                throw std::domain_error("ERROR: at::GetDileptonChargeTitle: charge_type out of bounds");
        };
    }

	DileptonChargeType::value_type GetDilepChargeTypeFromName(std::string charge_type_name) 
	{
		charge_type_name = lt::string_lower(charge_type_name);		
        {
            if (charge_type_name == "ss") {return DileptonChargeType::SS;}
            if (charge_type_name == "sf") {return DileptonChargeType::SF;}
            if (charge_type_name == "df") {return DileptonChargeType::DF;}
            if (charge_type_name == "os") {return DileptonChargeType::OS;}
        };
        throw std::domain_error(Form("ERROR: at::GetDilepChargeTypeFromName: name %s not found", charge_type_name.c_str()));
	}

	DileptonChargeType::value_type GetDilepChargeTypeFromNumber(const int charge_type_num) 
	{
        if (charge_type_num < 0) 
        {
            return DileptonChargeType::static_size;
        }
        switch(charge_type_num)
        {
            case 0: return DileptonChargeType::SS; break;
            case 1: return DileptonChargeType::SF; break;
            case 2: return DileptonChargeType::DF; break;
            case 3: return DileptonChargeType::OS; break;
            case 4: return DileptonChargeType::static_size; break;
            default:
                throw std::domain_error(Form("ERROR: at::GetDilepChargeTypeFromName: num %d not found", charge_type_num));
        };
	}
}

