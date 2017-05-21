#include "../def.h"
#include "../language.h"
#include "../edit.h"
#include <iostream>
using std::cerr;
using std::endl;

static const char *JsWordlist1 =
    "var class function document ";

static const char *KeyWordListJava[] = {
    JsWordlist1,
    NULL,
};


static lang_style js_styles[] = {
    {mySTC_TYPE_DEFAULT},       //Default
    {mySTC_TYPE_COMMENT},       //Comment
    {mySTC_TYPE_COMMENT_LINE},  //CommentLine
    {mySTC_TYPE_COMMENT_DOC},   //CommentDoc
    {mySTC_TYPE_NUMBER},        //Number
    {mySTC_TYPE_WORD1},         //word 1
    {mySTC_TYPE_STRING},        //String
    {mySTC_TYPE_CHARACTER},     //Character
    {mySTC_TYPE_UUID},          //UUID
    {mySTC_TYPE_PREPROCESSOR},  //Preprocessor
    {mySTC_TYPE_OPERATOR},      //Operator
    {mySTC_TYPE_IDENTIFIER},    //Identifier
    {mySTC_TYPE_STRING_EOL},    //String eol,
    {mySTC_TYPE_DEFAULT},       //Verbatim
    {mySTC_TYPE_REGEX},         //Regex
    {mySTC_TYPE_COMMENT_DOC},   //Comment Line Doc
    {mySTC_TYPE_WORD2},         //Words 2
    {mySTC_TYPE_WORD3},         //Comment key words
    {mySTC_TYPE_ERROR},         //Comments error key words
    {mySTC_TYPE_LABEL},    //Global class
    {mySTC_TYPE_STRING},        //String
    {mySTC_TYPE_DEFAULT},       //Triple verbatim
    {mySTC_TYPE_SCRIPT},        //Quoted string
    {mySTC_TYPE_COMMENT_SPECIAL},//Preprocessor comment

    {TERMINATE_TYPE}        //End style list
};


LanguageJS::LanguageJS(Edit *sct)
:Language(sct)
{
    commentSym = wxT("//");
}

void LanguageJS::InitializeSCT()
{
    m_sct->SetLexer(wxSTC_LEX_CPP);

    //Set folding property
    m_sct->SetProperty(wxT("fold"),wxT("1"));
    m_sct->SetProperty(wxT("fold.comment"),wxT("1"));
    m_sct->SetProperty(wxT("fold.compact"),wxT("1"));
    m_sct->SetProperty(wxT("fold.preprocessor"),wxT("1"));
    m_sct->SetProperty(wxT("fold.cpp.syntax.based"),wxT("1"));
    m_sct->SetProperty(wxT("fold.cpp.explicit.anywhere"),wxT("1"));
    m_sct->SetProperty(wxT("fold.at.else"),wxT("1"));


    //Set Style
    int n = 0;
    while(true)
    {
        if(js_styles[n].type == TERMINATE_TYPE)
            break;

        const StyleInfo &cur_type = Styles[js_styles[n].type];

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
            if(KeyWordListJava[n] == NULL)
                break;
            m_sct->SetKeyWords(n,KeyWordListJava[n]);
            n++;
        }

}

void LanguageJS::OnCharAdded(wxStyledTextEvent &event)
{
    Language::AutoCompBraces();
    //Language::StyleBraces();
}

void LanguageJS::OnKeyDown(wxKeyEvent &event)
{

}

void LanguageJS::OnNewLine(wxStyledTextEvent &event)
{
    cerr<<"Open brace pos = "<<GetOpenBrace(m_sct->GetCurrentPos()-1,'{')<<endl;
    cerr<<"Open brace line = "<<m_sct->LineFromPosition(GetOpenBrace(m_sct->GetCurrentPos()-1,'{'))<<endl;

    char charBeforeNew = m_sct->GetCharAt(m_sct->GetCurrentPos()-2);
    int currentLine = m_sct->GetCurrentLine();
    if(charBeforeNew == ':')
    {
        int indentation = m_sct->GetLineIndentation(currentLine-1);
        m_sct->SetLineIndentation(currentLine-1,indentation-m_sct->GetTabWidth());
        m_sct->SetLineIndentation(currentLine,indentation == 0 ? m_sct->GetTabWidth() : indentation);
        m_sct->GotoPos(m_sct->GetLineEndPosition(currentLine));
    }
    else
    {
        AutoIndentWithBrace(currentLine);
    }
    AlignBraceAfterEnter();
}
