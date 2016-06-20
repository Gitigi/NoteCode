#include <wx/filedlg.h>
#include <wx/panel.h>
#include <wx/textctrl.h>
#include <wx/bitmap.h>
#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/msgdlg.h>
#include <wx/log.h>

#include "frame.h"
#include "panel.h"
#include "edit.h"
#include "def.h"
#include "dialog.h"
#include "CustomTabArt.h"




BEGIN_EVENT_TABLE(MyFrame,wxFrame)
    EVT_MENU(wxID_NEW,MyFrame::OnNew)
    EVT_MENU(wxID_SAVE,MyFrame::OnSave)
    EVT_MENU(wxID_OPEN,MyFrame::OnOpen)
    EVT_MENU(wxID_SAVEAS,MyFrame::OnSaveAs)

    EVT_MENU(wxID_UNDO,MyFrame::OnEdit)
    EVT_MENU(wxID_REDO,MyFrame::OnEdit)
    EVT_MENU(wxID_COPY,MyFrame::OnEdit)
    EVT_MENU(wxID_CUT,MyFrame::OnEdit)
    EVT_MENU(wxID_PASTE,MyFrame::OnEdit)
    EVT_MENU(wxID_DELETE,MyFrame::OnEdit)
    EVT_MENU(wxID_SELECTALL,MyFrame::OnEdit)

    EVT_MENU_RANGE(myID_HIGLIGHTLANGFIRST,myID_HIGLIGHTLANGLAST,
                   MyFrame::OnHighlightLang)
    EVT_MENU(wxID_SETUP,MyFrame::OnPreference)
END_EVENT_TABLE()

MyFrame::MyFrame(const wxString &title,const wxString &name,const wxString &directory)
:wxFrame(NULL,wxID_ANY,title,wxDefaultPosition,wxSize(780,500))
{
    SetDropTarget(new MyFileDropTarget(this));

    m_menuBar = new wxMenuBar();
    CreateMenu();

    notebook = new wxAuiNotebook();
    notebook->Create(this,wxID_ANY,wxDefaultPosition,wxDefaultSize,
                     wxAUI_NB_CLOSE_ON_ALL_TABS|wxAUI_NB_SCROLL_BUTTONS|wxAUI_NB_TAB_MOVE|wxAUI_NB_WINDOWLIST_BUTTON|
                     wxNB_NOPAGETHEME);

    CreateImageList();

    notebook->AssignImageList(imagelist);

    CreatePage(name,directory);

}


void MyFrame::OnNew(wxCommandEvent &event)
{
    CreatePage();
}

void MyFrame::CreatePage(const wxString &title,const wxString &directory)
{
    wxFileName fname;
    fname.SetName(title);
    fname.SetPath(directory);

    MyPanel *panel = new MyPanel(notebook,title,directory);
    panel->text_area->SetDropTarget(new MyFileDropTarget(this));

    int file_type = DetermineFile(fname.GetFullName().AfterLast(wxT('.')));
    notebook->AddPage(panel,title,true,file_type);

    panel->text_area->SetLanguage(file_type);

}


void MyFrame::OnSave(wxCommandEvent &event)
{
    MyPanel *panel = wxDynamicCast(notebook->GetCurrentPage(),MyPanel);
    panel->Save();
}

void MyFrame::OnOpen(wxCommandEvent &event)
{
    wxString default_dir = wxT("/home/");
    //wxString wildcard = wxT("TXT files (*.txt)|*.txt | CPP files (*.cpp) | *.cpp | H files (*.h) | *.h");
    wxString wildcard = wxEmptyString;

    wxFileDialog dialog(this,wxT("NoteCode Open"),default_dir,wxEmptyString,wildcard,wxFD_OPEN);

    wxString path;
    wxString name;
    if(dialog.ShowModal() == wxID_OK)
    {
        path = dialog.GetDirectory();
        name = dialog.GetFilename();
        CreatePage(name,path);
    }
}

void MyFrame::OnSaveAs(wxCommandEvent &event)
{
    wxString default_dir = wxT("/home/");
    wxString wildcard = wxEmptyString;

    wxFileDialog dialog(this,wxT("NoteCode Open"),default_dir,wxEmptyString,wildcard,wxFD_SAVE);

    wxString directory;
    wxString name;
    if(dialog.ShowModal() == wxID_OK)
    {
        directory = dialog.GetDirectory();
        name = dialog.GetFilename();
    }
    MyPanel *panel = wxDynamicCast(notebook->GetCurrentPage(),MyPanel);
    panel->SetDirectory(directory);
    panel->SetName(name);
    panel->Save();

    notebook->SetPageText(notebook->GetSelection(),name);
}

void MyFrame::OnEdit(wxCommandEvent &event)
{
    MyPanel *panel = wxDynamicCast(notebook->GetCurrentPage(),MyPanel);
    panel->text_area->GetEventHandler()->ProcessEvent(event);
}

void MyFrame::OnHighlightLang(wxCommandEvent &event)
{
    MyPanel *panel = wxDynamicCast(notebook->GetCurrentPage(),MyPanel);
    panel->text_area->SetLanguage(event.GetId() - myID_HIGLIGHTLANGFIRST);
    //panel->text_area->SetLanguage(FILE_CPLUS);
}

void MyFrame::OnPreference(wxCommandEvent &event)
{
    SettingDialog dlg(this,wxID_ANY);
    if(dlg.ShowModal() == wxID_OK)
    {
        MyPanel *panel;
        for(size_t i=0; i < notebook->GetPageCount(); i++)
        {
            panel = (MyPanel *)notebook->GetPage(i);
            panel->text_area->RefreshStyle();
        }
    }
}


void MyFrame::CreateMenu()
{
    wxMenu *menuFile = new wxMenu();
    menuFile->Append(wxID_NEW,wxT("&New \tCtrl - N"),wxT("Open New File"));
    menuFile->Append(wxID_OPEN,wxT("&Open \tCtrl - O"),wxT("Open File"));
    menuFile->Append(wxID_SAVE,wxT("&Save \tCtrl - S"),wxT("Save File"));
    menuFile->Append(wxID_SAVEAS,wxT("Save As"),wxT("Save with another name"));
    menuFile->Append(wxID_SETUP,wxT("&Preference"),wxT("Edit Setting"));

    wxMenu *menuEdit = new wxMenu();
    menuEdit->Append(wxID_UNDO);
    menuEdit->Append(wxID_REDO);
    menuEdit->AppendSeparator();
    menuEdit->Append(wxID_CUT);
    menuEdit->Append(wxID_COPY);
    menuEdit->Append(wxID_PASTE);
    menuEdit->Append(wxID_DELETE);
    menuEdit->Append(wxID_SELECTALL);

    wxMenu *menuView = new wxMenu();
    wxMenu *HighlightLang = new wxMenu();
    //Populate HighlightLang menu with all files available
    for(int i = 0; i < FILE_TOTOAL; i++)
    {
        HighlightLang->Append(myID_HIGLIGHTLANGFIRST + i,
                              files_supported[i].name);
    }
    menuView->Append(myID_HIGLIGHTLANG,wxT("Highlight Language"),HighlightLang);

    // construct menubar
    m_menuBar->Append (menuFile, _("&File"));
    m_menuBar->Append(menuEdit,wxT("Edit"));
    m_menuBar->Append(menuView,wxT("View"));

    SetMenuBar (m_menuBar);

}

void MyFrame::CreateImageList()
{
    imagelist = new wxImageList(32,32,true,FILE_TOTOAL);
    imagelist->Add(wxBitmap(wxT("images/32/c++.png"),wxBITMAP_TYPE_PNG));
    imagelist->Add(wxBitmap(wxT("images/32/python.png"),wxBITMAP_TYPE_PNG));
    imagelist->Add(wxBitmap(wxT("images/32/css.png"),wxBITMAP_TYPE_PNG));
    imagelist->Add(wxBitmap(wxT("images/32/xml.png"),wxBITMAP_TYPE_PNG));
    imagelist->Add(wxBitmap(wxT("images/32/xml.png"),wxBITMAP_TYPE_PNG));
    imagelist->Add(wxBitmap(wxT("images/32/c.png"),wxBITMAP_TYPE_PNG));
    imagelist->Add(wxBitmap(wxT("images/32/php.png"),wxBITMAP_TYPE_PNG));
    imagelist->Add(wxBitmap(wxT("images/32/csharp.png"),wxBITMAP_TYPE_PNG));
    imagelist->Add(wxBitmap(wxT("images/32/java.png"),wxBITMAP_TYPE_PNG));
    imagelist->Add(wxBitmap(wxT("images/32/javascript.png"),wxBITMAP_TYPE_PNG));
    imagelist->Add(wxBitmap(wxT("images/32/text.png"),wxBITMAP_TYPE_PNG));
    imagelist->Add(wxBitmap(wxT("images/32/shellscript.png"),wxBITMAP_TYPE_PNG));
    imagelist->Add(wxBitmap(wxT("images/32/ruby.png"),wxBITMAP_TYPE_PNG));

}


int MyFrame::DetermineFile(const wxString &extension)
{
    int n;
    for(n = 0; n < FILE_TOTOAL; n++)
    {
        if(files_supported[n].ext.Contains(extension))
            return files_supported[n].file_type;

    }
    return FILE_TXT;
}
