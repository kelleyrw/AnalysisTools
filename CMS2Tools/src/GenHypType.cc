#include "AnalysisTools/CMS2Tools/interface/GenHypType.h"
#include "CMS2/NtupleMacrosHeader/interface/CMS2.h"

namespace at
{
    // construct:
    GenHyp::GenHyp(const GenInfo& l1, const GenInfo& l2)
        : m_lep1(l1)
        , m_lep2(l2)
        , m_type(GenHypType::static_size)
    {
        m_type = Type();
    }

    // methods:
    const GenInfo& GenHyp::Lep1() const
    {
        return m_lep1;
    }

    const GenInfo& GenHyp::Lep2() const
    {
        return m_lep2;
    }

    LorentzVector GenHyp::P4() const
    {
        const LorentzVector& l1_p4 = abs(m_lep1.id)==15 ? m_lep1.d_p4 : m_lep1.p4;
        const LorentzVector& l2_p4 = abs(m_lep2.id)==15 ? m_lep2.d_p4 : m_lep2.p4;
        return (l1_p4 + l2_p4);
    }

    GenHypType::value_type GenHyp::Type() const
    {
        if      (abs(m_lep1.id) == 13 and abs(m_lep2.id) == 13) {return GenHypType::MuMu   ;}
        else if (abs(m_lep1.id) == 11 and abs(m_lep2.id) == 13) {return GenHypType::EMu    ;}
        else if (abs(m_lep1.id) == 13 and abs(m_lep2.id) == 11) {return GenHypType::MuE    ;}
        else if (abs(m_lep1.id) == 11 and abs(m_lep2.id) == 11) {return GenHypType::EE     ;}
        else if (abs(m_lep1.id) == 15 and abs(m_lep2.id) == 15) {return GenHypType::TauTau ;}
        else if (abs(m_lep1.id) == 15 and abs(m_lep2.id) == 13) {return GenHypType::TauMu  ;}
        else if (abs(m_lep1.id) == 13 and abs(m_lep2.id) == 15) {return GenHypType::MuTau  ;}
        else if (abs(m_lep1.id) == 15 and abs(m_lep2.id) == 11) {return GenHypType::TauE   ;}
        else if (abs(m_lep1.id) == 11 and abs(m_lep2.id) == 15) {return GenHypType::ETau   ;}
        else {return GenHypType::static_size;}
    }
           
    bool GenHyp::IsFromZ() const
    {
        return (m_lep1.mom_id==23 && m_lep2.mom_id==23);
    }

    bool GenHyp::IsEE() const
    {
        return (m_type==GenHypType::EE);
    }

    bool GenHyp::IsMuMu() const
    {
        return (m_type==GenHypType::MuMu);
    }

    bool GenHyp::IsTauTau() const
    {
        return (m_type==GenHypType::TauTau);
    }

    bool GenHyp::IsEE_IncludeTaus() const
    {
        switch(Type())
        {
            case GenHypType::EE    : return true; break;
            case GenHypType::TauTau: return (abs(m_lep1.d_id)==11 && abs(m_lep2.d_id)==11); break;
            case GenHypType::TauE  : return (abs(m_lep1.d_id)==11 && abs(m_lep2.id  )==11); break;
            case GenHypType::ETau  : return (abs(m_lep1.id  )==11 && abs(m_lep2.d_id)==11); break;
            default: return false;
        }
    }

    bool GenHyp::IsMuMu_IncludeTaus() const
    {
        switch(Type())
        {
            case GenHypType::MuMu  : return true; break;
            case GenHypType::TauTau: return (abs(m_lep1.d_id)==13 && abs(m_lep2.d_id)==13); break;
            case GenHypType::TauMu : return (abs(m_lep1.d_id)==13 && abs(m_lep2.id  )==13); break;
            case GenHypType::MuTau : return (abs(m_lep1.id  )==13 && abs(m_lep2.d_id)==13); break;
            default: return false;
        }
    }

    bool GenHyp::IsOS() const
    {
        return (m_lep1.id*m_lep2.id < 0);
    }

    bool GenHyp::IsSS() const
    {
        return (m_lep1.id*m_lep2.id > 0);
    }

    bool GenHyp::IsSF() const
    {
        return (Type()==GenHypType::EE or Type()==GenHypType::MuMu or Type()==GenHypType::TauTau); 
    }

    bool GenHyp::IsOSSF() const
    {
        return (IsOS() && IsSF()); 
    }

    bool GenHyp::IsAccepted(const double min_pt, const double max_eta) const
    {
        if (m_lep1.p4.pt() < min_pt)         {return false;}
        if (m_lep2.p4.pt() < min_pt)         {return false;}
        if (fabs(m_lep1.p4.eta()) > max_eta) {return false;}
        if (fabs(m_lep2.p4.eta()) > max_eta) {return false;}
        if (1.4442 < fabs(m_lep1.p4.eta()) && fabs(m_lep1.p4.eta()) < 1.566) {return false;}
        if (1.4442 < fabs(m_lep2.p4.eta()) && fabs(m_lep2.p4.eta()) < 1.566) {return false;}
        return true;
    }

    std::vector<GenHyp> GetGenHyps(const double min_pt, const double max_eta)
    {
        std::vector<GenInfo> gen_infos;
        for (size_t gen_idx = 0; gen_idx < tas::genps_p4().size(); ++gen_idx)
        {
            // only keep status 3
            if (cms2.genps_status().at(gen_idx) != 3) {continue;}

            // only keep charged leptons
            const int id            = tas::genps_id().at(gen_idx);
            const int abs_id        = abs(id); 
            const bool is_lep       = (abs_id == 11 || abs_id==13 || abs_id==15);
            if (not is_lep) {continue;}
            const int charge        = -1*id/abs_id; // 11 == e^- and -11 == e+

            // kinematic cuts
            const LorentzVector& p4 = tas::genps_p4().at(gen_idx);
            if (p4.pt() < min_pt || fabs(p4.eta()) > max_eta) {continue;}

            // keep this lepton
            const int mom_id = tas::genps_id_mother().at(gen_idx);
            GenInfo gen_info = {p4, static_cast<int>(gen_idx), id, charge, mom_id, -1, -1, LorentzVector(-999, -999, -999, -999)}; 

            // e/mu block
            if (abs_id == 11 || abs_id == 13)
            {
                gen_infos.push_back(gen_info);
            }

            // tau block
            // need to add protection here to not get more than one lepton from the same tau
            if (abs_id == 15)
            {
                // only consider tau --> nu_tau + nu_lep + lep events
                // we count neutrino's because that guarantees that 
	            // there is a corresponding lepton and that it comes from
	            // a leptonic tau decay. You can get electrons from converted photons
	            // which are radiated by charged pions from the tau decay but thats
	            // hadronic and we don't care for those 
                const auto& tds = tas::genps_lepdaughter_id().at(gen_idx);
                if (std::find_if(tds.begin(), tds.end(), [](const int& id) {return (abs(id)==12 || abs(id)==14);}) == tds.end()) {continue;}

                for (size_t d_idx = 0; d_idx < tas::genps_lepdaughter_id().at(gen_idx).size(); ++d_idx)
                {
                    // check flavor
                    const int d_id = tas::genps_lepdaughter_id().at(gen_idx).at(d_idx);
                    if (abs(d_id) == 11 || abs(d_id)==13)
                    {
                        // check kinematic
                        const LorentzVector& d_p4 = tas::genps_lepdaughter_p4().at(gen_idx).at(d_idx);
                        if (d_p4.pt() < min_pt || fabs(d_p4.eta()) > max_eta) {continue;}

                        // assign values to gen_info
                        gen_info.d_idx = d_idx;
                        gen_info.d_id  = d_id;
                        gen_info.d_p4  = d_p4;
                        break;
                    }
                }

                if (gen_info.idx >= 0) 
                {
                    gen_infos.push_back(gen_info);
                }
            } // end tau block

        } // end genps loop

        // now make gen hyps
        std::vector<GenHyp> result;
        for (size_t idx1 = 0; idx1 < gen_infos.size(); idx1++)
        {
            for (size_t idx2 = idx1 + 1; idx2 < gen_infos.size(); idx2++)
            {
                const auto& gen_l1 = (gen_infos.at(idx1).p4.pt() > gen_infos.at(idx2).p4.pt() ? gen_infos.at(idx1) : gen_infos.at(idx2));
                const auto& gen_l2 = (gen_infos.at(idx1).p4.pt() > gen_infos.at(idx2).p4.pt() ? gen_infos.at(idx2) : gen_infos.at(idx1));
                const GenHyp gen_hyp(gen_l1, gen_l2);
                result.push_back(gen_hyp);
            }
        }
        std::sort(result.begin(), result.end(), Compare);
        return result;
    }

    bool Compare(const GenHyp& hyp1, const GenHyp& hyp2)
    {
        if (hyp1.Type() != hyp2.Type())
        {
            // choose MuMu over EE over TauTau 
            return (hyp1.Type() < hyp2.Type());
        }
        else
        {
            // choose one closer to pdg value of mass of Z-boson
            static const float Mz = 91.1876;
            const float dm1 = fabs(hyp1.P4().mass() - Mz);
            const float dm2 = fabs(hyp2.P4().mass() - Mz);
            return (dm1 < dm2);
        }
    }

} // namepsace at
