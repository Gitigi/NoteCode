#ifndef PANEL_H_INCLUDED
#define PANEL_H_INCLUDED

#include <wx/panel.h>
#include "edit.h"

class MyPanel : public wxPanel
{
public:
    MyPanel(wxWindow *parent,const wxString &name,
            const wxString &directory = wxT("/home/"));
    void Save();
    void SetName(const wxString &name)
    {
        m_name = name;
    }
    void SetDirectory(const wxString &directory)
    {
        m_directory = directory;
    }

    Edit *text_area;
private:
    wxString m_name;
    wxString m_directory;
};


#endif // PANEL_H_INCLUDED
