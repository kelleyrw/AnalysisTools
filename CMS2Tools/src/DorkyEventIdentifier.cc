#include "AnalysisTools/CMS2Tools/interface/DorkyEventIdentifier.h"
#include <set>

namespace at
{
    bool DorkyEventIdentifier::operator < (const DorkyEventIdentifier &other) const
    {
        if (run != other.run)
            return run < other.run;
        if (event != other.event)
            return event < other.event;
        if (lumi != other.lumi)
            return lumi < other.lumi;
        return false;
    }

    bool DorkyEventIdentifier::operator == (const DorkyEventIdentifier &other) const
    {
        if (run != other.run) 
            return false;
        if (lumi != other.lumi)
            return false;
        if (event != other.event)
            return false;
        return true;
    }

    static std::set<DorkyEventIdentifier> already_seen;
    bool is_duplicate (const DorkyEventIdentifier &id) 
    {
        std::pair<std::set<DorkyEventIdentifier>::const_iterator, bool> ret = already_seen.insert(id);
        return !ret.second;
    }

} // namespace at
