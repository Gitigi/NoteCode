#ifndef DIALOG_H
#define DIALOG_H

#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/log.h>


class SettingDialog : public wxDialog
{
    DECLARE_CLASS(SettingDialog)
public:
    SettingDialog();
    SettingDialog( wxWindow *parent,wxWindowID id=wxID_ANY,
                  const wxString &caption = wxT("Preference"),
                  long style = wxCAPTION|wxRESIZE_BORDER|wxCLOSE_BOX);

    virtual ~SettingDialog();
    virtual bool Create(wxWindow *parent,wxWindowID=wxID_ANY,
                const wxString &caption = wxT("Preference"),
                long style = wxCAPTION|wxRESIZE_BORDER|wxCLOSE_BOX);

    void CreateControl();
    void OnButtonClick(wxCommandEvent &event);
    void OnOk(wxCommandEvent &event);
protected:

private:
    DECLARE_EVENT_TABLE()
};

#endif // DIALOG_H
