// c++
#include <iostream>
#include <stdexcept>

// ROOT
#include "TChain.h"
#include "TTreeCache.h"
#include "TBenchmark.h"

// CMS2
#include "CMS2/NtupleMacrosHeader/interface/CMS2.h"

// tools
#include "AnalysisTools/CMS2Tools/interface/DorkyEventIdentifier.h"
#include "AnalysisTools/CMS2Tools/interface/GoodRun.h"
#include "AnalysisTools/RootTools/interface/RootTools.h"
#include "AnalysisTools/LanguageTools/interface/LanguageTools.h"

namespace at
{
    // Peform an analysis on a chain.
    template <typename NtupleClass, typename Analyzer>
    int ScanChain
    (
        TChain* const chain, 
        Analyzer analyzer, 
        NtupleClass& ntuple_class,
        const long num_events,
        const std::string& goodrun_file_name,
        const bool fast,
        const bool verbose,
        const int evt_run,
        const int evt_lumi,
        const int evt_event
    )
    {
        using namespace std;

        // test chain
        if (!chain)
        {
            throw std::invalid_argument("at::ScanChain: chain is NULL!");
        }
        if (chain->GetListOfFiles()->GetEntries()<1)
        {
            throw std::invalid_argument("at::ScanChain: chain has no files!");
        }
        if (not chain->GetFile())
        {
            throw std::invalid_argument("at::ScanChain: chain has no files or file path is invalid!");
        }

        // tree name
        string tree_name = chain->GetName();

        // set the "good run" list 
        if (!goodrun_file_name.empty())
        {
            set_goodrun_file(goodrun_file_name.c_str());
        }

        // set the style
        rt::SetStyle("emruoi");
    
        // benchmark
        TBenchmark bmark;
        bmark.Start("benchmark");
    
        // events counts and max events
        int i_permilleOld = 0;
        long num_events_total = 0;
        long num_events_chain = (num_events >= 0 && num_events < chain->GetEntries()) ? num_events : chain->GetEntries();
        TObjArray* list_of_files = chain->GetListOfFiles();
        TIter file_iter(list_of_files);
        TFile* current_file = NULL;

        // count the duplicates and bad events
        unsigned long duplicates = 0;
        unsigned long bad_events = 0;

        // begin job
        analyzer.BeginJob();

        // loop over files in the chain
        //while ((current_file = (TFile*)file_iter.Next()))
        while ((current_file = static_cast<TFile*>(file_iter.Next())))
        {
            TFile *file = TFile::Open(current_file->GetTitle());
            if (!file || file->IsZombie())
            {
                //std::cout << "Skipping bad input file: " << current_file->GetTitle() << std::endl;
                //continue;
                throw std::runtime_error(Form("File from TChain is invalid or corrupt: %s", current_file->GetTitle()));
            }

            // get the trees in each file
            TTree *tree = dynamic_cast<TTree*>(file->Get(tree_name.c_str()));
            if (!tree || tree->IsZombie())
            {
                //std::cout << "Skipping bad input file (missing tree): " << current_file->GetTitle() << std::endl;
                //continue;
                throw std::runtime_error(Form("File from TChain has an invalid TTree or is corrupt: %s", current_file->GetTitle()));
            }

            if (fast)
            {
                TTreeCache::SetLearnEntries(10);
                tree->SetCacheSize(128*1024*1024);
            }
            Init(ntuple_class, tree);

            // Loop over Events in current file
            if (num_events_total >= num_events_chain) continue;
            long num_events_tree = tree->GetEntriesFast();

            // loop over events to Analyze
            for (long event = 0; event != num_events_tree; ++event)
            {
                // quit if the total is > the number in the chain
                if (num_events_total >= num_events_chain) continue;

                // load the entry
                if (fast) tree->LoadTree(event);
                GetEntry(ntuple_class, event);
                ++num_events_total;

                // pogress
                int i_permille = (int)floor(1000 * num_events_total / float(num_events_chain));
                if (i_permille != i_permilleOld) {
                    printf("  \015\033[32m ---> \033[1m\033[31m%4.1f%%" "\033[0m\033[32m <---\033[0m\015", i_permille/10.);
                    fflush(stdout);
                    i_permilleOld = i_permille;
                }

                unsigned int run = Run(ntuple_class);
                unsigned int ls  = LumiBlock(ntuple_class);
                unsigned int evt = Event(ntuple_class);

                // check run/ls/evt
                if (evt_event >= 0)
                {
                    if (evt==static_cast<unsigned int>(evt_event))
                    {
                        if (verbose) {cout << "selected event:\t" << evt << endl;}
                    }
                    else
                    {
                        continue;
                    }
                }
                if (evt_lumi >= 0)
                {
                    if (ls==static_cast<unsigned int>(evt_lumi))
                    {
                        if (verbose) {cout << "selected lumi:\t" << ls << endl;}
                    }
                    else
                    {
                        continue;
                    }
                }
                if (evt_run >= 0)
                {
                    if (ls==static_cast<unsigned int>(evt_run))
                    {
                        if (verbose) {cout << "selected run:\t" << run << endl;}
                    }
                    else
                    {
                        continue;
                    }
                }

                // filter out events
                if (IsRealData(ntuple_class))
                {
                    if (!goodrun_file_name.empty())
                    {
                        // check for good run and events
                        if(!goodrun(run, ls)) 
                        {
                            if (verbose) {cout << "Bad run and lumi:\t" << run << ", " << ls << endl;}
                            bad_events++;
                            continue;
                        }
                    }

                    // check for dupiclate run and events
                    DorkyEventIdentifier id = {run, evt, ls};
                    if (is_duplicate(id))
                    {
                        if (verbose) {cout << "good run file = " << goodrun_file_name << endl;}
                        duplicates++;
                        continue;
                    }
                }

                // print run/ls/event
                if (verbose)
                {
                    //cout << Form("run %d, ls %d, evt %d", run, ls, evt) << endl;
                }

                // analysis
                analyzer.Analyze(event);

            } // end event loop

            // close current file
            file->Close();
            delete file;

        } // end file loop
    
        // print warning if the totals don't line up
        if (num_events_chain != num_events_total) 
        {
            cout << "Error: number of events from the files " 
                << "(" << num_events_chain << ") " 
                << "is not equal to the total number of events "
                << "(" << num_events_total << ")." 
                << endl;
        }

        // save the output
        analyzer.EndJob();
    
        // the benchmark results 
        // -------------------------------------------------------------------------------------------------//
        bmark.Stop("benchmark");
        cout << endl;
        cout << num_events_total << " Events Processed" << endl;
        cout << "# of bad events filtered = " << bad_events << endl; 
        cout << "# of duplicates filtered = " << duplicates << endl; 
        cout << "------------------------------" << endl;
        cout << "CPU  Time: " << Form("%.01f", bmark.GetCpuTime("benchmark" )) << endl;
        cout << "Real Time: " << Form("%.01f", bmark.GetRealTime("benchmark")) << endl;
        cout << endl;
    
        // done
        return 0;
    }

    // Peform an analysis on a chain.
    // Same as ScanChain except we pass the file name to the analysis object 
    template <typename NtupleClass, typename Analyzer>
    int ScanChainWithFilename
    (
        TChain* const chain, 
        Analyzer analyzer, 
        NtupleClass& ntuple_class,
        const long num_events,
        const std::string& goodrun_file_name,
        const bool fast,
        const bool verbose,
        const int evt_run,
        const int evt_lumi,
        const int evt_event
    )
    {
        using namespace std;

        // test chain
        if (!chain)
        {
            throw std::invalid_argument("at::ScanChain: chain is NULL!");
        }
        if (chain->GetListOfFiles()->GetEntries()<1)
        {
            throw std::invalid_argument("at::ScanChain: chain has no files!");
        }
        if (not chain->GetFile())
        {
            throw std::invalid_argument("at::ScanChain: chain has no files or file path is invalid!");
        }

        if (verbose) {rt::PrintFilesFromTChain(chain);}
        string tree_name = chain->GetName();
    
        // set the "good run" list 
        if (!goodrun_file_name.empty())
        {
            set_goodrun_file(goodrun_file_name.c_str());
            //set_goodrun_file_json(goodrun_file_name.c_str());
        }

        // set the style
        rt::SetStyle("emruoi");
    
        // benchmark
        TBenchmark bmark;
        bmark.Start("benchmark");
    
        // events counts and max events
        int i_permilleOld = 0;
        long num_events_total = 0;
        long num_events_chain = (num_events >= 0 && num_events < chain->GetEntries()) ? num_events : chain->GetEntries();
        TObjArray* list_of_files = chain->GetListOfFiles();
        TIter file_iter(list_of_files);
        TFile* current_file = NULL;

        // count the duplicates and bad events
        unsigned long duplicates = 0;
        unsigned long bad_events = 0;

        // begin job
        analyzer.BeginJob();

        // loop over files in the chain
        while ((current_file = (TFile*)file_iter.Next()))
        {
            TFile *file = TFile::Open(current_file->GetTitle());
            if (!file || file->IsZombie())
            {
                //std::cout << "Skipping bad input file: " << current_file->GetTitle() << std::endl;
                //continue;
                throw std::runtime_error(Form("File from TChain is invalid or corrupt: %s", current_file->GetTitle()));
            }

            // get the trees in each file
            TTree *tree = dynamic_cast<TTree*>(file->Get(tree_name.c_str()));
            if (!tree || tree->IsZombie())
            {
                //std::cout << "Skipping bad input file (missing tree): " << current_file->GetTitle() << std::endl;
                //continue;
                throw std::runtime_error(Form("File from TChain has an invalid TTree or is corrupt: %s", current_file->GetTitle()));
            }

            if (fast)
            {
                TTreeCache::SetLearnEntries(10);
                tree->SetCacheSize(128*1024*1024);
            }
            Init(ntuple_class, tree);

            // Loop over Events in current file
            if (num_events_total >= num_events_chain) continue;
            long num_events_tree = tree->GetEntriesFast();

            // loop over events to Analyze
            for (long event = 0; event != num_events_tree; ++event)
            {
                // quit if the total is > the number in the chain
                if (num_events_total >= num_events_chain) continue;

                // load the entry
                if (fast) tree->LoadTree(event);
                GetEntry(ntuple_class, event);
                ++num_events_total;

                // pogress
                int i_permille = (int)floor(1000 * num_events_total / float(num_events_chain));
                if (i_permille != i_permilleOld) {
                    printf("  \015\033[32m ---> \033[1m\033[31m%4.1f%%" "\033[0m\033[32m <---\033[0m\015", i_permille/10.);
                    fflush(stdout);
                    i_permilleOld = i_permille;
                }

                unsigned int run = Run(ntuple_class);
                unsigned int ls  = LumiBlock(ntuple_class);
                unsigned int evt = Event(ntuple_class);

                // check run/ls/evt
                if (evt_event >= 0)
                {
                    if (evt==static_cast<unsigned int>(evt_event))
                    {
                        if (verbose) {cout << "selected event:\t" << evt << endl;}
                    }
                    else
                    {
                        continue;
                    }
                }
                if (evt_lumi >= 0)
                {
                    if (ls==static_cast<unsigned int>(evt_lumi))
                    {
                        if (verbose) {cout << "selected lumi:\t" << ls << endl;}
                    }
                    else
                    {
                        continue;
                    }
                }
                if (evt_run >= 0)
                {
                    if (ls==static_cast<unsigned int>(evt_run))
                    {
                        if (verbose) {cout << "selected run:\t" << run << endl;}
                    }
                    else
                    {
                        continue;
                    }
                }

                // filter out events
                if (IsRealData(ntuple_class))
                {
                    if (!goodrun_file_name.empty())
                    {
                        // check for good run and events
                        if(!goodrun(run, ls)) 
                        {
                            if (verbose) {cout << "Bad run and lumi:\t" << run << ", " << ls << endl;}
                            bad_events++;
                            continue;
                        }
                    }

                    // check for dupiclate run and events
                    DorkyEventIdentifier id = {run, evt, ls};
                    if (is_duplicate(id))
                    {
                        if (verbose) {cout << "Duplicate event:\t" << run << ", " << ls << ", " << evt << endl;}
                        duplicates++;
                        continue;
                    }
                }

                // print run/ls/event
                if (verbose)
                {
                    //cout << Form("run %d, ls %d, evt %d", run, ls, evt) << endl;
                }

                // analysis
                analyzer.Analyze(event, file->GetName());

            } // end event loop

            // close current file
            file->Close();
            delete file;

        } // end file loop
    
        // print warning if the totals don't line up
        if (num_events_chain != num_events_total) 
        {
            cout << "Error: number of events from the files " 
                << "(" << num_events_chain << ") " 
                << "is not equal to the total number of events "
                << "(" << num_events_total << ")." 
                << endl;
        }
    
        // save the output
        analyzer.EndJob();
    
        // the benchmark results 
        // -------------------------------------------------------------------------------------------------//
        bmark.Stop("benchmark");
        cout << endl;
        cout << num_events_total << " Events Processed" << endl;
        cout << "# of bad events filtered = " << bad_events << endl; 
        cout << "# of duplicates filtered = " << duplicates << endl; 
        cout << "------------------------------" << endl;
        cout << "CPU  Time: " << Form("%.01f", bmark.GetCpuTime("benchmark" )) << endl;
        cout << "Real Time: " << Form("%.01f", bmark.GetRealTime("benchmark")) << endl;
        cout << endl;
    
        // done
        return 0;
    }
} // namespace at

