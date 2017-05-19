#include "panel.h"
#include <wx/sizer.h>
#include <wx/file.h>
#include <wx/artprov.h>
#include <wx/button.h>
#include <wx/bmpbuttn.h>
#include <wx/checkbox.h>
#include <iostream>

BEGIN_EVENT_TABLE(MyPanel,wxPanel)
	EVT_TEXT(ID_SEARCH_TEXT_INPUT,MyPanel::SearchTextChanged)
    EVT_BUTTON(ID_SEARCH_DOWN,MyPanel::SearchTextDown)
    EVT_BUTTON(ID_SEARCH_UP,MyPanel::SearchTextUp)
END_EVENT_TABLE()

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
	
	//search component
	searchPanel = new wxPanel(this,wxID_ANY);
	searchInput = new SearchTextCtrl(searchPanel,ID_SEARCH_TEXT_INPUT,wxEmptyString,wxDefaultPosition,wxSize(240,30));
	wxBoxSizer *searchSizer = new wxBoxSizer(wxHORIZONTAL);
	searchPanel->SetSizer(searchSizer);
	
	wxBitmap goUp = wxArtProvider::GetBitmap(wxART_GO_UP,wxART_BUTTON);
	wxBitmap goDown = wxArtProvider::GetBitmap(wxART_GO_DOWN,wxART_BUTTON);
	wxBitmapButton *goUpButton = new wxBitmapButton(searchPanel,ID_SEARCH_UP,goUp,
                                              wxDefaultPosition,wxDefaultSize,wxBU_AUTODRAW);

	wxBitmapButton *goDownButton = new wxBitmapButton(searchPanel,ID_SEARCH_DOWN,goDown,
                                              wxDefaultPosition,wxDefaultSize,wxBU_AUTODRAW);
	
	wxCheckBox *matchCase = new wxCheckBox(searchPanel,wxID_ANY,wxT("Match Case"));
	wxCheckBox *matchRegex = new wxCheckBox(searchPanel,wxID_ANY,wxT("Regex"));
	wxCheckBox *matchWords = new wxCheckBox(searchPanel,wxID_ANY,wxT("Words"));

	
	searchSizer->Add(searchInput,1);
	searchSizer->Add(goDownButton,0);
	searchSizer->Add(goUpButton,0);
	searchSizer->Add(matchCase,0);
	searchSizer->Add(matchRegex,0);
	searchSizer->Add(matchWords,0);
	
	searchSizer->Fit(searchPanel);
	searchSizer->SetSizeHints(searchPanel);
	searchPanel->Show(false);

    sizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(sizer);

	sizer->Add(searchPanel,0,wxALL,0);
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
        text_area->SetSavePoint();
    }
    text_area->filePath = path;
}

void MyPanel::ShowSearchControl()
{
	if(searchPanel->IsShown()){
		searchPanel->Show(false);
		sizer->Layout();
		text_area->SetFocus();
	}
	else{
		searchPanel->Show(true);
		searchInput->SetFocus();
		sizer->Layout();
	}
}

void MyPanel::SearchTextChanged(wxCommandEvent &event)
{
	wxTextCtrl *search = (wxTextCtrl *)event.GetEventObject();
	text_area->SearchText(search->GetValue());
}

void MyPanel::SearchTextDown(wxCommandEvent &event)
{
    text_area->SearchTextDown();
}

void MyPanel::SearchTextUp(wxCommandEvent &event)
{
    text_area->SearchTextUp();
}

wxBEGIN_EVENT_TABLE(SearchTextCtrl,wxTextCtrl)
	EVT_KEY_DOWN(SearchTextCtrl::OnChar)
wxEND_EVENT_TABLE()

void SearchTextCtrl::OnChar(wxKeyEvent &event)
{
	if(event.ControlDown() && event.GetKeyCode() == 70){
		wxDynamicCast(GetParent()->GetParent(),MyPanel)->ShowSearchControl();
	}
	else{
		event.Skip();
	}
}
