#include "../def.h"
#include "../language.h"
#include "../edit.h"

static lang_style css_styles[] = {
    {mySTC_TYPE_DEFAULT},       //Default
    {mySTC_TYPE_WORD1},         //Tag
    {mySTC_TYPE_WORD1},         //Class
    {mySTC_TYPE_WORD1},         //Psedoclass
    {mySTC_TYPE_PREPROCESSOR},  //Unknown Psedoclasss
    {mySTC_TYPE_OPERATOR},      //Operator
    {mySTC_TYPE_IDENTIFIER},    //Identifier
    {mySTC_TYPE_IDENTIFIER},    //Unknown Identifier
    {mySTC_TYPE_VALUE},         //Value
    {mySTC_TYPE_COMMENT},       //Comment
    {mySTC_TYPE_WORD1},         //Id
    {mySTC_TYPE_WORD1},         //Important
    {mySTC_TYPE_DEFAULT},       //Directive
    {mySTC_TYPE_STRING},        //Double string
    {mySTC_TYPE_STRING},        //Single string
    {mySTC_TYPE_WORD1},         //Identifier 2
    {mySTC_TYPE_WORD5},         //Attribute
    {mySTC_TYPE_WORD1},         //Identifier 3
    {mySTC_TYPE_WORD1},         //Pseudoelement
    {mySTC_TYPE_WORD1},         //Extended identifier
    {mySTC_TYPE_WORD1},         //Extended pseudoclass
    {mySTC_TYPE_WORD1},         //Extended pseudoelement
    {mySTC_TYPE_WORD1},       //Media
    {mySTC_TYPE_DEFAULT},       //Variable

    {TERMINATE_TYPE},
    };

LanguageCss::LanguageCss(Edit *sct)
:Language(sct)
{
    commentSym = wxT("//");
}

void LanguageCss::InitializeSCT()
{
    m_sct->SetLexer(wxSTC_LEX_CSS);

    m_sct->SetProperty(wxT("fold.comment"),wxT("1"));
    m_sct->SetProperty(wxT("fold.compact"),wxT("1"));

    //Set styles
    int n = 0;
    while(true)
    {
        if(css_styles[n].type == TERMINATE_TYPE)
            break;

        const StyleInfo &cur_type = Styles[css_styles[n].type];

        m_sct->StyleSetForeground(n,wxColour(cur_type.foreground.red,cur_type.foreground.green,cur_type.foreground.blue));
        m_sct->StyleSetBackground(n,wxColour(cur_type.background.red,cur_type.background.green,cur_type.background.blue));

        m_sct->StyleSetBold(n,(cur_type.fontstyle & mySTC_STYLE_BOLD)>0);
        m_sct->StyleSetItalic(n,(cur_type.fontstyle & mySTC_STYLE_ITALIC)>0);
        m_sct->StyleSetUnderline(n,(cur_type.fontstyle & mySTC_STYLE_UNDERL)>0);
        m_sct->StyleSetVisible(n,(cur_type.fontstyle & mySTC_STYLE_HIDDEN)== 0);

        n++;
    }
}

void LanguageCss::OnCharAdded(wxStyledTextEvent &event)
{
    Language::OnCharAdded(event);
}

void LanguageCss::OnKeyDown(wxKeyEvent &event)
{

}
