#include <wx/filedlg.h>
#include <wx/panel.h>
#include <wx/textctrl.h>
#include <wx/bitmap.h>
#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/msgdlg.h>
#include <wx/log.h>
#include <wx/splitter.h>
#include <wx/button.h>
#include <wx/bmpbuttn.h>

#include <wx/wx.h>

#include "frame.h"
#include "panel.h"
#include "edit.h"
#include "def.h"
#include "dialog.h"
#include "CustomTabArt.h"

#include <iostream>
using  std::cerr;
using  std::endl;

BEGIN_EVENT_TABLE(SplitterWindow,wxSplitterWindow)
    EVT_MOTION(SplitterWindow::OnMouseMotion)
END_EVENT_TABLE()

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
    EVT_MENU(myID_LINEHIGHLIGHT,MyFrame::OnLineHighlight)
    EVT_MENU(ID_COMMENT_TOGGLE,MyFrame::OnCommentToggle)
    EVT_MENU(ID_COMMENT,MyFrame::OnComment)
    EVT_MENU(ID_UNCOMMENT,MyFrame::OnUncomment)

    EVT_MENU_RANGE(myID_HIGLIGHTLANGFIRST,myID_HIGLIGHTLANGLAST,
                   MyFrame::OnHighlightLang)
    EVT_MENU(wxID_SETUP,MyFrame::OnPreference)

    EVT_STC_SAVEPOINTLEFT(wxID_ANY,MyFrame::OnSavePointReachLeave)
    EVT_STC_SAVEPOINTREACHED(wxID_ANY,MyFrame::OnSavePointReachLeave)

	EVT_DIRCTRL_FILEACTIVATED(myID_DIRCTRL,MyFrame::OnDirCtrlFileActivated)
	EVT_BUTTON(myID_LOCKBUTTON,MyFrame::OnChangeAutoHideLeftPanel)
END_EVENT_TABLE()

MyFrame::MyFrame(const wxString &title,const wxString &name,const wxString &directory)
:wxFrame(NULL,wxID_ANY,title,wxDefaultPosition,wxSize(780,500))
{
	wxIcon appIcon("images/icons/notecode64.png",wxBITMAP_TYPE_PNG);
    SetIcon(appIcon);

    SetDropTarget(new MyFileDropTarget(this));

    m_menuBar = new wxMenuBar();
    CreateMenu();

    splitWin = new SplitterWindow(this,myID_SPLITTER_WIN,wxSP_LIVE_UPDATE|wxSP_3D);
    //splitWin->Create(this,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxSP_3D|wxSP_LIVE_UPDATE);

    leftWindow = new wxPanel(splitWin,wxID_ANY);
    wxBoxSizer *leftWindowSizer = new wxBoxSizer(wxVERTICAL);
    lockedPad = new wxBitmap("images/icons/closedLockTransparent16.png",wxBITMAP_TYPE_PNG);
    unlockedPad = new wxBitmap("images/icons/openedLockTransparent16.png",wxBITMAP_TYPE_PNG);
    wxBitmapButton *lockButton = new wxBitmapButton(leftWindow,myID_LOCKBUTTON,*unlockedPad,
                                              wxDefaultPosition,wxDefaultSize,wxBU_AUTODRAW);

    wxBoxSizer *subLeftWindowSizer = new wxBoxSizer(wxHORIZONTAL);
    leftWindowSizer->Add(subLeftWindowSizer,0,wxEXPAND,0);
    subLeftWindowSizer->Add(50,10,1,wxALL,0);
    subLeftWindowSizer->Add(lockButton,0,wxALL,5);
    m_dirCtrl = new wxGenericDirCtrl(leftWindow,myID_DIRCTRL,wxDirDialogDefaultFolderStr,wxDefaultPosition,
                                      wxDefaultSize);
    leftWindowSizer->Add(m_dirCtrl,1,wxGROW|wxALL,0);
    leftWindow->SetSizer(leftWindowSizer);
    leftWindowSizer->Fit(leftWindow);
    leftWindowSizer->SetSizeHints(leftWindow);

    notebook = new wxAuiNotebook();
    notebook->Create(splitWin,wxID_ANY,wxDefaultPosition,wxDefaultSize,
                     wxAUI_NB_CLOSE_ON_ALL_TABS|wxAUI_NB_SCROLL_BUTTONS|wxAUI_NB_TAB_MOVE|wxAUI_NB_WINDOWLIST_BUTTON|
                     wxNB_NOPAGETHEME);
    splitWin->Initialize(notebook);

    splitWin->left = leftWindow;
    splitWin->right = notebook;


    CreateImageList();

    notebook->AssignImageList(imagelist);

    //CustomTabArt *tab = new CustomTabArt();
    //notebook->SetArtProvider(tab);

    CreatePage(name,directory);

}


void MyFrame::OnNew(wxCommandEvent &event)
{
    CreatePage();
}

void MyFrame::OnSavePointReachLeave(wxStyledTextEvent &event)
{
    MyPanel *panel = wxDynamicCast(notebook->GetCurrentPage(),MyPanel);
    int pageIndex = notebook->GetPageIndex(notebook->GetCurrentPage());
    wxString pageTitle = notebook->GetPageText(pageIndex);

    if(panel->text_area->GetModify() == 0)    //save point reached
    {
        notebook->SetPageText(pageIndex,pageTitle.AfterLast('*'));
    }
    else //save point left
    {
        notebook->SetPageText(pageIndex,"*"+pageTitle.AfterLast('*'));
    }
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
	panel->LoadFile(title,directory);

}


void MyFrame::OnSave(wxCommandEvent &event)
{
    MyPanel *panel = wxDynamicCast(notebook->GetCurrentPage(),MyPanel);
    wxString fullPath;
    fullPath.Printf("%s/%s",panel->GetDirectory(),panel->GetName());
    if(wxFileName::Exists(fullPath))
    {
        panel->Save();
    }
    else
    {
        OnSaveAs(event);
    }
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

    wxFileDialog dialog(this,wxT("NoteCode Save As"),default_dir,wxEmptyString,wildcard,wxFD_SAVE);

    wxString directory;
    wxString name;
    if(dialog.ShowModal() == wxID_OK)
    {
        directory = dialog.GetDirectory();
        name = dialog.GetFilename();
        MyPanel *panel = wxDynamicCast(notebook->GetCurrentPage(),MyPanel);
        panel->SaveAs(name,directory);

        notebook->SetPageText(notebook->GetSelection(),name);
    }

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
}

void MyFrame::OnLineHighlight(wxCommandEvent &event)
{
    //changes a global value
    lineHighlightPref = event.IsChecked();
    if(lineHighlightPref == true)
    {
        for(size_t i =0;i<notebook->GetPageCount();i++)
        {
            MyPanel *panel = wxDynamicCast(notebook->GetPage(i),MyPanel);
            panel->text_area->HighlightCurrentLine();
        }
    }
    else
    {
        for(size_t i= 0; i < notebook->GetPageCount();i++)
        {
            MyPanel *panel = wxDynamicCast(notebook->GetPage(i),MyPanel);
            panel->text_area->UnHighlightLine();
        }
    }

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

void MyFrame::OnDirCtrlFileActivated(wxTreeEvent &event)
{
	wxFileName fname(m_dirCtrl->GetPath(event.GetItem()));
	CreatePage(fname.GetFullName(),fname.GetPath());
}

void MyFrame::OnChangeAutoHideLeftPanel(wxCommandEvent &event)
{
    wxBitmapButton *btn = wxDynamicCast(FindWindow(myID_LOCKBUTTON),wxBitmapButton);
    if(autoHideLeftPanelPref)
    {
        autoHideLeftPanelPref = false;
        btn->SetBitmap(*lockedPad);
    }
    else
    {
        autoHideLeftPanelPref = true;
        btn->SetBitmap(*unlockedPad);
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
    menuEdit->AppendSeparator();
    menuEdit->Append(ID_COMMENT_TOGGLE,wxT("Toggle Comment\tCtrl-/"));
    menuEdit->Append(ID_COMMENT,wxT("Comment"));
    menuEdit->Append(ID_UNCOMMENT,wxT("Uncomment"));

    wxMenu *menuView = new wxMenu();
    wxMenu *HighlightLang = new wxMenu();
    //Populate HighlightLang menu with all files available
    for(int i = 0; i < FILE_TOTOAL; i++)
    {
        HighlightLang->Append(myID_HIGLIGHTLANGFIRST + i,
                              files_supported[i].name);
    }
    menuView->Append(myID_HIGLIGHTLANG,wxT("Highlight Language"),HighlightLang);

    menuView->Append(myID_LINEHIGHLIGHT,wxT("Line Highlight"),"enable or disable lihe highlighting",true);
    menuView->Check(myID_LINEHIGHLIGHT,lineHighlightPref);

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

void MyFrame::OnCommentToggle(wxCommandEvent &event)
{
    MyPanel *panel = wxDynamicCast(notebook->GetCurrentPage(),MyPanel);
    panel->text_area->CommentToggle();
}

void MyFrame::OnComment(wxCommandEvent &event)
{
    MyPanel *panel = wxDynamicCast(notebook->GetCurrentPage(),MyPanel);
    panel->text_area->Comment();
}

void MyFrame::OnUncomment(wxCommandEvent &event)
{
    MyPanel *panel = wxDynamicCast(notebook->GetCurrentPage(),MyPanel);
    panel->text_area->Uncomment();
}
