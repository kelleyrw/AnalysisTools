#ifndef AT_DILEPTONCHARGETYPE_H
#define AT_DILEPTONCHARGETYPE_H

#include <string>

namespace at 
{
	// flavor
	struct DileptonChargeType
	{
		enum value_type
		{
			SS,  // same sign
			SF,  // single fake
			DF,  // double fake
			OS,  // opposite sign
			static_size
		};
	};

	// return the name of the name the DileptonChargeType
	std::string GetDileptonChargeTypeName(const DileptonChargeType::value_type& charge_type);

	// return the title for ROOT of the name the DileptonChargeType
	std::string GetDileptonChargeTypeTitle(const DileptonChargeType::value_type& charge_type);

	// return the title for latex of the name the DileptonChargeType
	std::string GetDileptonChargeTypeLatex(const DileptonChargeType::value_type& charge_type);

	// get the type from the name
	DileptonChargeType::value_type GetDilepChargeTypeFromName(std::string charge_type_name); 

	// get the type from the name number
	DileptonChargeType::value_type GetDilepChargeTypeFromNumber(const int charge_type_num); 
}

#endif // AT_DILEPTONCHARGETYPE_H 
