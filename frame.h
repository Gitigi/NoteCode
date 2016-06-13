#ifndef FRAME_H_INCLUDED
#define FRAME_H_INCLUDED


#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/aui/auibook.h>
#include <wx/dnd.h>
#include <wx/filename.h>

#include "def.h"

class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString &title,const wxString &name = wxT("Unammed"),
            const wxString &directory = wxEmptyString);
    void CreateMenu();
    void CreatePage(const wxString &title=wxT("Unammed"),const wxString &path=wxEmptyString);
    void OnNew(wxCommandEvent &event);
    void OnSave(wxCommandEvent &event);
    void OnOpen(wxCommandEvent &event);
    void OnSaveAs(wxCommandEvent &event);
    void OnEdit(wxCommandEvent &event);
    void OnHighlightLang(wxCommandEvent &event);
    void OnPreference(wxCommandEvent &event);
private:
    void CreateImageList();
    int DetermineFile(const wxString &extension = wxEmptyString);
    wxImageList *imagelist;
    wxMenuBar *m_menuBar;
    wxAuiNotebook *notebook;
    DECLARE_EVENT_TABLE();
};

class MyFileDropTarget : public wxFileDropTarget
{
public:
    MyFileDropTarget(MyFrame *frame)
    {
        m_frame = frame;
    }

    virtual bool OnDropFiles(wxCoord x,wxCoord y,const wxArrayString &filenames)
    {
        size_t nFiles = filenames.GetCount();
        wxFileName file;
        wxString name;
        wxString directory;
        for(size_t n=0; n < nFiles; n++)
        {
            file = wxFileName(filenames[n]);
            name = file.GetFullName();
            directory = file.GetPath();

            m_frame->CreatePage(name,directory);
        }
        return true;
    }
private:
    MyFrame *m_frame;
};

#endif
