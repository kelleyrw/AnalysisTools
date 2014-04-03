#ifndef AT_CMS2TAG_H
#define AT_CMS2TAG_H

namespace at
{
    struct CMS2Tag 
    {
        int release;
        int sub_release;
        int version;
    };

    // method to return the cms2 tag from the cms2 ntuple
    CMS2Tag GetCMS2Tag();

} // namespace at

#endif // AT_CMS2TAG_H
