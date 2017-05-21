

#ifndef _EDIT_H_
#define _EDIT_H_

#include <wx/stc/stc.h>
#include <wx/textctrl.h>
#include "frame.h"

#include <iostream>
#include "language.h"

class Edit : public wxStyledTextCtrl
{
public:
    Edit(wxWindow *parent,wxWindowID id,
         wxPoint pos=wxDefaultPosition,
         wxSize siz=wxDefaultSize,
         long style = wxVSCROLL|wxHSCROLL);

    ~Edit()
    {
        delete lang;
    }

    void OnMarginClick(wxStyledTextEvent &event);
    void OnCharAdded(wxStyledTextEvent &event);
    void OnDoubleClick(wxStyledTextEvent &event);
    void OnModified(wxStyledTextEvent &event);
    void OnKeyDown(wxKeyEvent &event);
    void OnUndo(wxCommandEvent &event);
    void OnRedo(wxCommandEvent &event);
    void OnCut(wxCommandEvent &event);
    void OnCopy(wxCommandEvent &event);
    void OnPaste(wxCommandEvent &event);
    void OnDelete(wxCommandEvent &event);
    void OnSelectAll(wxCommandEvent &event);
    void OnLeftDown(wxMouseEvent &event);
    void OnSavePointReachLeave(wxStyledTextEvent &event);
    void OnUpdateUI(wxStyledTextEvent &event);
    void OnMouseMotion(wxMouseEvent &event)
    {
        SplitterWindow *par = wxDynamicCast(FindWindowById(myID_SPLITTER_WIN,NULL),SplitterWindow);
        if(par)
        {
            par->OnMouseMotion(event);
        }
        OnMouseMove(event);
    }

    void OnMouseEnter(wxMouseEvent &event)
    {
        if(autoHideLeftPanelPref)
        {
            SplitterWindow *par = wxDynamicCast(FindWindowById(myID_SPLITTER_WIN,NULL),SplitterWindow);
            if(par)
            {
                if(par->IsSplit())
                {
                    par->Unsplit(par->GetWindow1());
                    SetFocus();
                }
            }
        }
    }
	
	void SearchText(const wxString &text,int position = -1);
    void SearchTextDown();
    void SearchTextUp();

    void SetLanguage(int file_type);
    void RefreshStyle();
    void HighlightCurrentLine();
    void UnHighlightLine();
    void CommentToggle();
    void Comment();
    void Uncomment();
    wxString filePath;
    bool hightlightLine;
private:
    Language *lang;
    int m_LineNrID;
    int m_FoldingID;
    int m_DividerID;
    int lineMarkerHandle;
    int mainSearchSelPos;
    wxString searchText;
    DECLARE_EVENT_TABLE();

};

#endif // _EDIT_H_
