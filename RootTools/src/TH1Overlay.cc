#include "AnalysisTools/RootTools/interface/TH1Overlay.h"
#include "AnalysisTools/RootTools/interface/TH1Tools.h"
#include "AnalysisTools/RootTools/interface/MiscTools.h"
#include "AnalysisTools/LanguageTools/interface/StringTools.h"

// c++ includes
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <map>

// ROOT includes
#include "TStyle.h"
#include "TProfile.h"
#include "TPad.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "THStack.h"
#include "TLegend.h"
#include "TPaveStats.h"
#include "THistPainter.h"
#include "TLatex.h"

// boost includes
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#define TH1OVERLAY_STATIC_ASSERT_concat_impl(lhs, rhs) lhs ## rhs
#define TH1OVERLAY_STATIC_ASSERT_concat(lhs, rhs) TH1OVERLAY_STATIC_ASSERT_concat_impl(lhs, rhs)
#define TH1OVERLAY_STATIC_ASSERT(expression) typedef char TH1OVERLAY_STATIC_ASSERT_concat(TH1OVERLAY_STATIC_ASSERT_error_, __LINE__) [(expression) ? 1 : -1]

using namespace std;
using namespace boost;

namespace rt
{

    // non members helpers
    // ---------------------------------------------------------------------------------------- //

    std::string UniqueHistName()
    {
        static int static_index = 0;
        return "TH1Overlay_temp_hist_" + lexical_cast<string>(static_index++);
    }


    // simple Rectangle 
    struct Rectangle
    {
        double x1;
        double y1;
        double x2;
        double y2;
    };


    // simple 2D Point 
    struct Point
    {
        double x;
        double y;
    };


    struct HeightCompare
    {
        bool operator () (const TH1 *lhs, const TH1 *rhs) const
        {
            return rhs->GetMaximum() < lhs->GetMaximum();
        }
    };


    struct XMaxCompare
    {
        bool operator () (const TH1 *lhs, const TH1 *rhs) const
        {
            return rhs->GetXaxis()->GetXmax() < lhs->GetXaxis()->GetXmax();
        }
    };


    struct XMinCompare
    {
        bool operator () (const TH1 *lhs, const TH1 *rhs) const
        {
            return rhs->GetXaxis()->GetXmin() > lhs->GetXaxis()->GetXmin();
        }
    };


    static const Rectangle GetCellRectangle(int i, int j, bool is_default = false)
    {
        //upper right corner (short name for clarity in the array);
        static Point c = {1.0 - gStyle->GetPadRightMargin(), 1.0 - gStyle->GetPadTopMargin()};

        //offset (short name for clarity in the array);
        static Point o = {(c.x - gStyle->GetPadLeftMargin())/4.0, (c.y - gStyle->GetPadBottomMargin())/4.0 };

        // default placement if index is out of bounds or is_default == true
        const Rectangle default_placement = {c.x-o.x, c.y-o.y, c.x, c.y};
        if (is_default)
        {
            return default_placement;
        }

        const Rectangle s_cell_array[4][4] =
        {
            { {c.x-4*o.x,c.y-1*o.y,c.x-3*o.x,c.y-0*o.y}, {c.x-3*o.x,c.y-1*o.y,c.x-2*o.x,c.y-0*o.y}, {c.x-2*o.x,c.y-1*o.y,c.x-1*o.x,c.y-0*o.y}, {c.x-1*o.x,c.y-1*o.y,c.x-0*o.x,c.y-0*o.y} },
            { {c.x-4*o.x,c.y-2*o.y,c.x-3*o.x,c.y-1*o.y}, {c.x-3*o.x,c.y-2*o.y,c.x-2*o.x,c.y-1*o.y}, {c.x-2*o.x,c.y-2*o.y,c.x-1*o.x,c.y-1*o.y}, {c.x-1*o.x,c.y-2*o.y,c.x-0*o.x,c.y-1*o.y} },
            { {c.x-4*o.x,c.y-3*o.y,c.x-3*o.x,c.y-2*o.y}, {c.x-3*o.x,c.y-3*o.y,c.x-2*o.x,c.y-2*o.y}, {c.x-2*o.x,c.y-3*o.y,c.x-1*o.x,c.y-2*o.y}, {c.x-1*o.x,c.y-3*o.y,c.x-0*o.x,c.y-2*o.y} },
            { {c.x-4*o.x,c.y-4*o.y,c.x-3*o.x,c.y-3*o.y}, {c.x-3*o.x,c.y-4*o.y,c.x-2*o.x,c.y-3*o.y}, {c.x-2*o.x,c.y-4*o.y,c.x-1*o.x,c.y-3*o.y}, {c.x-1*o.x,c.y-4*o.y,c.x-0*o.x,c.y-3*o.y} }
        };

        if ((i < 4) && (j < 4))
        {
            return s_cell_array[i][j];
            cout << s_cell_array[i][j].x1 << "\t"  << s_cell_array[i][j].y1 << "\t"  << s_cell_array[i][j].x2 << "\t"  << s_cell_array[i][j].y2 << endl;

        }
        else
        {
            return default_placement;
        }
    }


    //static const Rectangle GetDefaultRectangle()
    //{
    //    return GetCellRectangle(0, 0, /*is_default=*/true);
    //}


    static int GetCellLayoutIndex(StatBoxPlacement::value_type placement, int i, int j)
    {
        const int s_cell_array[StatBoxPlacement::static_size][4][4] =
        {
            // top_left
            {
                { 0,  2,  6, 11},
                { 1,  3,  7, 12},
                { 4,  5, 10, 14},
                { 8,  9, 13, 15}
            },
            // top_right
            {
                {11,  6,  1,  0},
                {12,  7,  3,  2},
                {14, 10,  5,  4},
                {15, 13,  9,  8}
            },
            // top_middle
            {
                { 4,  1,   0,   2},
                { 8,  6,   5,   3},
                {12, 10,   9,   7},
                {15, 14,  13,  11}
            },
            // top_middle_left
            {
                {0,  3,  7,  11},
                {1,  5,  9,  12},
                {2,  6, 10,  14},
                {4,  8, 13,  15}
            },
            // top_middle_right
            {
                {11,  7,  3,  0},
                {12,  9,  5,  1},
                {14, 10,  6,  2},
                {15, 13,  8,  4}
            },
            // top
            {
                { 3,  2,  1,  0},
                { 7,  6,  5,  4},
                {11, 10,  9,  8},
                {15, 14, 13, 12}
            },
            // right
            {
                {12,  8,  4,  0},
                {13,  9,  5,  1},
                {14, 10,  6,  2},
                {15, 11,  7,  3}
            },
            // left
            {
                {0,  4,  8, 12},
                {2,  5,  9, 13},
                {3,  6, 10, 14},
                {3,  7, 11, 15}
            },
            // bottom_left
            {
                {6,  9,  11,  15},
                {4,  5,  10,  14},
                {1,  3,   8,  13},
                {0,  2,   7,  12}
            },
            // bottom_right
            {
                {15,  11,  9,  6},
                {14,  10,  5,  4},
                {13,  8,   3,  1},
                {12,  7,   2,  0}
            },
            // bottom_middle
            {
                {15,  12, 13, 14},
                {11,  8,   9, 10},
                { 7,  2,   3,  6},
                { 5,  0,   1,  4}
            },
            // bottom
            {
                {12,  13, 14, 15},
                {8,   9,  10, 11},
                {4,   5,   6,  7},
                {0,   1,   2,  3}
            }
        };
        // this array designates the layout order for each placement option
        // if this fails, you updated the enum without updating this too
        TH1OVERLAY_STATIC_ASSERT(StatBoxPlacement::static_size == 12);

        if ((placement < StatBoxPlacement::static_size) && (i < 4) && (j < 4))
        {
            return s_cell_array[placement][i][j];
        }
        else
        {
            std::cerr << "Invalid layout index\t" << placement << " >= " 
                << StatBoxPlacement::static_size << "\ti = " << i << "\tj =" << j << std::endl;
            return -1;
        }
    }


    static void GetCellRowColumnIndex(int &i, int &j, StatBoxPlacement::value_type placement, int layout_index)
    {
        for(i = 0; i < 4; ++i)
        {
            for(j = 0; j < 4; ++j)
            {
                if (GetCellLayoutIndex(placement, i, j) == layout_index)
                {
                    return;
                }
            }
        }

        i = 0;
        j = 0;
        return;
    }


    //static bool IsStatBoxUsingCell(StatBoxPlacement::value_type StatBoxPlacement, int statbox_count, int i, int j)
    //{
    //    return
    //        (StatBoxPlacement != StatBoxPlacement::Disabled)
    //        ? GetCellLayoutIndex(StatBoxPlacement, i, j) < statbox_count
    //        : false;
    //}


    static const Rectangle GetStatBoxRectangle(StatBoxPlacement::value_type StatBoxPlacement, int statbox_index)
    {
        int i;
        int j;
        GetCellRowColumnIndex(i, j, StatBoxPlacement, statbox_index);
        return GetCellRectangle(i, j);
    }


    // declare data structures
    // ---------------------------------------------------------------------------------------- //

    // histogram attributes structure
    struct HistAttributes
    {
        HistAttributes();
        ~HistAttributes();
        HistAttributes(TH1* h, Color_t c = -1, Width_t w = -1, Style_t s = -1, Style_t f = -1);
        HistAttributes(TH1* h, const std::string& l, Color_t c = -1, Width_t w = -1, Style_t s = -1, Style_t f = -1);
        HistAttributes(TH1* h, bool d, Color_t c = -1, Width_t w = -1, Style_t s = -1, Style_t f = -1);
        HistAttributes(TH1* h, bool d, const std::string& l, Color_t c = -1, Width_t w = -1, Style_t s = -1, Style_t f = -1);

        // member funtions
        void SetAttributes(float min = 1, float max = -1, bool is_stack = false, bool is_norm = false);
        void SetProfileAttributes(Style_t profile_marker_style, float profile_marker_size);

        // data members
        boost::shared_ptr<TH1> hist;
        string legend_value;
        Color_t color;
        Width_t width;
        Style_t style;
        Style_t fill;
        bool nostack;
    };


    HistAttributes::HistAttributes()
        : hist()
          , legend_value("")
          , color(gStyle->GetHistLineColor())
          , width(gStyle->GetHistLineWidth())
          , style(gStyle->GetHistLineStyle())
          , fill (gStyle->GetHistFillStyle())
          , nostack(false)
    {
    }


    HistAttributes::~HistAttributes()
    {
    }


    HistAttributes::HistAttributes(TH1* h, Color_t c, Width_t w, Style_t s, Style_t f)
        : hist(h)
        , legend_value("")
        , color(c != -1 ? c : h->GetLineColor())
        , width(w != -1 ? w : h->GetLineWidth())
        , style(s != -1 ? s : h->GetLineStyle())
        , fill (f != -1 ? f : h->GetFillStyle())
        , nostack(false)
   {
   }


    HistAttributes::HistAttributes(TH1* h, const string& l, Color_t c, Width_t w, Style_t s, Style_t f)
        : hist(h)
        , legend_value(l)
        , color(c != -1 ? c : h->GetLineColor()) 
        , width(w != -1 ? w : h->GetLineWidth())
        , style(s != -1 ? s : h->GetLineStyle())
        , fill (f != -1 ? f : h->GetFillStyle())
        , nostack(false)
    {
    }


    HistAttributes::HistAttributes(TH1* h, bool d, Color_t c, Width_t w, Style_t s, Style_t f)
        : hist(h)
        , legend_value("")
        , color(c != -1 ? c : h->GetLineColor())
        , width(w != -1 ? w : h->GetLineWidth())
        , style(s != -1 ? s : h->GetLineStyle())
        , fill (s != -1 ? f : h->GetFillStyle())
        , nostack(d)
    {
    }


    HistAttributes::HistAttributes(TH1* h, bool d, const string& l, Color_t c, Width_t w, Style_t s, Style_t f)
        : hist(h)
        , legend_value(l)
        , color(c != -1 ? c : h->GetLineColor()) 
        , width(w != -1 ? w : h->GetLineWidth())
        , style(s != -1 ? s : h->GetLineStyle())
        , fill (s != -1 ? f : h->GetFillStyle())
        , nostack(d)
   {
   }


    void HistAttributes::SetAttributes(float min, float max, bool is_stack, bool is_norm)
    {
        if (is_stack && !nostack)
        {
            hist->SetFillColor(color);
            //hist->SetLineColor(color);
            hist->SetLineColor(kBlack);
            hist->SetMarkerColor(color);
            hist->SetMarkerSize(hist->GetMarkerSize());
            //hist->SetBarWidth(hist->GetBarWidth());
            hist->SetLineWidth(1);
            hist->SetOption(hist->GetOption());
            hist->SetDrawOption(hist->GetDrawOption());
            hist->SetFillStyle(1001);  // hard coded until i determine how to do this better
            //hist->SetFillStyle(fill);  // hard coded until i determine how to do this better
        }
        else
        {
            hist->SetFillColor(0);
            hist->SetLineColor(color);
            hist->SetMarkerColor(color);
            hist->SetLineWidth(width);
            hist->SetFillStyle(fill);
            hist->SetMarkerStyle(style);
            //hist->SetBarWidth(hist->GetBarWidth());
            hist->SetOption(hist->GetOption());
            hist->SetDrawOption(hist->GetDrawOption());
            hist->SetMarkerSize(hist->GetMarkerSize());
        }
        if (is_norm)
        {
            rt::Normalize(hist.get(), 1.0);
        }
        if (min <= max)
        {
            hist->SetMinimum(min);
            hist->SetMaximum(max);
        }
    }


    void HistAttributes::SetProfileAttributes(Style_t profile_marker_style, float profile_marker_size)
    {
        if (TProfile* temp_hist  = dynamic_cast<TProfile*>(hist.get()))
        {
            //temp_hist->SetMarkerColor(color);
            temp_hist->SetLineColor(color);
            temp_hist->SetMarkerColor(kBlack);
            temp_hist->SetLineWidth(static_cast<Width_t>(1.0));
            temp_hist->SetMarkerStyle(profile_marker_style);
            temp_hist->SetMarkerSize(profile_marker_size);
            CalcErrorsForProfile(temp_hist);
        }
    }


    // Pointer to implementation structure
    struct TH1Overlay::impl
    {
        // construct
        impl();
        impl
        (
             const string& title,
             StatBoxPlacement::value_type sb_place,
             LegendPlacement::value_type leg_place,
             DrawType::value_type DrawType
        );
        impl(const string& title, const string& option);

        // Overlay data
        string title;
        string option;  // for a presistent drawing option
        rt::StatBoxPlacement::value_type StatBoxPlacement;
        rt::LegendPlacement::value_type LegendPlacement;
        rt::DrawType::value_type DrawType;
        bool logx;
        bool logy;
        float yaxis_min;
        float yaxis_max;
        float xaxis_min;
        float xaxis_max;

        // constants 
        float   legend_width;
        float   legend_height_per_entry;
        float   legend_offset;
        float   legend_text_size;
        int     legend_ncol;
        string  legend_option;
        Color_t statbox_fill_color;
        float   profile_marker_size;
        Style_t profile_marker_style;

        // holds the drawable things
        boost::shared_ptr<THStack> hist_stack;
        vector<HistAttributes> hist_vec;
        boost::shared_ptr<TLegend> legend;
        vector<boost::shared_ptr<TLatex> > text_vector;
        vector<boost::shared_ptr<TLine> > line_vector;

        // handle the colors
        Color_t unique_hist_color(Color_t color);
        bool is_hist_color_used(Color_t color);
    };


    TH1Overlay::impl::impl()
        : title("")
        , option("")
        , StatBoxPlacement(StatBoxPlacement::StatBoxPlacementDefault)
        , LegendPlacement(LegendPlacement::LegendPlacementDefault)
        , DrawType(DrawType::DrawTypeDefault)
        , logx(false)
        , logy(false)
        , yaxis_min(1.0)
        , yaxis_max(-1.0)
        , xaxis_min(1.0)
        , xaxis_max(-1.0)
        , legend_width(TH1Overlay::legend_width_default)
        , legend_height_per_entry(TH1Overlay::legend_height_per_entry_default)
        , legend_offset(TH1Overlay::legend_offset_default)
        , legend_text_size(TH1Overlay::legend_text_size_default)
        , legend_ncol(TH1Overlay::legend_ncol_default)
        , legend_option(TH1Overlay::legend_option_default)
        , statbox_fill_color(TH1Overlay::statbox_fill_color_default)
        , profile_marker_size(TH1Overlay::profile_marker_size_default)
        , profile_marker_style(TH1Overlay::profile_marker_style_default)
        , hist_stack(new THStack) 
        , legend(new TLegend) 
    {
    }


    TH1Overlay::impl::impl
    (
         const string& title,
         StatBoxPlacement::value_type sb_place,
         LegendPlacement::value_type leg_place,
         DrawType::value_type DrawType
    )
        : title(title)
        , option("")
        , StatBoxPlacement(sb_place)
        , LegendPlacement(leg_place)
        , DrawType(DrawType)
        , logx(false)
        , logy(false)
        , yaxis_min(1.0)
        , yaxis_max(-1.0) 
        , xaxis_min(1.0)
        , xaxis_max(-1.0) 
        , legend_width(TH1Overlay::legend_width_default)
        , legend_height_per_entry(TH1Overlay::legend_height_per_entry_default)
        , legend_offset(TH1Overlay::legend_offset_default)
        , legend_text_size(TH1Overlay::legend_text_size_default)
        , legend_ncol(TH1Overlay::legend_ncol_default)
        , legend_option(TH1Overlay::legend_option_default)
        , statbox_fill_color(TH1Overlay::statbox_fill_color_default)
        , profile_marker_size(TH1Overlay::profile_marker_size_default)
        , profile_marker_style(TH1Overlay::profile_marker_style_default)
        , hist_stack(new THStack)
        , legend(new TLegend) 
    {
    }


    TH1Overlay::impl::impl(const string& title, const string& option)
        : title(title)
        , option(option)
        , StatBoxPlacement(StatBoxPlacement::StatBoxPlacementDefault)
        , LegendPlacement(LegendPlacement::LegendPlacementDefault)
        , DrawType(DrawType::DrawTypeDefault)
        , logx(false)
        , logy(false)
        , yaxis_min(1.0)
        , yaxis_max(-1.0) 
        , xaxis_min(1.0)
        , xaxis_max(-1.0) 
        , legend_width(TH1Overlay::legend_width_default)
        , legend_height_per_entry(TH1Overlay::legend_height_per_entry_default)
        , legend_offset(TH1Overlay::legend_offset_default)
        , legend_text_size(TH1Overlay::legend_text_size_default)
        , legend_ncol(TH1Overlay::legend_ncol_default)
        , legend_option(TH1Overlay::legend_option_default)
        , statbox_fill_color(TH1Overlay::statbox_fill_color_default)
        , profile_marker_size(TH1Overlay::profile_marker_size_default)
        , profile_marker_style(TH1Overlay::profile_marker_style_default)
        , hist_stack(new THStack)
        , legend(new TLegend) 
    {
    }


    Color_t TH1Overlay::impl::unique_hist_color(Color_t color)
    {
        static std::vector<Color_t> color_array;

        if (color_array.empty())
        {
#if ROOT_VERSION_CODE <= ROOT_VERSION(5,15,0)
            color_array.push_back(4);
            color_array.push_back(2);
            color_array.push_back(1);
            color_array.push_back(8);
            color_array.push_back(46);
            color_array.push_back(28);
#else
            color_array.push_back(kBlue);
            color_array.push_back(kRed);
            color_array.push_back(kBlack);
            color_array.push_back(kGreen+2);
            color_array.push_back(kOrange+2);
            color_array.push_back(kMagenta+3);
            color_array.push_back(kCyan+2);
            color_array.push_back(kGray+1);
            color_array.push_back(kBlue-6);
            color_array.push_back(kYellow+3);
            color_array.push_back(kGray+3);
            color_array.push_back(kRed-9);
            color_array.push_back(kGreen-9);
#endif
        }

        for (std::size_t i = 0, j = color_array.size(); i < j; ++i)
        {
            if (!this->is_hist_color_used(color_array[i]))
            {
                return color_array[i];
            }
        }
        std::cout << "TH1Overlay::unique_hist_color(): Ran out of unique colors -- returning black." << std::endl;
        return kBlack;
    }


    bool TH1Overlay::impl::is_hist_color_used(Color_t color)
    {
        for (vector<HistAttributes>::const_iterator iter = hist_vec.begin(); iter != hist_vec.end(); iter++)
        {
            if (color == iter->color)
            {
                return true;
                break;
            }
        }
        return false;
    }


    // static public constants
    // ---------------------------------------------------------------------------------------- //
    float       TH1Overlay::legend_width_default            = 0.295;
    float       TH1Overlay::legend_height_per_entry_default = 0.065;
    float       TH1Overlay::legend_offset_default           = 0.020;
    float       TH1Overlay::legend_text_size_default        = 0.030;
    int         TH1Overlay::legend_ncol_default             = 1;
    const char* TH1Overlay::legend_option_default           = "LEP";
    Color_t     TH1Overlay::statbox_fill_color_default      = gStyle->GetFrameFillColor();
    float       TH1Overlay::profile_marker_size_default     = 0.8;
    Style_t     TH1Overlay::profile_marker_style_default    = 20;


    // constructors
    // ---------------------------------------------------------------------------------------- //

    TH1Overlay::TH1Overlay()
        : m_pimpl(new TH1Overlay::impl)
    {
    }


    TH1Overlay::~TH1Overlay()
    {
    }


    TH1Overlay::TH1Overlay(const TH1Overlay& rhs)
        : m_pimpl(new TH1Overlay::impl())
          //    : m_pimpl(new TH1Overlay::impl(rhs.GetTitle(), 
          //                                   rhs.GetStatBoxPlacement(),
          //                                   rhs.GetLegendPlacement(),
          //                                   rhs.GetDrawType())) 
    {
        // copy everything except map (should probably make an copy constructor for impl)
        SetTitle(rhs.GetTitle());
        SetOption(rhs.GetOption());
        SetStatBoxPlacement(rhs.GetStatBoxPlacement());
        SetLegendPlacement(rhs.GetLegendPlacement());
        SetDrawType(rhs.GetDrawType());
        SetLogx(rhs.GetLogx());
        SetLogy(rhs.GetLogy());
        SetLegendWidth(rhs.GetLegendWidth());
        SetLegendHeightPerEntry(rhs.GetLegendHeightPerEntry());
        SetLegendOffset(rhs.GetLegendOffset());
        SetLegendTextSize(rhs.GetLegendTextSize());
        SetLegendOption(rhs.GetLegendOption());
        SetStatBoxFillColor(rhs.GetStatBoxFillColor());
        SetYAxisRange(rhs.GetYAxisMin(), rhs.GetYAxisMax());
        //Setprofile_marker_size(rhs.Getprofile_marker());
        //Setprofile_marker_style(rhs.Getprofile_marker_style());

        // copy the map
        for 
        (
             vector<HistAttributes>::const_iterator iter = rhs.m_pimpl->hist_vec.begin();
             iter != rhs.m_pimpl->hist_vec.end(); 
             iter++
        )
        {
            const HistAttributes& ha = *iter;
            Add(ha.hist.get(), ha.nostack, ha.legend_value, ha.color, ha.width, ha.style, ha.fill);
        }

        // copy the text
        for (size_t i = 0; i != rhs.m_pimpl->text_vector.size(); i++)
        {
            AddText(rhs.m_pimpl->text_vector.at(i).get());
        }

        // copy the lines
        for (size_t i = 0; i != rhs.m_pimpl->line_vector.size(); i++)
        {
            AddLine(rhs.m_pimpl->line_vector.at(i).get());
        }

        return;
    }


    void TH1Overlay::Swap(TH1Overlay& other)
    {
        std::swap(m_pimpl, other.m_pimpl);
        return;
    }


    TH1Overlay& TH1Overlay::operator=(const TH1Overlay& rhs)
    {
        TH1Overlay temp(rhs);
        Swap(temp);
        return *this;
    }


    TH1Overlay::TH1Overlay
    (
         const string& title,
         StatBoxPlacement::value_type sb_place,
         LegendPlacement::value_type leg_place,
         DrawType::value_type DrawType
    )
        : m_pimpl(new TH1Overlay::impl(title, sb_place, leg_place, DrawType))
    {
    }

    TH1Overlay::TH1Overlay(const string& title, const string& option)
        : m_pimpl(new TH1Overlay::impl(title, option))
    {
    }


    // members 
    // ---------------------------------------------------------------------------------------- //

    void TH1Overlay::Add(TH1* h, bool no_stack, const string& legend_value, Color_t c, Width_t w, Style_t s, Style_t f)
    {
        if (h)
        {
            TH1* temp_hist = dynamic_cast<TH1*>(h->Clone());
            temp_hist->SetDirectory(0);
            Color_t color = c != -1 ? c : m_pimpl->unique_hist_color(h->GetLineColor());
            m_pimpl->hist_vec.push_back(HistAttributes(temp_hist, no_stack, legend_value, color, w, s, f));
        }
        else
        {
            cerr << "rt::TH1Overlay::Add(): histogram is NULL, not adding to plot." << endl;
        }
    }


    void TH1Overlay::Add(TH1* h, bool no_stack, const char* legend_value, Color_t c, Width_t w, Style_t s, Style_t f)
    {
        Add(h, no_stack, std::string(legend_value), c, w, s, f);
    }


    void TH1Overlay::Add(TH1* h, bool no_stack, Color_t c, Width_t w, Style_t s, Style_t f)
    {
        Add(h, no_stack, h->GetTitle(), c, w, s, f);
    }


    void TH1Overlay::Add(TH1* h, const string& legend_value, Color_t c, Width_t w, Style_t s, Style_t f)
    {
        Add(h, false, legend_value, c, w, s, f);
    }


    void TH1Overlay::Add(TH1* h, const char* legend_value, Color_t c, Width_t w, Style_t s, Style_t f)
    {
        Add(h, false, std::string(legend_value), c, w, s, f);
    }


    void TH1Overlay::Add(TH1* h, Color_t c, Width_t w, Style_t s, Style_t f)
    {
        Add(h, false, h->GetTitle(), c, w, s, f);
    }

    void TH1Overlay::AddText(const TLatex& text)
    {
        m_pimpl->text_vector.push_back(boost::shared_ptr<TLatex>(dynamic_cast<TLatex*>(text.Clone())));
        m_pimpl->text_vector.back()->SetNDC(); 
    }

    void TH1Overlay::AddText(const TLatex* text)
    {
        m_pimpl->text_vector.push_back(boost::shared_ptr<TLatex>(dynamic_cast<TLatex*>(text->Clone())));
        m_pimpl->text_vector.back()->SetNDC(); 
    }

    void TH1Overlay::AddText(const std::string& text, float x, float y, float text_size)
    {
        m_pimpl->text_vector.push_back(boost::shared_ptr<TLatex>(new TLatex(x, y, text.c_str())));
        m_pimpl->text_vector.back()->SetNDC(); 
        m_pimpl->text_vector.back()->SetTextSize(text_size); 
    }

    void TH1Overlay::AddLine(const TLine& line)
    {
        m_pimpl->line_vector.push_back(boost::shared_ptr<TLine>(dynamic_cast<TLine*>(line.Clone())));
    }

    void TH1Overlay::AddLine(const TLine* line)
    {
        m_pimpl->line_vector.push_back(boost::shared_ptr<TLine>(dynamic_cast<TLine*>(line->Clone())));
    }

    void TH1Overlay::AddLine(float x1, float y1, float x2, float y2, Color_t c)
    {
        m_pimpl->line_vector.push_back(boost::shared_ptr<TLine>(new TLine(x1, y1, x2, y2)));
        m_pimpl->line_vector.back()->SetLineColor(c);
    }

    struct CompareHistAttributesByName 
    {
        CompareHistAttributesByName(const std::string& hist_name) : m_name(hist_name) {}
        bool operator () (const HistAttributes& ha) const 
        {
            return std::string(ha.hist->GetName()) == m_name;
        }
        const std::string& m_name;
    };


    void TH1Overlay::Remove(const string& hist_name)
    {
        vector<HistAttributes>::iterator hist_to_remove = std::find_if(m_pimpl->hist_vec.begin(), m_pimpl->hist_vec.end(), CompareHistAttributesByName(hist_name)); 
        if (hist_to_remove != m_pimpl->hist_vec.end())
            m_pimpl->hist_vec.erase(hist_to_remove);
    }


    void TH1Overlay::Remove(const char* hist_name)
    {
        Remove(static_cast<string>(hist_name));
    }


    void TH1Overlay::Remove(const TString& hist_name)
    {
        Remove(static_cast<string>(hist_name.Data()));
    }


    void TH1Overlay::Remove(TH1* hist_ptr)
    {
        Remove(string(hist_ptr->GetName()));
    }


    void TH1Overlay::List() const
    {
        cout << "TH1Overlay::List(): listing all histograms in the overlay" << endl;
        cout <<  "  " << setw(15) << left << "Hist Type" << "\t" << setw(15) << left 
            << "Hist Name" << "\t" << "Hist Title" << endl; 
        cout <<  "-----------------------------------------------------------------------" << endl; 
        for (vector<HistAttributes>::const_iterator iter = m_pimpl->hist_vec.begin(); iter != m_pimpl->hist_vec.end(); iter++)
        {
            const boost::shared_ptr<TH1>& hist = iter->hist;
            cout <<  "  " << setw(15) << left << hist->ClassName() << "\t" << setw(15) << left << hist->GetName() << "\t" << hist->GetTitle() << endl;
        }
    }


    // set the THStack
    void TH1Overlay::BuildStack()
    {
        const bool is_norm  = (m_pimpl->DrawType == DrawType::normalize);
        const bool is_stack = (m_pimpl->DrawType == DrawType::stack or m_pimpl->DrawType == DrawType::stack_norm);

        // clear the old plots
        if (m_pimpl->hist_stack->GetHists() && !m_pimpl->hist_stack->GetHists()->IsEmpty())
        {
            m_pimpl->hist_stack->GetHists()->Clear();
        }

        // set the THSTack title
        if (Empty())
        {
            m_pimpl->hist_stack->SetTitle(m_pimpl->title != "" ? m_pimpl->title.c_str() : "");
        }
        else
        {
            if (m_pimpl->title != "")
            {
                if (m_pimpl->title.find(";") == string::npos)
                {
                    boost::shared_ptr<TH1>& temp_hist = m_pimpl->hist_vec.begin()->hist;
                    if (temp_hist->GetXaxis()) m_pimpl->title += string(";") + temp_hist->GetXaxis()->GetTitle();
                    if (temp_hist->GetYaxis()) m_pimpl->title += string(";") + temp_hist->GetYaxis()->GetTitle();
                }
                m_pimpl->hist_stack->SetTitle(m_pimpl->title.c_str());
            }
            else
            {
                boost::shared_ptr<TH1>& temp_hist = m_pimpl->hist_vec.begin()->hist;
                string temp_string = temp_hist->GetTitle();
                if (temp_hist->GetXaxis()) temp_string += string(";") + temp_hist->GetXaxis()->GetTitle();
                if (temp_hist->GetYaxis()) temp_string += string(";") + temp_hist->GetYaxis()->GetTitle();
                m_pimpl->hist_stack->SetTitle(temp_string.c_str());
            }
        }

        // determine the stack overall normalization
        float stack_total = 0.0;
        for (vector<HistAttributes>::iterator iter = m_pimpl->hist_vec.begin(); iter != m_pimpl->hist_vec.end(); ++iter)
        {
            // only count hist that are in the stack
            if (iter->nostack) {continue;}
            stack_total += iter->hist->Integral(); 
        }

        // Fill the THSsack
        double max_height = 0.0;
        double min_height = 0.0;
        size_t hist_index = 0;
        for (vector<HistAttributes>::reverse_iterator iter = m_pimpl->hist_vec.rbegin(); iter != m_pimpl->hist_vec.rend(); ++iter)
        {
            if (!iter->hist)
            {
                cerr << "TH1Overlay::BuildStack(): Trying to add NULL histogram.  Spipping!" << endl;
                continue;
            }

            // set the attributes 
            iter->SetAttributes(m_pimpl->yaxis_min, m_pimpl->yaxis_max, is_stack, is_norm);

            // special treament for profile hists
            iter->SetProfileAttributes(m_pimpl->profile_marker_style, m_pimpl->profile_marker_size);

            if (is_stack and !iter->nostack and m_pimpl->DrawType == DrawType::stack_norm)
            {
                rt::Normalize(iter->hist.get(), 1.0/stack_total);
            }

            // get the maximum value
            max_height = std::max(max_height, GetHistMaximumValue(iter->hist.get(), /*include_error=*/true));
            min_height = std::min(min_height, GetHistMinimumValue(iter->hist.get(), /*include_error=*/true));

            // Fill the stack 
            if (is_stack)
            {
                if (!iter->nostack)
                {
                    m_pimpl->hist_stack->Add(iter->hist.get(), (m_pimpl->option + "hist" + iter->hist->GetDrawOption()).c_str());
                }
            }
            else
            {
                m_pimpl->hist_stack->Add(iter->hist.get(), m_pimpl->option.c_str());
            }
            hist_index++;
        }
        // shouldn't need to do this -- THStack is behaving weird by 
        // reporting an Integral of 2 instead of 1
        if (is_norm)
        {
            for (int hist_index = 0; hist_index < m_pimpl->hist_stack->GetStack()->GetEntries(); hist_index++)
            {
                TH1 *hist_ptr = dynamic_cast<TH1*>(m_pimpl->hist_stack->GetStack()->At(hist_index));
                rt::Normalize(hist_ptr, 1.0);
            }
        }

        // this is a kludge to get the annoying THStack's blank histogram from showing up
        m_pimpl->hist_stack->Draw("goff");
        if (m_pimpl->hist_stack->GetHistogram())
        {
            m_pimpl->hist_stack->GetHistogram()->SetLineColor(gStyle->GetFrameLineColor());
            m_pimpl->hist_stack->GetHistogram()->SetLineWidth(gStyle->GetFrameLineWidth());
            m_pimpl->hist_stack->GetHistogram()->SetLineStyle(gStyle->GetFrameLineStyle());
        }
        
        // set the y axis boundaries
        if (m_pimpl->hist_stack->GetHists() && !m_pimpl->hist_stack->GetHists()->IsEmpty())
        {
            m_pimpl->hist_stack->GetHists()->SetOwner(false);
            for (int hist_index = 0; hist_index < m_pimpl->hist_stack->GetStack()->GetEntries(); hist_index++)
            {
                TH1 *hist_ptr = dynamic_cast<TH1*>(m_pimpl->hist_stack->GetStack()->At(hist_index));
                if (m_pimpl->yaxis_min <= m_pimpl->yaxis_max)
                {
                    hist_ptr->GetYaxis()->SetRangeUser(m_pimpl->yaxis_min, m_pimpl->yaxis_max);
                }
                else
                {
                    hist_ptr->GetYaxis()->SetRangeUser(min_height, max_height*1.4);
                }
            }
        }

        return;
    }


    // buld the Legend
    void TH1Overlay::BuildLegend()
    {
        Rectangle pos = { 0.1, 0.1, 0.6, 0.9 };
        float height = m_pimpl->hist_vec.size() * m_pimpl->legend_height_per_entry;

        switch (m_pimpl->LegendPlacement)
        {
            case LegendPlacement::top: 
                pos.x1 = 0.5 - m_pimpl->legend_width/2.0 + m_pimpl->legend_offset*2.5;
                pos.y1 = (1.0 - gStyle->GetPadTopMargin()) - height;
                pos.x2 = 0.5 + m_pimpl->legend_width/2.0 + m_pimpl->legend_offset*2.5;
                pos.y2 = (1.0 - gStyle->GetPadTopMargin() - m_pimpl->legend_offset);
                break;
            case LegendPlacement::top_left: 
                pos.x1 = gStyle->GetPadLeftMargin() + m_pimpl->legend_offset;
                pos.y1 = (1.0 - gStyle->GetPadTopMargin()) - height;
                pos.x2 = gStyle->GetPadLeftMargin() + m_pimpl->legend_width;
                pos.y2 = (1.0 - gStyle->GetPadTopMargin() - m_pimpl->legend_offset);
                break;
            case LegendPlacement::top_right:
                pos.x1 = (1.0 - gStyle->GetPadLeftMargin()) + m_pimpl->legend_offset*2.0 - m_pimpl->legend_width;
                pos.y1 = (1.0 - gStyle->GetPadTopMargin()) - height;
                pos.x2 = (1.0 - gStyle->GetPadRightMargin()) + m_pimpl->legend_offset*2.0;
                pos.y2 = (1.0 - gStyle->GetPadTopMargin() - m_pimpl->legend_offset*2.0);
                break;
            case LegendPlacement::right:
                pos.x1 = (1.0 - gStyle->GetPadLeftMargin()) + m_pimpl->legend_offset*5.0 - m_pimpl->legend_width;
                pos.y1 = 0.8 - gStyle->GetPadTopMargin() - height;
                pos.x2 = (1.0 - gStyle->GetPadRightMargin()) + m_pimpl->legend_offset*5.0;
                pos.y2 = 0.8 - gStyle->GetPadTopMargin() - m_pimpl->legend_offset;
                break;
            case LegendPlacement::left:
                pos.x1 = gStyle->GetPadLeftMargin() + m_pimpl->legend_offset;
                pos.y1 = 0.9 - gStyle->GetPadTopMargin() - height;
                pos.x2 = gStyle->GetPadLeftMargin() + m_pimpl->legend_width;
                pos.y2 = 0.9 - gStyle->GetPadTopMargin() - m_pimpl->legend_offset;
                break;
            case LegendPlacement::top_middle_right:
                pos.x1 = 0.5 - m_pimpl->legend_width/2.0 + m_pimpl->legend_offset*2.5;
                pos.y1 = 0.8 - gStyle->GetPadTopMargin() - height;
                pos.x2 = 0.5 + m_pimpl->legend_width/2.0 + m_pimpl->legend_offset*2.5;
                pos.y2 = 0.8 - gStyle->GetPadTopMargin() - m_pimpl->legend_offset;
                break;
            case LegendPlacement::bottom_left:
                pos.x1 = gStyle->GetPadLeftMargin() + m_pimpl->legend_offset;
                pos.y1 = gStyle->GetPadBottomMargin() + m_pimpl->legend_offset;
                pos.x2 = gStyle->GetPadLeftMargin() + m_pimpl->legend_width;
                pos.y2 = gStyle->GetPadBottomMargin() + m_pimpl->legend_offset + height;
                break;
            case LegendPlacement::bottom_right:
                pos.x1 = (1.0 - gStyle->GetPadLeftMargin()) - m_pimpl->legend_offset - m_pimpl->legend_width;
                pos.y1 = gStyle->GetPadBottomMargin() + m_pimpl->legend_offset;
                pos.x2 = (1.0 - gStyle->GetPadRightMargin()) - m_pimpl->legend_offset;
                pos.y2 = gStyle->GetPadBottomMargin() + m_pimpl->legend_offset + height;
                break;
            case LegendPlacement::bottom:
                pos.x1 = 0.5 - m_pimpl->legend_width/2.0 + m_pimpl->legend_offset*2.5;
                pos.y1 = gStyle->GetPadBottomMargin() + m_pimpl->legend_offset;
                pos.x2 = 0.5 + m_pimpl->legend_width/2.0 + m_pimpl->legend_offset*2.5;
                pos.y2 = gStyle->GetPadBottomMargin() + m_pimpl->legend_offset + height;
                break;
            case LegendPlacement::disabled:
                pos.x1 = 0.0;
                pos.y1 = 0.0;
                pos.x2 = 0.0;
                pos.y2 = 0.0;
                break;
            default:
                break;
        };

        m_pimpl->legend->SetX1NDC(pos.x1);
        m_pimpl->legend->SetY1NDC(pos.y1);
        m_pimpl->legend->SetX2NDC(pos.x2);
        m_pimpl->legend->SetY2NDC(pos.y2);

        m_pimpl->legend->Clear();
        m_pimpl->legend->SetFillColor(0);  // 0 makes it the background clear on the pad
        m_pimpl->legend->SetFillStyle(0);
        m_pimpl->legend->SetBorderSize(0);
        m_pimpl->legend->SetTextSize(m_pimpl->legend_text_size);
        m_pimpl->legend->SetNColumns(m_pimpl->legend_ncol);

        for (vector<HistAttributes>::const_iterator iter = m_pimpl->hist_vec.begin(); iter != m_pimpl->hist_vec.end(); iter++)
        {
            const HistAttributes& hist_att = *iter;
            if ((m_pimpl->DrawType==DrawType::stack or m_pimpl->DrawType==DrawType::stack_norm) && hist_att.nostack)
            {
                if (hist_att.fill>3000)  // hack for the pred fill 
                {
                    m_pimpl->legend->AddEntry(hist_att.hist.get(), hist_att.legend_value.c_str(), "f"); 
                }
                else 
                {
                    m_pimpl->legend->AddEntry(hist_att.hist.get(), hist_att.legend_value.c_str(), "lep"); 
                }
            }
            else if ((m_pimpl->DrawType==DrawType::stack or m_pimpl->DrawType==DrawType::stack_norm) && not hist_att.nostack)
            {
                m_pimpl->legend->AddEntry(hist_att.hist.get(), hist_att.legend_value.c_str(), "f"); 
            }
            else 
            {
                m_pimpl->legend->AddEntry(hist_att.hist.get(), hist_att.legend_value.c_str(), m_pimpl->legend_option.c_str()); 
            }
        } 
    }

    // draw the texts
    void TH1Overlay::DrawText()
    {
        for (size_t i = 0; i != m_pimpl->text_vector.size(); i++)
        {
            m_pimpl->text_vector.at(i)->Draw();
        }
    }


    // draw the lines
    void TH1Overlay::DrawLines()
    {
        for (size_t i = 0; i != m_pimpl->line_vector.size(); i++)
        {
            m_pimpl->line_vector.at(i)->Draw();
        }
    }


    // draw the Legend
    void TH1Overlay::DrawLegend() const
    {
        if (!m_pimpl->hist_vec.empty() && m_pimpl->LegendPlacement != LegendPlacement::disabled)
        {
            m_pimpl->legend->Draw();
        }
    }


    // set the log values
    void TH1Overlay::SetLog()
    {
        if (m_pimpl->logy)
        {
            double min = (GetYAxisMin() < 0.0001) ?  0.0001 : GetYAxisMin();
            double max = GetYAxisMax();
            SetYAxisRange(min, max);
        }
        gPad->SetLogy(m_pimpl->logy);
        gPad->SetLogx(m_pimpl->logx);
    }


    void TH1Overlay::DrawStatBoxes()
    {
        size_t hist_index = 0;
        for 
            (
             vector<HistAttributes>::const_iterator iter = m_pimpl->hist_vec.begin();
             iter != m_pimpl->hist_vec.end(); 
             iter++
            )
            {
                const boost::shared_ptr<TH1>& hist_ptr = iter->hist;

                // check if disabled
                if(m_pimpl->StatBoxPlacement == StatBoxPlacement::disabled)
                {
                    hist_ptr->SetStats(false);
                }
                else
                {
                    hist_ptr->SetStats(true);
                    hist_ptr->GetPainter()->PaintStat(gStyle->GetOptStat(),0);

                    if(TPaveStats *statbox = dynamic_cast<TPaveStats*>(hist_ptr->FindObject("stats")))
                    {
                        statbox->SetFillColor(m_pimpl->statbox_fill_color);
                        statbox->SetLineColor(iter->color);
                        statbox->SetLineWidth(iter->width);
                        statbox->SetLineStyle(iter->style);

                        const Rectangle r = GetStatBoxRectangle(m_pimpl->StatBoxPlacement, hist_index);
                        statbox->SetX1NDC(r.x1);
                        statbox->SetY1NDC(r.y1);
                        statbox->SetX2NDC(r.x2);
                        statbox->SetY2NDC(r.y2);
                        statbox->Draw("same");
                    }
                }
                hist_index++;
            }
    }


    // draw the list of non stacked hists
    void TH1Overlay::DrawNonStackedHists(const string& option)
    {
        double max_height = m_pimpl->hist_stack ? m_pimpl->hist_stack->GetMaximum() : 0.0;
        double min_height = m_pimpl->hist_stack ? m_pimpl->hist_stack->GetMinimum() : 0.0;
        vector< boost::shared_ptr<TH1> > hists_to_draw;
        for (vector<HistAttributes>::const_iterator iter = m_pimpl->hist_vec.begin();iter != m_pimpl->hist_vec.end();iter++)
        {
            if ((m_pimpl->DrawType==DrawType::stack or m_pimpl->DrawType==DrawType::stack_norm) && iter->nostack)
            {
                if (m_pimpl->DrawType==DrawType::stack_norm)
                {
                    rt::Normalize(iter->hist.get(), 1.0);
                }
                max_height = std::max(max_height, GetHistMaximumValue(iter->hist.get(), /*include_error=*/true));
                min_height = std::min(min_height, GetHistMinimumValue(iter->hist.get(), /*include_error=*/true));
                hists_to_draw.push_back(iter->hist);
            }
        }
        const bool stack_is_full = (m_pimpl->hist_stack->GetHists() && !m_pimpl->hist_stack->GetHists()->IsEmpty());
        if (m_pimpl->hist_stack->GetHists() && !m_pimpl->hist_stack->GetHists()->IsEmpty())
        {
            m_pimpl->hist_stack->SetMinimum(min_height);
            m_pimpl->hist_stack->SetMaximum(max_height*1.4);
        }
        for (size_t i = 0; i != hists_to_draw.size(); ++i)
        {
            std::string temp_option;
            if (hists_to_draw[i]->GetFillStyle()>3000) // hack for the pred fill overlay
            {
                hists_to_draw[i]->SetFillColor(kBlack);
                hists_to_draw[i]->SetLineWidth(1);
                temp_option += "E2";
            }
            if (stack_is_full)
            {
                temp_option += " same ";
            }
            temp_option += option + string(hists_to_draw[i]->GetOption()) + string(hists_to_draw[i]->GetDrawOption());
            hists_to_draw[i]->Draw(temp_option.c_str());  
        }
    }


    string TH1Overlay::ProcessOptionString(const std::string& option)
    {
        // start with the original string
        string filtered_option(option);

        // remove whitepsace
        filtered_option.erase(std::remove_if(filtered_option.begin(), filtered_option.end(), lt::is_space), filtered_option.end());

        // draw type 
        static vector< pair<DrawType::value_type, string> > DrawType_options;
        if (DrawType_options.empty())
        {
            // the order matters because it does a search on the string.
            // If the string is subset of another string then it should be before.
            // (example: "dt::stack" is subset of "dt::stack_norm")
            DrawType_options.push_back(pair<DrawType::value_type, string>(DrawType::none      , "dt::none"      ));
            DrawType_options.push_back(pair<DrawType::value_type, string>(DrawType::stack_norm, "dt::stack_norm"));
            DrawType_options.push_back(pair<DrawType::value_type, string>(DrawType::stack     , "dt::stack"     ));
            DrawType_options.push_back(pair<DrawType::value_type, string>(DrawType::normalize , "dt::norm"      ));
        }
        for (size_t i = 0; i < DrawType_options.size(); i++) 
        {
            const pair<DrawType::value_type, string>& opt = DrawType_options[i];   
            if (lt::string_contains(filtered_option, opt.second))
            {
                SetDrawType(opt.first);
                filtered_option = lt::string_remove_all(filtered_option, opt.second);
            }
        }

        // legend placement
        static vector< pair<LegendPlacement::value_type, string> > legend_type_options;
        if (legend_type_options.empty())
        {
            legend_type_options.push_back(pair<LegendPlacement::value_type, string>(LegendPlacement::top_left        , "lg::top_left"        ));
            legend_type_options.push_back(pair<LegendPlacement::value_type, string>(LegendPlacement::top_right       , "lg::top_right"       ));
            legend_type_options.push_back(pair<LegendPlacement::value_type, string>(LegendPlacement::top_middle_right, "lg::top_middle_right"));
            legend_type_options.push_back(pair<LegendPlacement::value_type, string>(LegendPlacement::top             , "lg::top"             ));
            legend_type_options.push_back(pair<LegendPlacement::value_type, string>(LegendPlacement::right           , "lg::right"           ));
            legend_type_options.push_back(pair<LegendPlacement::value_type, string>(LegendPlacement::left            , "lg::left"            ));
            legend_type_options.push_back(pair<LegendPlacement::value_type, string>(LegendPlacement::bottom_left     , "lg::bottom_left"     ));
            legend_type_options.push_back(pair<LegendPlacement::value_type, string>(LegendPlacement::bottom_right    , "lg::bottom_right"    ));
            legend_type_options.push_back(pair<LegendPlacement::value_type, string>(LegendPlacement::bottom          , "lg::bottom"          ));
            legend_type_options.push_back(pair<LegendPlacement::value_type, string>(LegendPlacement::disabled        , "lg::off"             ));
        }
        for (size_t i = 0; i < legend_type_options.size(); i++) 
        {
            const pair<LegendPlacement::value_type, string>& opt = legend_type_options[i];   
            if (lt::string_contains(filtered_option, opt.second))
            {
                SetLegendPlacement(opt.first);
                filtered_option = lt::string_remove_all(filtered_option, opt.second);
            }
        }

        // statbox placement
        static vector< pair<StatBoxPlacement::value_type, string> > statbox_type_options;
        if (statbox_type_options.empty())
        {
            statbox_type_options.push_back(pair<StatBoxPlacement::value_type, string>(StatBoxPlacement::top_left         , "sb::top_left"        ));
            statbox_type_options.push_back(pair<StatBoxPlacement::value_type, string>(StatBoxPlacement::top_right        , "sb::top_right"       ));
            statbox_type_options.push_back(pair<StatBoxPlacement::value_type, string>(StatBoxPlacement::top_middle       , "sb::top_middle"      ));
            statbox_type_options.push_back(pair<StatBoxPlacement::value_type, string>(StatBoxPlacement::top_middle_left  , "sb::top_middle_left" ));
            statbox_type_options.push_back(pair<StatBoxPlacement::value_type, string>(StatBoxPlacement::top_middle_right , "sb::top_middle_right"));
            statbox_type_options.push_back(pair<StatBoxPlacement::value_type, string>(StatBoxPlacement::top              , "sb::top"             ));
            statbox_type_options.push_back(pair<StatBoxPlacement::value_type, string>(StatBoxPlacement::right            , "sb::right"           ));
            statbox_type_options.push_back(pair<StatBoxPlacement::value_type, string>(StatBoxPlacement::left             , "sb::left"            ));
            statbox_type_options.push_back(pair<StatBoxPlacement::value_type, string>(StatBoxPlacement::bottom_left      , "sb::bottom_left"     ));
            statbox_type_options.push_back(pair<StatBoxPlacement::value_type, string>(StatBoxPlacement::bottom_right     , "sb::bottom_right"    ));
            statbox_type_options.push_back(pair<StatBoxPlacement::value_type, string>(StatBoxPlacement::bottom_middle    , "sb::bottom_middle"   ));
            statbox_type_options.push_back(pair<StatBoxPlacement::value_type, string>(StatBoxPlacement::bottom           , "sb::bottom"          ));
            statbox_type_options.push_back(pair<StatBoxPlacement::value_type, string>(StatBoxPlacement::disabled         , "sb::off"             ));
        }
        for (size_t i = 0; i < statbox_type_options.size(); i++) 
        {
            const pair<StatBoxPlacement::value_type, string>& opt = statbox_type_options[i];   
            if (lt::string_contains(filtered_option, opt.second))
            {
                SetStatBoxPlacement(opt.first);
                filtered_option = lt::string_remove_all(filtered_option, opt.second);
            }
        }

        // axis logs
        if (lt::string_contains(filtered_option, "nology"))
        {
            SetLogy(false);
            filtered_option = lt::string_remove_all(filtered_option, "nology");
        }
        if (lt::string_contains(filtered_option, "logy"))
        {
            SetLogy(true);
            filtered_option = lt::string_remove_all(filtered_option, "logy");
        }
        if (lt::string_contains(filtered_option, "nologx"))
        {
            SetLogx(false);
            filtered_option = lt::string_remove_all(filtered_option, "nologx");
        }
        if (lt::string_contains(filtered_option, "logx"))
        {
            SetLogx(true);
            filtered_option = lt::string_remove_all(filtered_option, "logx");
        }

        m_pimpl->option = filtered_option;
        return filtered_option;
    }


    // draw functions
    void TH1Overlay::DrawRegular(const std::string& option)
    {
        BuildLegend();
        BuildStack();
        m_pimpl->hist_stack->Draw(("nostack"+option).c_str());
        SetLog();
        DrawLegend();
        DrawStatBoxes();
        DrawText();
        DrawLines();
    }


    void TH1Overlay::DrawStacked(const std::string& option)
    {
        BuildLegend();
        BuildStack();
        DrawNonStackedHists(option);
        SetLog();
        DrawLegend();
        DrawStatBoxes();
        DrawText();
        DrawLines();
    }


    void TH1Overlay::DrawStackedNormalized(const std::string& option)
    {
        BuildLegend();
        BuildStack();
        DrawNonStackedHists(option);
        SetLog();
        DrawLegend();
        DrawStatBoxes();
        DrawText();
        DrawLines();
    }


    void TH1Overlay::DrawNormalized(const std::string& option)
    {
        BuildLegend();
        BuildStack();
        m_pimpl->hist_stack->Draw(("nostack"+option).c_str());
        SetLog();
        DrawLegend();
        DrawStatBoxes();
        DrawText();
        DrawLines();
    }


    // draw plots
    void TH1Overlay::Draw(const std::string& option)
    {
        if (Empty()) return;
        string processed_option = ProcessOptionString(option + m_pimpl->option);
        switch(m_pimpl->DrawType)
        {
            case     (DrawType::none      ): DrawRegular(processed_option);           break;
            case     (DrawType::stack     ): DrawStacked(processed_option);           break;
            case     (DrawType::stack_norm): DrawStackedNormalized(processed_option); break;
            case     (DrawType::normalize ): DrawNormalized(processed_option);        break;
            default                        : DrawRegular(processed_option);           break;
        };
        gPad->Update();
    }


    // properties
    // -------------------------------------------------------------------------------------------------//

    // range
    void TH1Overlay::SetYAxisRange(double min, double max)
    {
        m_pimpl->yaxis_min = min;
        m_pimpl->yaxis_max = max;
    }

    //void TH1Overlay::SetXAxisRange(double min, double max)
    //{
    //    m_pimpl->xaxis_min = min;
    //    m_pimpl->xaxis_max = max;
    //}

    double TH1Overlay::GetYAxisMax() const
    {
        return m_pimpl->yaxis_max;
    }


    double TH1Overlay::GetYAxisMin() const
    {
        return m_pimpl->yaxis_min;
    }


    // title
    void TH1Overlay::SetTitle(const std::string& title)
    {
        m_pimpl->title = title;
    }


    string TH1Overlay::GetTitle() const
    {
        return m_pimpl->title;
    }


    // option
    void TH1Overlay::SetOption(const std::string& option)
    {
        m_pimpl->option = option;
    }


    string TH1Overlay::GetOption() const
    {
        return m_pimpl->option;
    }


    // draw type
    void TH1Overlay::SetDrawType(DrawType::value_type DrawType)
    {
        m_pimpl->DrawType = DrawType;
    }


    DrawType::value_type TH1Overlay::GetDrawType() const
    {
        return m_pimpl->DrawType;
    }


    // legend placement
    void TH1Overlay::SetLegendPlacement(LegendPlacement::value_type LegendPlacement)
    {
        m_pimpl->LegendPlacement = LegendPlacement;
    }


    LegendPlacement::value_type TH1Overlay::GetLegendPlacement() const
    {
        return m_pimpl->LegendPlacement;
    }


    // legend width
    void TH1Overlay::SetLegendWidth(float width)
    {
        m_pimpl->legend_width = width;
    }


    float TH1Overlay::GetLegendWidth() const
    {
        return m_pimpl->legend_width;
    }


    // legend height per entry
    void TH1Overlay::SetLegendHeightPerEntry(float height)
    {
        m_pimpl->legend_height_per_entry = height;
    }


    float TH1Overlay::GetLegendHeightPerEntry() const
    {
        return m_pimpl->legend_height_per_entry;
    }


    // legend offset
    void TH1Overlay::SetLegendOffset(float offset)
    {
        m_pimpl->legend_offset = offset;
    }


    float TH1Overlay::GetLegendOffset() const
    {
        return m_pimpl->legend_offset;
    }


    // legend text size
    void TH1Overlay::SetLegendTextSize(float text_size)
    {
        m_pimpl->legend_text_size = text_size;
    }


    float TH1Overlay::GetLegendTextSize() const
    {
        return m_pimpl->legend_text_size;
    }


    // legend number of columns 
    void TH1Overlay::SetLegendNCol(int ncol)
    {
        m_pimpl->legend_ncol = ncol;
    }


    float TH1Overlay::GetLegendNCol() const
    {
        return m_pimpl->legend_ncol;
    }


    // legend option
    void TH1Overlay::SetLegendOption(const string& option)
    {
        m_pimpl->legend_option = option;
    }


    string TH1Overlay::GetLegendOption() const
    {
        return m_pimpl->legend_option;
    }


    // statbox placement
    void TH1Overlay::SetStatBoxPlacement(StatBoxPlacement::value_type StatBoxPlacement)
    {
        m_pimpl->StatBoxPlacement = StatBoxPlacement;
    }


    StatBoxPlacement::value_type TH1Overlay::GetStatBoxPlacement() const
    {
        return m_pimpl->StatBoxPlacement;
    }


    // statbox fill color
    void TH1Overlay::SetStatBoxFillColor(Color_t color)
    {
        m_pimpl->statbox_fill_color = color;
    }


    Color_t TH1Overlay::GetStatBoxFillColor() const
    {
        return m_pimpl->statbox_fill_color;
    }


    // log y 
    void TH1Overlay::SetLogy(bool value)
    {
        m_pimpl->logy = value;
    }


    bool TH1Overlay::GetLogy() const
    {
        return m_pimpl->logy;
    }


    // log y 
    void TH1Overlay::SetLogx(bool value)
    {
        m_pimpl->logx = value;
    }


    bool TH1Overlay::GetLogx() const
    {
        return m_pimpl->logx;
    }


    // methods
    // -------------------------------------------------------------------------------------------------//

    size_t TH1Overlay::Size() const
    {
        return m_pimpl->hist_vec.size();
    }


    bool TH1Overlay::Empty() const
    {
        return m_pimpl->hist_vec.empty();
    }

    void TH1Overlay::Print(const std::string& file_name, const std::string& suffix, const std::string& option) const
    {
        rt::Print(const_cast<TH1Overlay*>(this), file_name, suffix, option); 
    }

    // related methods
    // -------------------------------------------------------------------------------------------------//

    void SetLogy(std::map<std::string, rt::TH1Overlay>& overlay_map, bool set_logy)
    {
        for (std::map<std::string, rt::TH1Overlay>::iterator iter = overlay_map.begin(); iter != overlay_map.end(); iter++)
        {
            rt::TH1Overlay& p = iter->second;
            p.SetLogy(set_logy);
        }
    }

    void SetLogy(std::vector<rt::TH1Overlay>& overlay_map, bool set_logy)
    {
        for (std::vector<rt::TH1Overlay>::iterator iter = overlay_map.begin(); iter != overlay_map.end(); iter++)
        {
            rt::TH1Overlay& p = *iter;
            p.SetLogy(set_logy);
        }
    }

    void SetOption(std::map<std::string, rt::TH1Overlay>& overlay_map, const std::string& option)
    {
        for (std::map<std::string, rt::TH1Overlay>::iterator iter = overlay_map.begin(); iter != overlay_map.end(); iter++)
        {
            rt::TH1Overlay& p = iter->second;
            p.SetOption(option);
        }
    }

    void SetOption(std::vector<rt::TH1Overlay>& overlay_map, const std::string& option)
    {
        for (std::vector<rt::TH1Overlay>::iterator iter = overlay_map.begin(); iter != overlay_map.end(); iter++)
        {
            rt::TH1Overlay& p = *iter;
            p.SetOption(option);
        }
    }

    // -------------------------------------------------------------------------------------------------//

} // namespace rt
