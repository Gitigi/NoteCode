#include "dialog.h"
#include <wx/colordlg.h>
#include <wx/button.h>
#include <wx/notebook.h>
#include <wx/panel.h>
#include "def.h"
#include <wx/stattext.h>
#include <wx/colour.h>
#include <wx/scrolwin.h>
#include <wx/statline.h>

IMPLEMENT_CLASS(SettingDialog,wxDialog)

BEGIN_EVENT_TABLE(SettingDialog,wxDialog)
    EVT_BUTTON(wxID_OK,SettingDialog::OnOk)
END_EVENT_TABLE()

class CustomButton : public wxButton
{
    DECLARE_EVENT_TABLE()
public:
    CustomButton(wxWindow *parent,wxWindowID id,const wxString &label);
    void OnButtonClick(wxCommandEvent &event)
    {
        wxLogMessage(wxT("button clicked"));
    }
};

CustomButton::CustomButton(wxWindow *parent,wxWindowID id,const wxString &label)
:wxButton(parent,id,label)
{

}

BEGIN_EVENT_TABLE(CustomButton,wxButton)
END_EVENT_TABLE()

SettingDialog::SettingDialog()
{

}

SettingDialog::SettingDialog(wxWindow *parent,wxWindowID id,
                             const wxString &caption,long style)
{
    Create(parent,id,caption,style);
}

SettingDialog::~SettingDialog()
{
    //dtor
}

bool SettingDialog::Create(wxWindow *parent,wxWindowID id,
                           const wxString &caption,long style)
{
    if(!wxDialog::Create(parent,id,caption,wxDefaultPosition,wxDefaultSize,style))
        return false;

    Connect(ID_DEFAULT,ID_LAST_STYLE_ELEMENT*2,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(SettingDialog::OnButtonClick));
    CreateControl();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    return true;
}

void SettingDialog::CreateControl()
{
    wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(topSizer);

    wxNotebook *prefernces = new wxNotebook(this,wxID_ANY,wxDefaultPosition,wxSize(500,500));
    topSizer->Add(prefernces,1,wxGROW|wxALL|wxALIGN_CENTER,5);

    wxPanel *theme = new wxPanel(prefernces,wxID_ANY,wxDefaultPosition,wxDefaultSize);

    prefernces->AddPage(theme,wxT("Theme"),true);

    wxScrolledWindow *scrolledWindow = new wxScrolledWindow(theme,wxID_ANY,wxDefaultPosition,wxDefaultSize,
                                                            wxVSCROLL);
    int pixelPerUnixX = 10;
    int pixelPerUnitY = 10;
    int noUnitsX = 1000;
    int noUnitsY = 1000;
    scrolledWindow->SetScrollbars(pixelPerUnixX,pixelPerUnitY,noUnitsX,noUnitsY);
    wxBoxSizer *scrollSizer = new wxBoxSizer(wxVERTICAL);
    scrolledWindow->SetSizer(scrollSizer);

    wxBoxSizer *themeSizer = new wxBoxSizer(wxVERTICAL);
    theme->SetSizer(themeSizer);
    themeSizer->Add(scrolledWindow,1,wxALL|wxGROW,5);

    //heading for the dialog
    wxStaticText *column1= new wxStaticText(scrolledWindow,wxID_ANY,wxT("ELEMENT STYLE"));
    wxStaticText *column2=new wxStaticText(scrolledWindow,wxID_ANY,wxT("FOREGROUND"));
    wxStaticText *column3=new wxStaticText(scrolledWindow,wxID_ANY,wxT("BACKGROUND"));
    wxBoxSizer *headerElement = new wxBoxSizer(wxHORIZONTAL);
    scrollSizer->Add(headerElement,0,wxALL|wxGROW,5);
    headerElement->Add(column1,1,wxRIGHT,6);
    headerElement->Add(column2,0,wxALL,2);
    headerElement->Add(column3,0,wxALL,2);

    wxBoxSizer *lineSeparatorSizer = new wxBoxSizer(wxHORIZONTAL);
    scrollSizer->Add(lineSeparatorSizer,0,wxALL|wxGROW,1);
    wxStaticLine *line = new wxStaticLine(scrolledWindow,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxLI_HORIZONTAL);
    lineSeparatorSizer->Add(line,1,wxALL,0);

    wxBoxSizer *element;
    wxStaticText *text;
    wxButton *button;
    wxButton *buttonFore;
    wxColour col;

    for(int i=0; i <StylesLength;i++)
    {
        element = new wxBoxSizer(wxHORIZONTAL);
        scrollSizer->Add(element,0,wxGROW|wxALL,5);
        text=new wxStaticText(scrolledWindow,wxID_ANY,Styles[i].name);
        element->Add(text,1,wxRIGHT,6);
        buttonFore = new wxButton(scrolledWindow,(LAST_STYLE_ELEMENT+ID_DEFAULT+Styles[i].id),wxT(""));
        col.Set(Styles[i].foreground.red,Styles[i].foreground.green,Styles[i].foreground.blue);
        buttonFore->SetBackgroundColour(col);
        element->Add(buttonFore,0,wxRIGHT,20);
        button = new wxButton(scrolledWindow,(ID_DEFAULT+Styles[i].id),wxT(""));
        col.Set(Styles[i].background.red,Styles[i].background.green,Styles[i].background.blue);
        button->SetBackgroundColour(col);
        element->Add(button,0,wxALL,2);
    }

    themeSizer->Fit(theme);
    themeSizer->SetSizeHints(theme);

    wxBoxSizer *footer = new wxBoxSizer(wxHORIZONTAL);
    topSizer->Add(footer,0,wxALL|wxGROW,5);

    footer->Add(60,30,1,wxGROW|wxALL|wxEXPAND,3);

    wxButton *ok = new wxButton(this,wxID_OK,wxT("OK"));
    footer->Add(ok,0,wxALL,2);
    wxButton *cancel = new wxButton(this,wxID_CANCEL,wxT("Cancel"));
    footer->Add(cancel,0,wxALL,2);
}

void SettingDialog::OnButtonClick(wxCommandEvent &event)
{
    wxButton *button = (wxButton *)FindWindow(event.GetId());

    wxColourData data;
    data.SetColour(button->GetBackgroundColour());
    data.SetChooseFull(true);
    wxColour color;
    for(int i=0; i< 16; i++)
    {
        color.Set(i*16,i*16,i*16);
        data.SetCustomColour(i,color);
    }
    wxColourDialog dialog(this,&data);
    if(dialog.ShowModal() == wxID_OK)
    {
        wxColourData retData = dialog.GetColourData();
        wxColour col = retData.GetColour();
        button->SetBackgroundColour(col);
        button->Refresh();
    }
}

void SettingDialog::OnOk(wxCommandEvent &event)
{
    wxButton *button;
    wxColour colour;
    for(int i = 0; i < StylesLength; i++)
    {
        button = (wxButton *) FindWindow(ID_DEFAULT+i);
        colour = button->GetBackgroundColour();
        Styles[i].background = {colour.Red(),colour.Green(),colour.Blue()};

        button = (wxButton *) FindWindow(ID_DEFAULT+LAST_STYLE_ELEMENT+i);
        colour = button->GetBackgroundColour();
        Styles[i].foreground = {colour.Red(),colour.Green(),colour.Blue()};
    }
    SaveStylesToJsonFile();
    if(Validate() && TransferDataFromWindow())
    {
        if(IsModal())
        {
            EndModal(wxID_OK);
        }
        else
        {
            SetReturnCode(wxID_OK);
            this->Show(false);
        }
    }
}
