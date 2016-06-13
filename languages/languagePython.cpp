#include "../language.h"
#include "../def.h"
#include <wx/log.h>

// Python
static const char* PythonWordlist1 =
    "and assert break class continue def del elif else except exec "
    "finally for from global if import in is lambda None not or pass "
    "print raise return try while yield";
static const char* PythonWordlist2 =
    "ACCELERATORS ALT AUTO3STATE AUTOCHECKBOX AUTORADIOBUTTON BEGIN "
    "BITMAP BLOCK BUTTON CAPTION CHARACTERISTICS CHECKBOX CLASS "
    "COMBOBOX CONTROL CTEXT CURSOR DEFPUSHBUTTON DIALOG DIALOGEX "
    "DISCARDABLE EDITTEXT END EXSTYLE FONT GROUPBOX ICON LANGUAGE "
    "LISTBOX LTEXT MENU MENUEX MENUITEM MESSAGETABLE POPUP PUSHBUTTON "
    "RADIOBUTTON RCDATA RTEXT SCROLLBAR SEPARATOR SHIFT STATE3 "
    "STRINGTABLE STYLE TEXTINCLUDE VALUE VERSION VERSIONINFO VIRTKEY";

static const char * KeyWordListPython[] =
{
    PythonWordlist1,
    PythonWordlist2,
    NULL,
};

static lang_style python_styles[] =
{
    {mySTC_TYPE_DEFAULT},           //Default
    {mySTC_TYPE_COMMENT_LINE},      //Comment line
    {mySTC_TYPE_NUMBER},            //Number
    {mySTC_TYPE_STRING},            //String
    {mySTC_TYPE_STRING},         //Character
    {mySTC_TYPE_WORD1},             //Word 1
    {mySTC_TYPE_DEFAULT},           //Triple
    {mySTC_TYPE_DEFAULT},           //Triple double
    {mySTC_TYPE_DEFAULT},           //Class name
    {mySTC_TYPE_DEFAULT},           //Def name
    {mySTC_TYPE_OPERATOR},          //Operator
    {mySTC_TYPE_IDENTIFIER},        //Identifier
    {mySTC_TYPE_COMMENT_DOC},       //Comment block
    {mySTC_TYPE_STRING_EOL},        //String eol
    {mySTC_TYPE_WORD2},             //word2
    {mySTC_TYPE_PREPROCESSOR},      //Decorator


    {TERMINATE_TYPE},
};


LanguagePython::LanguagePython(wxStyledTextCtrl *sct)
:Language(sct)
{

}

void LanguagePython::InitializeSCT()
{

    m_sct->SetLexer(wxSTC_LEX_PYTHON);

    m_sct->SetProperty(wxT("fold"),wxT("1"));
    m_sct->SetProperty(wxT("fold.compact"),wxT("1"));
    m_sct->SetProperty(wxT("fold.quotes.python"),wxT("1"));
    m_sct->SetProperty(wxT("fold.comment.python"),wxT("1"));
    m_sct->SetProperty(wxT("fold.python.syntax.based"),wxT("1"));

    //Set styles
    int n = 0;
    while(true)
    {
        if(python_styles[n].type == TERMINATE_TYPE)
            break;

        const StyleInfo &cur_type = Styles[python_styles[n].type];
        wxFont font(cur_type.fontsize,wxMODERN,wxNORMAL,wxNORMAL,false,
                    cur_type.fontname);

        m_sct->StyleSetFont(n,font);

        m_sct->StyleSetForeground(n,wxColour(cur_type.foreground.red,cur_type.foreground.green,cur_type.foreground.blue));
        m_sct->StyleSetBackground(n,wxColour(cur_type.background.red,cur_type.background.green,cur_type.background.blue));

        m_sct->StyleSetBold(n,(cur_type.fontstyle & mySTC_STYLE_BOLD)>0);
        m_sct->StyleSetItalic(n,(cur_type.fontstyle & mySTC_STYLE_ITALIC)>0);
        m_sct->StyleSetUnderline(n,(cur_type.fontstyle & mySTC_STYLE_UNDERL)>0);
        m_sct->StyleSetVisible(n,(cur_type.fontstyle & mySTC_STYLE_HIDDEN)== 0);

        n++;
    }

    //Set The key words
    n = 0;
    while(true)
    {
        if(KeyWordListPython[n] == NULL)
            break;

        m_sct->SetKeyWords(n,KeyWordListPython[n]);
        n++;
    }

}


void LanguagePython::OnCharAdded(wxStyledTextEvent &event)
{

}


void LanguagePython::OnKeyDown(wxKeyEvent &event)
{

}

void LanguagePython::OnNewLine(wxStyledTextEvent &event)
{
    wxString prevLine = m_sct->GetLine(m_sct->GetCurrentLine()-1);
    if(prevLine.EndsWith(wxT(":\n")))
    {
        m_sct->InsertText(m_sct->GetCurrentPos(),wxT("\t"));
        //move the cursor to the end of the line
        m_sct->GotoPos(m_sct->GetLineEndPosition(m_sct->GetCurrentLine()));
    }
}
