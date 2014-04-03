#ifndef AT_DORKYEVENTIDENTFIER_H
#define AT_DORKYEVENTIDENTFIER_H

namespace at
{
    struct DorkyEventIdentifier 
    {
        // this is a workaround for not having unique event id's in MC
        unsigned long int run, event, lumi;
        bool operator < (const DorkyEventIdentifier &) const;
        bool operator == (const DorkyEventIdentifier &) const;
    };

    bool is_duplicate(const DorkyEventIdentifier &id);

} // namespace at

#endif // AT_DORKYEVENTIDENTFIER_H
