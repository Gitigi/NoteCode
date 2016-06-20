#include "edit.h"
#include "def.h"
#include "language.h"
#include <wx/log.h>
#include <wx/wx.h>

#include <wx/msgdlg.h>

BEGIN_EVENT_TABLE(Edit,wxStyledTextCtrl)
    EVT_STC_MARGINCLICK(wxID_ANY, Edit::OnMarginClick)
    EVT_STC_CHARADDED(wxID_ANY, Edit::OnCharAdded)
    EVT_STC_DOUBLECLICK(wxID_ANY,Edit::OnDoubleClick)
    EVT_LEFT_DOWN(Edit::OnLeftDown)
    EVT_KEY_DOWN(Edit::OnKeyDown)

    EVT_MENU(wxID_UNDO, Edit::OnUndo)
    EVT_MENU(wxID_REDO, Edit::OnRedo)
    EVT_MENU(wxID_CUT, Edit::OnCut)
    EVT_MENU(wxID_COPY, Edit::OnCopy)
    EVT_MENU(wxID_PASTE, Edit::OnPaste)
    EVT_MENU(wxID_DELETE, Edit::OnDelete)
    EVT_MENU(wxID_SELECTALL, Edit::OnSelectAll)
END_EVENT_TABLE()


Edit::Edit(wxWindow *parent,wxWindowID id,wxPoint pos,wxSize siz,long style)
:wxStyledTextCtrl(parent,id,pos,siz,style)
{
    SetMouseDwellTime(100);
    SetMouseDownCaptures(true);

    m_LineNrID = 0;
    m_DividerID = 1;
    m_FoldingID = 2;

    wxFont font(10,wxFONTFAMILY_MODERN,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL);
    StyleSetFont(wxSTC_STYLE_DEFAULT,font);
    StyleSetBackground(wxSTC_STYLE_DEFAULT,*wxWHITE);
    StyleSetForeground(wxSTC_STYLE_DEFAULT,*wxBLACK);

    StyleSetBackground(wxSTC_STYLE_LINENUMBER,*wxWHITE);
    StyleSetForeground(wxSTC_STYLE_LINENUMBER,wxColour(wxT("DARK GREY")));


    int LineNrLength = TextWidth(wxSTC_STYLE_LINENUMBER,wxT("_999999"));
    SetMarginType(m_LineNrID,wxSTC_MARGIN_NUMBER);
    SetMarginWidth(m_LineNrID,LineNrLength);

    SetMarginType(m_DividerID,wxSTC_MARGIN_SYMBOL);
    SetMarginWidth(m_DividerID,0);

    SetMarginType(m_FoldingID,wxSTC_MARGIN_SYMBOL);
    SetMarginWidth(m_FoldingID,20);
    SetMarginMask (m_FoldingID, wxSTC_MASK_FOLDERS);
    SetMarginSensitive(m_FoldingID,true);

    SetFoldFlags (wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED |
                  wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED);

    MarkerDefine(wxSTC_MARKNUM_FOLDER,wxSTC_MARK_BOXPLUS,*wxWHITE,*wxBLACK);
    MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN,wxSTC_MARK_BOXMINUS,*wxWHITE,*wxBLACK);
    MarkerDefine(wxSTC_MARKNUM_FOLDERSUB,wxSTC_MARK_VLINE,*wxWHITE,*wxBLACK);
    MarkerDefine(wxSTC_MARKNUM_FOLDEREND,wxSTC_MARK_BOXPLUSCONNECTED,*wxWHITE,*wxBLACK);
    MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID,wxSTC_MARK_BOXMINUSCONNECTED,*wxWHITE,*wxBLACK);
    MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL,wxSTC_MARK_TCORNER,*wxWHITE,*wxBLACK);
    MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL,wxSTC_MARK_LCORNER,*wxWHITE,*wxBLACK);

    //Set Default styles
    int Nr;
    for (Nr = 0; Nr < wxSTC_STYLE_LASTPREDEFINED; Nr++) {
        wxFont font (10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
        StyleSetFont (Nr, font);
    }

    SetTabWidth(TAB_SIZE);
    SetLayoutCache (wxSTC_CACHE_PAGE);
}

void Edit::OnMarginClick (wxStyledTextEvent &event)
{
    if (event.GetMargin() == 2)
        {
            int lineClick = LineFromPosition (event.GetPosition());
            int levelClick = GetFoldLevel (lineClick);
            if ((levelClick & wxSTC_FOLDLEVELHEADERFLAG) > 0) {
                ToggleFold (lineClick);
                }
        }

}

void Edit::OnCharAdded(wxStyledTextEvent &event)
{
    char letter = (char)event.GetKey();

    if(letter == '\n')
    {
        int current_line = GetCurrentLine();

        int indentation = GetLineIndentation(current_line-1);
        SetLineIndentation(current_line,indentation);

        GotoPos(GetLineEndPosition(current_line));

        lang->OnNewLine(event);
    }

    lang->OnCharAdded(event);
}

void Edit::OnDoubleClick(wxStyledTextEvent &event)
{
    wxString selectedText = GetSelectedText();

    //Hold position of selected text
    int initialSelPos = GetSelectionStart();

    if(selectedText.IsEmpty())
        return;

    int searchStart = 0;
    int textLength = GetTextLength();
    while(searchStart < textLength)
    {
        searchStart = FindText(searchStart,textLength,selectedText);
        if(searchStart == -1)
            break;

        //Avoid highlighting the initially selected word
        if(searchStart == initialSelPos)
        {
            searchStart += selectedText.Length();
            continue;
        }
        AddSelection(searchStart+selectedText.Length(),searchStart);

        searchStart += selectedText.Length();
    }

    //if addition selection were made add initially selected word to
    //make it main selection again
    if(GetSelections() > 1)
        AddSelection(initialSelPos+selectedText.Length(),initialSelPos);

}

void Edit::OnKeyDown(wxKeyEvent &event)
{
    lang->OnKeyDown(event);
    event.Skip();
}

void Edit::SetLanguage(int file_type)
{
    switch(file_type)
    {
    case FILE_CPLUS:
    case FILE_C:
        lang = new LanguageCpp(this);
        lang->InitializeSCT();
        break;

    case FILE_PY:
        lang = new LanguagePython(this);
        lang->InitializeSCT();
        break;

    case FILE_HTML:
        lang = new LanguageHtml(this);
        lang->InitializeSCT();
        break;

    case FILE_JAVA:
        lang = new LanguageJava(this);
        lang->InitializeSCT();
        break;

    case FILE_JAVASCRIPT:
        lang = new LanguageJS(this);
        lang->InitializeSCT();
        break;

    case FILE_CSS:
        lang = new LanguageCss(this);
        lang->InitializeSCT();
        break;

    case FILE_XML:
        lang = new LanguageXml(this);
        lang->InitializeSCT();
        break;

    case FILE_PHP:
        lang = new LanguagePhp(this);
        lang->InitializeSCT();
        break;

    default:
        lang = new Language(this);
        lang->InitializeSCT();
        break;

    }
}

void Edit::OnUndo(wxCommandEvent &event)
{
    if(CanUndo())
        Undo();
}

void Edit::OnRedo(wxCommandEvent &event)
{
    if(CanRedo())
        Redo();
}

void Edit::OnCut(wxCommandEvent &event)
{
    if((GetSelectionEnd() - GetSelectionStart()) > 0)
        Cut();
}

void Edit::OnCopy(wxCommandEvent &event)
{
    if((GetSelectionEnd()-GetSelectionStart()) > 0)
        Copy();
}

void Edit::OnPaste(wxCommandEvent &event)
{
    if(CanPaste())
        Paste();
}

void Edit::OnDelete(wxCommandEvent &event)
{
    DeleteBack();
}

void Edit::OnSelectAll(wxCommandEvent &event)
{
    ClearSelections();
    SetSelection(0,GetTextLength());
}

//use as a means of notifying change of cursor position
void Edit::OnLeftDown(wxMouseEvent &event)
{
    //prevent event from propagating up
    event.Skip(false);
    //call OnMouseLeftDown to update the cursor position
    OnMouseLeftDown(event);
    lang->OnCursorPositionChange();
}

void Edit::RefreshStyle()
{
    lang->InitializeSCT();
}
