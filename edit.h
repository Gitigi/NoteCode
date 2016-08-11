

#ifndef _EDIT_H_
#define _EDIT_H_

#include <wx/stc/stc.h>
#include <wx/textctrl.h>

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

    void SetLanguage(int file_type);
    void RefreshStyle();
    void UpdateLine();
    wxString filePath;
private:
    Language *lang;
    int m_LineNrID;
    int m_FoldingID;
    int m_DividerID;
    bool hightlightLine;
    int lineMarkerHandle;
    DECLARE_EVENT_TABLE();

};

#endif // _EDIT_H_
