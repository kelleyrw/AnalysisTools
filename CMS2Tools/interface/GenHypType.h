#ifndef AT_GENHYPTYPE_H
#define AT_GENHYPTYPE_H

#include <string> 
#include "TH1.h"
#include "Math/LorentzVector.h"

typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float> > LorentzVector; 

namespace at
{
    struct GenInfo
    {
        // members: 
        LorentzVector p4;
        int idx;
        int id;
        int charge;
        int mom_id;
        int d_idx;
        int d_id;
        LorentzVector d_p4;
    };

    struct GenHypType
    {
        enum value_type
        {
            MuMu,
            EMu,
            MuE,
            EE,
            TauTau,
            TauMu,
            MuTau,
            TauE,
            ETau,
            static_size,
        };
    };

    class GenHyp
    {
        public: 
            // construct:
            GenHyp(const GenInfo& l1, const GenInfo& l2);

            // methods:
            const GenInfo& Lep1() const;
            const GenInfo& Lep2() const;
            LorentzVector P4() const;
            GenHypType::value_type Type() const;
            bool IsFromZ() const;
            bool IsEE() const;
            bool IsMuMu() const;
            bool IsTauTau() const;
            bool IsEE_IncludeTaus() const;
            bool IsMuMu_IncludeTaus() const;
            bool IsOS() const;
            bool IsSS() const;
            bool IsAccepted(const double min_pt, const double max_eta) const;

        private:
            // members:
            GenInfo m_lep1;
            GenInfo m_lep2;
            GenHypType::value_type m_type;
    };

    // non-members:
    bool Compare(const GenHyp& hyp1, const GenHyp& hyp2);

    // get the collection of GenHyps from the event
    std::vector<GenHyp> GetGenHyps(const double min_pt = 0.0, const double min_eta = 10.0);

} // namepsace at

#endif // AT_GENHYPTYPE_H

