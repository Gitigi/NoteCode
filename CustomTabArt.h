#ifndef CUSTOMTABART_H_INCLUDED
#define CUSTOMTABART_H_INCLUDED

#include <wx/aui/tabart.h>
#include <wx/aui/auibook.h>
#include <wx/dc.h>

class CustomTabArt : public wxAuiTabArt
{
public:
    CustomTabArt();
    void DrawBackground(wxDC &dc,wxWindow *wnd,const wxRect &rect);
    void DrawTab(wxDC& dc,wxWindow* wnd,const wxAuiNotebookPage& page,const wxRect& in_rect,
                        int close_button_state,wxRect* out_tab_rect,wxRect* out_button_rect,int* x_extent);

    void DrawButton(wxDC& dc,
                                    wxWindow* WXUNUSED(wnd),
                                    const wxRect& in_rect,
                                    int bitmap_id,
                                    int button_state,
                                    int orientation,
                                    wxRect* out_rect);
    virtual ~CustomTabArt();

    wxAuiTabArt *Clone();

    void SetFlags(unsigned int flags);

    int GetIndentSize();

    int GetBorderWidth(wxWindow* wnd);

    int GetAdditionalBorderSpace(wxWindow* WXUNUSED(wnd));

    wxSize GetTabSize(wxDC& dc,
                                      wxWindow* WXUNUSED(wnd),
                                      const wxString& caption,
                                      const wxBitmap& bitmap,
                                      bool WXUNUSED(active),
                                      int close_button_state,
                                      int* x_extent);

    void SetSizingInfo(const wxSize& tab_ctrl_size,size_t tab_count);

    void DrawBorder(wxDC& dc, wxWindow* wnd, const wxRect& rect);

    void SetActiveColour(const wxColour& colour);

    void SetColour(const wxColour& colour);

    void SetMeasuringFont(const wxFont& font);

    void SetSelectedFont(const wxFont& font);

    void SetNormalFont(const wxFont& font);

    int GetBestTabCtrlSize(wxWindow* wnd,
                                           const wxAuiNotebookPageArray& pages,
                                           const wxSize& requiredBmp_size);

    int ShowDropDown(wxWindow* wnd,
                                     const wxAuiNotebookPageArray& pages,
                                     int /*active_idx*/);

protected:

    wxFont m_normalFont;
    wxFont m_selectedFont;
    wxFont m_measuringFont;
    wxColour m_baseColour;
    wxPen m_baseColourPen;
    wxPen m_borderPen;
    wxBrush m_baseColourBrush;
    wxColour m_activeColour;
    wxBitmap m_activeCloseBmp;
    wxBitmap m_disabledCloseBmp;
    wxBitmap m_activeLeftBmp;
    wxBitmap m_disabledLeftBmp;
    wxBitmap m_activeRightBmp;
    wxBitmap m_disabledRightBmp;
    wxBitmap m_activeWindowListBmp;
    wxBitmap m_disabledWindowListBmp;

    int m_fixedTabWidth;
    int m_tabCtrlHeight;
    unsigned int m_flags;
};

#endif // CUSTOMTABART_H_INCLUDED
