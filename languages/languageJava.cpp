#include "../def.h"
#include "../language.h"
#include "../edit.h"

static const char* JavaWordlist1 =
    "bool break case catch char class const const_cast "
    "continue default delete do double dynamic_cast else enum explicit "
    "export extern false float for friend goto if inline int long "
    "mutable namespace new operator private protected public register "
    "reinterpret_cast return short signed sizeof static static_cast "
    "struct switch template this throw true try typedef typeid "
    "typename union unsigned using virtual void volatile wchar_t "
    "while cout std cin endl ";
static const char* JavaWordlist2 =
    "file";
static const char* JavaWordlist3 =
    "a addindex addtogroup anchor arg attention author b brief bug c "
    "class code date def defgroup deprecated dontinclude e em endcode "
    "endhtmlonly endif endlatexonly endlink endverbatim enum example "
    "exception f$ f[ f] file fn hideinitializer htmlinclude "
    "htmlonly if image include ingroup internal invariant interface "
    "latexonly li line link mainpage name namespace nosubgrouping note "
    "overload p page par param post pre ref relates remarks return "
    "retval sa section see showinitializer since skip skipline struct "
    "subsection test throw todo typedef union until var verbatim "
    "verbinclude version warning weakgroup $ @ \"\" & < > # { }";


static const char *KeyWordListJava[] = {
    JavaWordlist1,
    JavaWordlist2,
    JavaWordlist3,
    NULL,
};


static lang_style java_styles[] = {
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

LanguageJava::LanguageJava(Edit *sct)
:Language(sct)
{

}

void LanguageJava::InitializeSCT()
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
        if(java_styles[n].type == TERMINATE_TYPE)
            break;

        const StyleInfo &cur_type = Styles[java_styles[n].type];

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

void LanguageJava::OnCharAdded(wxStyledTextEvent &event)
{
    AutoCompBraces();
    StyleBraces();
}

void LanguageJava::OnKeyDown(wxKeyEvent &event)
{

}

void LanguageJava::OnNewLine(wxStyledTextEvent &event)
{
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
