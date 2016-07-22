#include "../def.h"
#include "../language.h"

#include <string>
#include <wx/log.h>
#include <wx/string.h>
#include <wx/chartype.h>
#include <exception>
using namespace std;

static wxString autocopmplet = wxT("html> input textarea> text_input>");

static lang_style html_styles[] =
{
    {mySTC_TYPE_DEFAULT},           //Default
    {mySTC_TYPE_WORD1},             //HTML Tags
    {mySTC_TYPE_WORD2},             //Unknown Html tags
    {mySTC_TYPE_COMMENT_LINE},      //Attributes
    {mySTC_TYPE_PREPROCESSOR},      //Unknown Attribute
    {mySTC_TYPE_NUMBER},            //Number
    {mySTC_TYPE_STRING},            //Double string
    {mySTC_TYPE_STRING},            //Single string
    {mySTC_TYPE_DEFAULT},           //Other
    {mySTC_TYPE_COMMENT},           //Comment
    {mySTC_TYPE_DEFAULT},           //entity
    {mySTC_TYPE_ERROR},             //Target
    {mySTC_TYPE_WORD1},             //XML start
    {mySTC_TYPE_WORD2},             //XML end
    {mySTC_TYPE_SCRIPT},            //Script
    {mySTC_TYPE_DEFAULT},           //ASP
    {mySTC_TYPE_DEFAULT},           //ASPAT
    {mySTC_TYPE_DEFAULT},           //CDATA
    {mySTC_TYPE_DEFAULT},           //Question
    {mySTC_TYPE_DEFAULT},             //Value

    {TERMINATE_TYPE},
};


LanguageHtml::LanguageHtml(wxStyledTextCtrl *sct)
:Language(sct)
{
    if(allAutoComplete.find("html") == allAutoComplete.end())
    {
        allAutoComplete["html"] = AutoCompList();
        allAutoComplete["html"].InsertWordsFromJsonFile("html");
    }
}


void LanguageHtml::InitializeSCT()
{

    m_sct->SetLexer(wxSTC_LEX_HTML);

    //Set fold properties
    m_sct->SetProperty(wxT("fold"),wxT("1"));
    m_sct->SetProperty(wxT("fold.html"),wxT("1"));
    m_sct->SetProperty(wxT("fold.html.preprocessor"),wxT("1"));
    m_sct->SetProperty(wxT("fold.compact"),wxT("1"));
    m_sct->SetProperty(wxT("fold.hypertext.comment"),wxT("1"));
    m_sct->SetProperty(wxT("fold.hypertext.heredoc"),wxT("1"));

    m_sct->SetProperty(wxT("lexer"),wxT("1"));
    m_sct->SetProperty(wxT("lexer.xml.allow.scripts"),wxT("1"));
    m_sct->SetProperty(wxT("lexer.xml.allow.scripts"),wxT("1"));
    m_sct->SetProperty(wxT("lexer.html.mako"),wxT("1"));
    m_sct->SetProperty(wxT("lexer.html.django"),wxT("1"));

    //Set styles
    int n = 0;
    while(true)
    {
        if(html_styles[n].type == TERMINATE_TYPE)
            break;

        const StyleInfo &cur_type = Styles[html_styles[n].type];

        m_sct->StyleSetForeground(n,wxColour(cur_type.foreground.red,cur_type.foreground.green,cur_type.foreground.blue));
        m_sct->StyleSetBackground(n,wxColour(cur_type.background.red,cur_type.background.green,cur_type.background.blue));

        m_sct->StyleSetBold(n,(cur_type.fontstyle & mySTC_STYLE_BOLD)>0);
        m_sct->StyleSetItalic(n,(cur_type.fontstyle & mySTC_STYLE_ITALIC)>0);
        m_sct->StyleSetUnderline(n,(cur_type.fontstyle & mySTC_STYLE_UNDERL)>0);
        m_sct->StyleSetVisible(n,(cur_type.fontstyle & mySTC_STYLE_HIDDEN)== 0);

        n++;

    }

}


void LanguageHtml::OnCharAdded(wxStyledTextEvent &event)
{
    Language::OnCharAdded(event);
    if(!m_sct->AutoCompActive())
    {
        wxString word_entered;
        wxString list_of_options;
        GetWordBeforeCursor(word_entered);
        if(!word_entered.IsEmpty())
        {
            allAutoComplete["html"].GenerateList(word_entered,list_of_options);
        }
        if(!list_of_options.IsEmpty())
        {
            m_sct->AutoCompShow(word_entered.length(),list_of_options);
        }
    }

}

void LanguageHtml::OnKeyDown(wxKeyEvent &event)
{

}
