#ifndef PANEL_H_INCLUDED
#define PANEL_H_INCLUDED

#include <wx/panel.h>
#include <wx/textctrl.h>
#include "edit.h"

class MyPanel : public wxPanel
{
public:
    MyPanel(wxWindow *parent,const wxString &name,
            const wxString &directory = wxT("/home/"));
    void Save();
    void SaveAs(const wxString &name,const wxString &dir);
    void LoadFile(const wxString &name,const wxString &dir);
	void ShowSearchControl();
    void SetName(const wxString &name)
    {
        m_name = name;
    }
    void SetDirectory(const wxString &directory)
    {
        m_directory = directory;
    }

    wxString GetName()
    {
        return m_name;
    }

    wxString GetDirectory()
    {
        return m_directory;
    }
	void SearchTextChanged(wxCommandEvent &event);
    void SearchTextDown(wxCommandEvent &event);
    void SearchTextUp(wxCommandEvent &event);
    void ChangeSearchFlags(wxCommandEvent &event);

    Edit *text_area;
private:
	wxPanel *searchPanel;
	wxTextCtrl *searchInput;
	wxBoxSizer *sizer;
    wxString m_name;
    wxString m_directory;
	
	wxDECLARE_EVENT_TABLE();
};


#endif // PANEL_H_INCLUDED
