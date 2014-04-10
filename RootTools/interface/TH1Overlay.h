#ifndef RT_TH1OVERLAY_H
#define RT_TH1OVERLAY_H

// c++ includes
#include <string>
#include <memory>
#include <map>
#include <vector>

// ROOT includes
#include "TH1.h"
#include "TLatex.h"
#include "TLine.h"

// ---------------------------------------------------------------------------------------- //
// TODO
// 1.  better interface for legend 
// ---------------------------------------------------------------------------------------- //

// uses ROOT name convecntions for classes and functions

namespace rt
{
    // enumeration used to determine the default drawing type
    struct DrawType
    {
        enum value_type
        {
            none,        // (option string: "dt::none"      ) simple overly of all histograms
            normalize,   // (option string: "dt::norm"      ) normalize all histograms and overlay them
            stack,       // (option string: "dt::stack"     ) stacked plot 
            stack_norm,  // (option string: "dt::stack_norm") stacked plot where the stack is normalized to one
            static_size,
    
            DrawTypeDefault = none
        };
    };
    
    // an enumeration for the legend placement
    struct LegendPlacement
    {
        enum value_type
        {
            top,
            top_left,
            top_right,
            top_middle_right,
            right,
            left,
            bottom_left,
            bottom_right,
            bottom,
            static_size,
    
            disabled,
            LegendPlacementDefault = top
        };
    };
    
    // an enumeration to place the placement of the statbox
    struct StatBoxPlacement
    {
        enum value_type
        {
            top_left,
            top_right,
            top_middle,
            top_middle_left,
            top_middle_right,
            top,
            right,
            left,
            bottom_left,
            bottom_right,
            bottom_middle,
            bottom,
            static_size,
    
            disabled,
            StatBoxPlacementDefault = top_middle_right
        };
    };
    
    // some convenience typedefs
    typedef StatBoxPlacement sb;
    typedef LegendPlacement  lg;
    typedef DrawType         dt;
    
    class TH1Overlay
    {
    public:
    
        // constructs
        TH1Overlay();
        TH1Overlay
        (
            const std::string& title, 
            StatBoxPlacement::value_type sb_place = StatBoxPlacement::StatBoxPlacementDefault,
            LegendPlacement::value_type leg_place = LegendPlacement::LegendPlacementDefault,
            DrawType::value_type DrawType = DrawType::DrawTypeDefault
        );
        TH1Overlay(const std::string& title, const std::string& option);
        ~TH1Overlay();
        TH1Overlay(const TH1Overlay& rhs);
        TH1Overlay& operator=(const TH1Overlay& rhs);
        void Swap(TH1Overlay& other);
    
        // add histograms to the overlay (creates a clone and owns the clone)
        void Add(TH1* h, bool d, Color_t c = -1, Width_t w = -1, Style_t s = -1, Style_t f = -1);
        void Add(TH1* h, bool d, const std::string& legend, Color_t c = -1, Width_t w = -1, Style_t = -1, Style_t f = -1);
        void Add(TH1* h, bool d, const char* legend, Color_t c = -1, Width_t w = -1, Style_t = -1, Style_t f = -1);
        void Add(TH1* h, Color_t c = -1, Width_t w = -1, Style_t s = -1, Style_t f = -1);
        void Add(TH1* h, const std::string& legend, Color_t c = -1, Width_t w = -1, Style_t = -1, Style_t f = -1);
        void Add(TH1* h, const char* legend, Color_t c = -1, Width_t w = -1, Style_t = -1, Style_t f = -1);

        // add text
        void AddText(const TLatex* text);
        void AddText(const TLatex& text);
        void AddText(const std::string& text, float x, float y, float text_size = 0.035);
    
        // add line
        void AddLine(const TLine* line);
        void AddLine(const TLine& line);
        void AddLine(float x1, float y1, float x2, float y2, Color_t c = kBlack);

        // remove histogram 
        void Remove(const std::string& hist_name);
        void Remove(const char* hist_name);
        void Remove(const TString& hist_name);
        void Remove(TH1* h);
    
        // list the histograms in the overlay
        void List() const;
    
        // draw
        void Draw(const std::string& option = "");
        void DrawRegular(const std::string& option = "");
        void DrawNormalized(const std::string& option = "");
        void DrawStacked(const std::string& option = "");
        void DrawStackedNormalized(const std::string& option = "");
    
        //properties
        void SetYAxisRange(double min, double max);
        double GetYAxisMax() const;
        double GetYAxisMin() const;

        // not working yet...
        //void SetXAxisRange(double min, double max);
        //double GetYAxisMax() const;
        //double GetYAxisMin() const;
    
        void SetTitle(const std::string& title);
        std::string GetTitle() const;
    
        void SetOption(const std::string& option);
        std::string GetOption() const;
    
        void SetDrawType(DrawType::value_type DrawType);
        DrawType::value_type GetDrawType() const;
    
        void SetLegendPlacement(LegendPlacement::value_type LegendPlacement);
        LegendPlacement::value_type GetLegendPlacement() const;
    
        void SetLegendWidth(float width);
        float GetLegendWidth() const;
    
        void SetLegendHeightPerEntry(float height);
        float GetLegendHeightPerEntry() const;
    
        void SetLegendOffset(float offset);
        float GetLegendOffset() const;
    
        void SetLegendTextSize(float size);
        float GetLegendTextSize() const;
    
        void SetLegendNCol(int ncol);
        float GetLegendNCol() const;
    
        void SetLegendOption(const std::string& option);
        std::string GetLegendOption() const;
    
        void SetStatBoxPlacement(StatBoxPlacement::value_type StatBoxPlacement);
        StatBoxPlacement::value_type GetStatBoxPlacement() const;
    
        void SetStatBoxFillColor(Color_t color);
        Color_t GetStatBoxFillColor() const;
    
        void SetLogy(bool value);
        bool GetLogy() const;
    
        void SetLogx(bool value);
        bool GetLogx() const;
    
        /*
          void SetProfileMarkerSize(float size);
          float GetProfileMarkerSize() const;
    
          void SetProfileMarkerStyle(Style_t style);
          Style_t GetProfileMarkerStyle() const;
        */
    
        // methods
        size_t Size() const;
        bool Empty() const;
    
        // write a canvas of the overlay to a root file
        //void Write(const std::string& file_name, const std::string& root_file_dir = "", const std::string& option = "UPDATE") const;
            
        // print overlay to an (eps, png, pdf)
        void Print(const std::string& dir_name, const std::string& suffix = "png", const std::string& option = "") const;
    
        // static public constants
        static float       legend_width_default;            
        static float       legend_height_per_entry_default; 
        static float       legend_offset_default;           
        static float       legend_text_size_default;        
        static int         legend_ncol_default;        
        static const char* legend_option_default;           
        static Color_t     statbox_fill_color_default;      
        static float       profile_marker_size_default;     
        static Style_t     profile_marker_style_default;    
    
    private:
    
        // implementation functions
        void BuildLegend();
        void BuildStack();
        void DrawNonStackedHists(const std::string& option);
        void DrawLegend() const;
        void DrawStatBoxes();
        void DrawText();
        void DrawLines();
        void SetLog();
        std::string ProcessOptionString(const std::string& option);
    
        // data members
        struct impl;
        std::auto_ptr<impl> m_pimpl;
    };
    
    // related methods
    void SetLogy(std::map<std::string, rt::TH1Overlay>& overlay_map, bool set_logy);
    void SetLogy(std::vector<rt::TH1Overlay>& overlay_map, bool set_logy);

    void SetOption(std::map<std::string, rt::TH1Overlay>& overlay_map, const std::string& option);
    void SetOption(std::vector<rt::TH1Overlay>& overlay_map, const std::string& option);

} // namespace rt


#endif //#define RT_TH1OVERLAY_H 
