#include "CMS2/NtupleMacrosHeader/interface/CMS2.h"

void Init(CMS2& cms2, TTree* tree)
{
    cms2.Init(tree);
}

void GetEntry(CMS2& cms2, long event)
{
    cms2.GetEntry(event);
}

void LoadAllBranches(CMS2& cms2)
{
    return cms2.LoadAllBranches();
}

bool IsRealData(CMS2& cms2)
{
    return cms2.evt_isRealData();
}

unsigned int Run(CMS2& cms2)
{
    return cms2.evt_run();
}

unsigned int LumiBlock(CMS2& cms2)
{
    return cms2.evt_lumiBlock();
}

unsigned int Event(CMS2& cms2)
{
    return cms2.evt_event();
}
