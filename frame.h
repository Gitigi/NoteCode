#ifndef FRAME_H_INCLUDED
#define FRAME_H_INCLUDED


#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/aui/auibook.h>
#include <wx/dnd.h>
#include <wx/filename.h>
#include <wx/stc/stc.h>
#include <wx/splitter.h>
#include <wx/panel.h>
#include <wx/dirctrl.h>

#include "def.h"

class SplitterWindow : public wxSplitterWindow
{
public:
    SplitterWindow(wxWindow *parent,wxWindowID id,long style)
    :wxSplitterWindow(parent,id,wxDefaultPosition,wxDefaultSize,style)
    {

    }
    void OnMouseMotion(wxMouseEvent &event)
    {
        static bool decreasing=false;
        static int lastPos = 0;
        if(autoHideLeftPanelPref)
        {
            int currentPos = event.GetX();
            if(currentPos <10 && decreasing)
            {
                if(!IsSplit())
                {
                    left->Show(true);
                    right->Show(true);
                    SplitVertically(left,right,GetSize().GetWidth()*0.3);
                    decreasing = false;
                }
            }
            else if(currentPos < lastPos)
            {
                decreasing = true;
            }
            else if(currentPos > lastPos)
            {
                decreasing = false;
            }

            lastPos = currentPos;
        }
    }
    wxWindow *left;
    wxWindow *right;
private:
    DECLARE_EVENT_TABLE();
};

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
    void OnLineHighlight(wxCommandEvent &event);
    void OnPreference(wxCommandEvent &event);
    void OnSavePointReachLeave(wxStyledTextEvent &event);
    void OnDirCtrlFileActivated(wxTreeEvent &event);
    void OnChangeAutoHideLeftPanel(wxCommandEvent &event);
    void OnCommentToggle(wxCommandEvent &event);
    void OnComment(wxCommandEvent &event);
    void OnUncomment(wxCommandEvent &event);
    void OnFind(wxCommandEvent &event);
    void OnChangeIndentation(wxCommandEvent &event);
    void SetIndentation(int id);
private:
    void CreateImageList();
    int DetermineFile(const wxString &extension = wxEmptyString);
    wxImageList *imagelist;
    wxMenuBar *m_menuBar;
    wxAuiNotebook *notebook;
    SplitterWindow *splitWin;
    wxGenericDirCtrl *m_dirCtrl;
    wxPanel *leftWindow;
    wxBitmap *lockedPad;
    wxBitmap *unlockedPad;
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
