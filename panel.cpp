#include "panel.h"
#include <wx/sizer.h>
#include <wx/file.h>

MyPanel::MyPanel(wxWindow *parent,const wxString &name,
                 const wxString &directory)
:wxPanel(parent,wxID_ANY)
{
    text_area = new Edit(this,wxID_ANY);
    text_area->SetFocus();

    wxString path;
    path.Printf(wxT("%s/%s"),directory,name);
    if(wxFile::Exists(path))
    {
        //text_area->LoadFile(path);
        //text_area->SetSavePoint();
    }
    text_area->filePath = path;

    m_name = name;

    if(directory == wxEmptyString)
        m_directory = wxT("/home/");
    else
        m_directory = directory;


    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(sizer);

    sizer->Add(text_area,1,wxALL|wxGROW,0);
    sizer->Fit(this);
    sizer->SetSizeHints(this);
}

void MyPanel::Save()
{
    if(text_area->GetModify() != 0)
    {
        wxString path;
        path.Printf(wxT("%s/%s"),m_directory,m_name);
        text_area->SaveFile(path);
        text_area->filePath=path;
        text_area->SetSavePoint();
    }
}

void MyPanel::SaveAs(const wxString &name,const wxString &dir)
{
    m_directory = dir;
    m_name = name;

    wxString path;
    path.Printf("%s/%s",m_directory,m_name);
    text_area->SaveFile(path);
    text_area->filePath = path;
    text_area->SetSavePoint();
}


void MyPanel::LoadFile(const wxString &name,const wxString &dir)
{
	wxString path;
    path.Printf(wxT("%s/%s"),dir,name);
    if(wxFile::Exists(path))
    {
        text_area->LoadFile(path);
        //text_area->SetSavePoint();
    }
    text_area->filePath = path;
}