#ifndef AT_DILEPTONHYPTYPE_H
#define AT_DILEPTONHYPTYPE_H

#include <string>

namespace at 
{
    // flavor
    struct DileptonHypType
    {
        enum value_type
        {
            ALL,  // 0
            MUMU, // 1
            EMU,  // 2
            EE,   // 3
            static_size
        };
    };

    // return the name of the name the DileptonHypType
    std::string GetDileptonHypTypeName(const DileptonHypType::value_type& hyp_type);
    std::string GetDileptonHypTypeName(const int hyp_type);

    // return the title for ROOT of the name the DileptonHypType
    std::string GetDileptonHypTypeTitle(const DileptonHypType::value_type& hyp_type);

    // return the title for latex of the name the DileptonHypType
    std::string GetDileptonHypTypeLatex(const DileptonHypType::value_type& hyp_type);

    // convert the int to a DileptonHypType
    DileptonHypType::value_type hyp_typeToHypType(const int hyp_type);

    // convert the two pdgid's into hype type a DileptonHypType
    DileptonHypType::value_type PdgidToHypType(int id1, int id2);

} // namespace at

#endif // AT_DILEPTONHYPTYPE_H
