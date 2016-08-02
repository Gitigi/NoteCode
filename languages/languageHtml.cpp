#include "../def.h"
#include "../language.h"

#include <string>
#include <wx/log.h>
#include <wx/string.h>
#include <wx/chartype.h>
#include <vector>
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

    if(event.GetKey() == '/' && m_sct->GetCharAt(m_sct->GetCurrentPos()-2)=='<')
    {
        int currentPos=m_sct->GetCurrentPos();
        wxString lastOpenTag;
        int tagPosition = GetOpenTag(lastOpenTag);
        if(!lastOpenTag.IsEmpty())
        {
            int currentLine = m_sct->GetCurrentLine();
            m_sct->InsertText(currentPos,lastOpenTag+">");

            //get the first character that is not a space in current line
            char firstChar=' ';
            int index = m_sct->PositionFromLine(currentLine);
            while(index<currentPos)
            {
                if(!isspace(m_sct->GetCharAt(index)))
                {
                    firstChar=m_sct->GetCharAt(index);
                    break;
                }
                index++;
            }
            //end of acquiring first character that is not a space

            if(firstChar == '<' && m_sct->GetCharAt(index+1) == '/')
            {
                int indentation = m_sct->GetLineIndentation(m_sct->LineFromPosition(tagPosition));
                m_sct->SetLineIndentation(currentLine,indentation);
            }
            m_sct->GotoPos(m_sct->GetLineEndPosition(currentLine));
        }
        return;
    }

    if(!m_sct->AutoCompActive())
    {
        wxString word_entered;
        GetWordBeforeCursor(word_entered);
        if(word_entered.length() > 1)
        {
            wxString list_of_options;
            allAutoComplete["html"].GenerateList(word_entered,list_of_options);
            if(!list_of_options.IsEmpty())
            {
                m_sct->AutoCompShow(word_entered.length(),list_of_options);
            }
        }
    }

}

void LanguageHtml::OnKeyDown(wxKeyEvent &event)
{

}

void LanguageHtml::findTag(const wxString &tagName,std::vector<std::pair<int,int>> &tagPos,int limit)
{
    int tagStart = 0;
    int tagEnd = 0;
    int maxPos = limit == -1 ? m_sct->GetTextLength() : limit;
    while(true)
    {
        tagStart = m_sct->FindText(tagStart,maxPos,"<"+tagName);
        if(tagStart == -1)
            break;
        tagEnd = m_sct->FindText(tagStart,maxPos,"</"+tagName);
        if(tagEnd != -1)
        {
            tagEnd = m_sct->FindText(tagEnd,maxPos,">");
        }
        tagPos.push_back(std::pair<int,int>(tagStart,tagEnd));
        if(tagEnd == -1)
            break;

        tagStart +=tagName.Length();
    }
}

void LanguageHtml::OnNewLine(wxStyledTextEvent &event)
{
    int currentLine = m_sct->GetCurrentLine();

    wxString tagName;
    int openTagLine = m_sct->LineFromPosition(GetOpenTag(tagName,m_sct->GetCurrentPos()));
    if(!tagName.IsEmpty())
    {
        int indentation = m_sct->GetLineIndentation(openTagLine) + m_sct->GetTabWidth();
        m_sct->SetLineIndentation(currentLine,indentation);
        m_sct->GotoPos(m_sct->GetLineEndPosition(currentLine));
    }
}

void LanguageHtml::OnCursorPositionChange()
{
    wxString tagName;
    GetOpenTag(tagName);
}

int LanguageHtml::GetOpenTag(wxString &tagName,int position,int limit)
{
    int currentPos = position == -1 ? m_sct->GetCurrentPos()-1 : position;

    int index=currentPos;
    int closedTag=0;

    std::vector<std::pair<int,int>> scriptPos;
    findTag("script",scriptPos,currentPos);
    int scriptVectorIndex = scriptPos.size()-1;
    while(true)
    {
        if(index < limit)
            break;
        if(scriptVectorIndex != -1)
        {
            std::pair<int,int> &scriptLoc = scriptPos[scriptVectorIndex];
            if(scriptLoc.second == -1)
            {
                if(scriptLoc.first>=limit)
                {
                    tagName = "script";
                    return scriptLoc.first;
                }
                else
                {
                    return -1;
                }
            }
            if(index >= scriptLoc.first && index <= scriptLoc.second)
            {
                index = scriptLoc.first -1;
                scriptVectorIndex--;
            }

        }
        if(m_sct->GetCharAt(index) == '>')
        {
            index--;
            while(true)
            {
                if(index < 0)
                    break;
                if(m_sct->GetCharAt(index)=='/' && m_sct->GetCharAt(index-1)=='<')
                {
                    closedTag++;
                    index-=1;
                    break;
                }
                else if(m_sct->GetCharAt(index)=='<')
                {
                    int charPos = index + 1;
                    while(true)
                    {
                        if(m_sct->GetCharAt(charPos) == ' '||m_sct->GetCharAt(charPos)=='>'
                           ||m_sct->GetCharAt(charPos)=='\n')
                            break;
                        tagName.Append((wxUniChar)m_sct->GetCharAt(charPos));
                        charPos++;
                    }
                    auto iter = allAutoComplete["html"].GetMap().find(tagName);
                    if(closedTag == 0)
                    {
                        if(iter != allAutoComplete["html"].GetMap().end())
                        {
                            if(iter->second.type == TYPE_OPENTAG)
                            {
                                tagName.Clear();
                            }
                            else
                                return index;
                        }
                        else
                        {
                            return index;
                        }
                    }
                    else
                    {
                        if(iter != allAutoComplete["html"].GetMap().end())
                        {
                            if(iter->second.type == TYPE_OPENTAG)
                            {
                                tagName.Clear();
                            }
                            else
                            {
                                tagName.Clear();
                                closedTag--;
                            }
                        }
                        else
                        {
                            tagName.Clear();
                            closedTag--;
                        }
                    }
                    break;
                }
                index--;
            }
        }
        else if(m_sct->GetCharAt(index)=='<' && !(m_sct->GetCharAt(index+1)=='/'))
        {
            int charPos = index + 1;
            while(true)
            {
                if(m_sct->GetCharAt(charPos) == ' '||m_sct->GetCharAt(charPos)=='>'||m_sct->GetCharAt(charPos)=='\n')
                    break;
                tagName.Append((wxUniChar)m_sct->GetCharAt(charPos));
                charPos++;
            }
            return index-1;
        }
        index--;
    }
    return -1;
}


int LanguageHtml::GetNumOpenTag(int position,int limit)
{
    int limitPos = m_sct->PositionFromLine(limit);
    int tagCount=0;
    wxString tagName;

    while(true)
    {
        if(position == -1)
            break;
        position = GetOpenTag(tagName,position,limitPos)-1;
        if(tagName.IsEmpty())
            break;
        else
        {
            tagCount++;
            tagName.clear();
        }
    }
    return tagCount;
}

void LanguageHtml::GetEnclosingTag(wxString &tagName,int position)
{
    position = position == -1 ? m_sct->GetCurrentPos()  : position;

    int scriptStart = m_sct->FindText(0,position,"<script");
    int scriptEnd;
    if(scriptStart == -1)
    {
        GetOpenTag(tagName,position);
    }
    else
    {
        scriptEnd = m_sct->FindText(scriptStart,m_sct->GetTextLength(),"</script");
        if(position > scriptStart && position <= scriptEnd)
        {
            tagName = "script";
        }
        else
        {
            GetOpenTag(tagName);
        }
    }
}
