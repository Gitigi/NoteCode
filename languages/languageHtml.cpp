#include "../def.h"
#include "../language.h"
#include "../edit.h"

#include "parser/cssParser.hpp"

#include <string>
#include <cstring>
#include <sstream>
#include <wx/log.h>
#include <wx/string.h>
#include <wx/chartype.h>
#include <wx/filesys.h>
#include <wx/process.h>
#include <wx/stdstream.h>
#include <wx/sstream.h>
#include <wx/mstream.h>

#include <vector>
#include <exception>
#include <iostream>
#include <algorithm>
#include <iterator>

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>
#include <boost/spirit/include/support_line_pos_iterator.hpp>
#include <boost/fusion/view/iterator_range.hpp>
#include <boost/fusion/include/iterator_range.hpp>
#include <boost/spirit/include/classic_position_iterator.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>
#include <type_traits>

using namespace std;

#define SKIP_SPACE(pos) {   \
                        int textLength = m_sct->GetTextLength(); \
                        while(pos < textLength)         \
                        {                   \
                            if(!isspace(m_sct->GetCharAt(pos)))     \
                                break;                  \
                            pos++;                      \
                        }               \
                        }
#define SKIP_SPACE_STRING(text,pos) { while(pos<text.length()){if(!isspace(text[pos]))break;pos++;}}

#define GET_WORD_STRING(source,dest,index) {\
                                    while(index<source.length())\
                                    {\
                                        if(!isalnum(source[index]))\
                                            break;  \
                                        dest.append(source[index]);\
                                        index++;\
                                    }\
                                    }
#define GET_STRING(source,dest,index){\
            char quote = source[index];\
            if(quote=='"' || quote=='\'')\
            {\
            index+=1;\
            while(index<source.length()&&source[index]!=quote)\
            {\
                dest.append(source[index]);\
                index++;\
            }\
            index++;\
            }\
            }

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


LanguageHtml::LanguageHtml(Edit *sct)
:Language(sct),currentLang(FILE_HTML),langCss(sct),langJs(sct),dirty(false),insertionOccured(false)
{
    if(allAutoComplete.find("html") == allAutoComplete.end())
    {
        allAutoComplete["html"] = AutoCompList();
        allAutoComplete["html"].InsertWordsFromJsonFile("html");
    }
    scope.push_back("html");
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
    switch(currentLang)
    {
    case FILE_HTML:
        OnCharAddedHtml(event);
        break;
    case FILE_CSS:
        langCss.OnCharAdded(event);
        break;
    case FILE_JAVASCRIPT:
        langJs.OnCharAdded(event);
        break;
    }
}

void LanguageHtml::OnModification(wxStyledTextEvent &event)
{
    switch(currentLang)
    {
    case FILE_HTML:
        OnModificationHtml(event);
        break;
    case FILE_CSS:
        langCss.OnModification(event);
        break;
    case FILE_JAVASCRIPT:
        langJs.OnModification(event);
        break;
    }
}

void LanguageHtml::OnNewLine(wxStyledTextEvent &event)
{
    switch(currentLang)
    {
    case FILE_HTML:
        OnNewLineHtml(event);
        break;
    case FILE_CSS:
        langCss.OnNewLine(event);
        break;
    case FILE_JAVASCRIPT:
        langJs.OnNewLine(event);
        break;
    }
}

void LanguageHtml::OnCursorPositionChange()
{
    switch(currentLang)
    {
    case FILE_HTML:
        OnCursorPositionChangeHtml();
        break;
    case FILE_CSS:
        langCss.OnCursorPositionChange();
        break;
    case FILE_JAVASCRIPT:
        langJs.OnCursorPositionChange();
        break;
    }
}

void LanguageHtml::OnCharAddedHtml(wxStyledTextEvent &event)
{
    Language::StyleBraces();

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
            std::vector<wxString> vectorList;
            for(const auto &i : scope)
            {
                allAutoComplete[i].GenerateList(word_entered,vectorList);
            }
            std::sort(vectorList.begin(),vectorList.end());
            for(const auto &word : vectorList)
            {
                list_of_options.Append(word+" ");
            }
            if(!list_of_options.IsEmpty())
            {
                m_sct->AutoCompShow(word_entered.Length(),list_of_options);
            }
        }
    }
    if(m_sct->GetCharAt(m_sct->GetCurrentPos()-1)=='>')
    {
        EvaluateDirty();
        wxString tagContent;
        GetTagContent(tagContent,m_sct->GetCurrentPos()-1);
        if(!tagContent.IsEmpty())
        {
            std::map<wxString,wxString> attr;
            wxString tagName = ParseTagContent(tagContent,attr);
            EvalTagContent(tagName,attr);
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

void LanguageHtml::OnNewLineHtml(wxStyledTextEvent &event)
{
    int currentLine = m_sct->GetCurrentLine();

    wxString tagName;
    int openTagLine = m_sct->LineFromPosition(GetOpenTag(tagName,m_sct->GetCurrentPos()));
    cerr<<"html new called with = "<<tagName<<endl;
    if(!tagName.IsEmpty())
    {
        int indentation = m_sct->GetLineIndentation(openTagLine) + m_sct->GetTabWidth();
        m_sct->SetLineIndentation(currentLine,indentation);
        cerr<<"indentation = "<<indentation<<"line number = "<<currentLine<<endl;
        m_sct->GotoPos(m_sct->GetLineEndPosition(currentLine));
    }
}

void LanguageHtml::OnCursorPositionChangeHtml()
{
    //wxString tagName;
    //GetOpenTag(tagName);
    EvaluateDirty();
}

void LanguageHtml::OnLoad()
{
    EvaluateHtml(0,m_sct->GetTextLength());
}

void LanguageHtml::OnUpdateUI(wxStyledTextEvent &event)
{
    wxString currentTag;
    GetOpenTag2(currentTag);
    if(currentTag == "script")
    {
        langJs.InitializeSCT();
        currentLang = FILE_JAVASCRIPT;
    }
    else if(currentTag == "style")
    {
        langCss.InitializeSCT();
        currentLang = FILE_CSS;
    }
    else
    {
        InitializeSCT();
        currentLang = FILE_HTML;
    }
}

void LanguageHtml::OnModificationHtml(wxStyledTextEvent &event)
{
    switch(event.GetModificationType())
    {
    case 1040:  //About to insert Text
        break;
    case 8209:  //New insertion Point
        {
            if(InsideOpenTag(event.GetPosition()))
            {
                if(!dirty)
                {
                    dirty = true;
                    dirtyType = 1;
                    dirtyStartPos = event.GetPosition();
                    dirtyEndPos = dirtyStartPos+event.GetLength()-1;
                }
                if(dirty && dirtyType == 0)
                {
                    if(insertionOccured)
                    {
                        insertionEnd = event.GetPosition();
                    }
                    else
                    {
                        insertionOccured = true;
                        insertionStart = event.GetPosition();
                        insertionEnd = insertionStart+event.GetLength()-1;
                    }
                }
            }
            else if(dirty == true && dirtyType == 0)
            {
                if(insertionOccured)
                {
                    insertionEnd = event.GetPosition();
                }
                else
                {
                    insertionOccured = true;
                    insertionStart = event.GetPosition();
                    insertionEnd = insertionStart+ event.GetLength()-1;
                }
            }
            else if(dirty && dirtyType == 1)
            {
                EvaluateDirty();
                dirty = true;
                dirtyType = 1;
                dirtyStartPos = event.GetPosition();
                dirtyEndPos = dirtyStartPos;
            }

            else
            {
                EvaluateHtml(event.GetPosition(),event.GetPosition()+event.GetLength());
            }
            break;
        }
    case 17:    //Continued text insertion
        {
            if(dirty && dirtyType == 1)
            {
                dirtyEndPos = event.GetPosition();
            }
            else if(dirty && dirtyType == 0)
            {
                insertionEnd = event.GetPosition();
            }
            break;
        }
    case 2064:  //About to delete Text
        {
            int _begin = event.GetPosition();
            int _end = _begin+event.GetLength()-1;
            if((!InsideOpenTag(_begin) && InsideOpenTag(_end))||(!InsideOpenTag(_end)&&InsideOpenTag(_begin))||
               (_begin==_end && (m_sct->GetCharAt(_begin) == '<' || m_sct->GetCharAt(_begin)=='>')))
            {
                if(!dirty)
                {
                    dirty = true;
                    dirtyType = -1;
                    dirtyStartPos = event.GetPosition();
                    dirtyEndPos = dirtyStartPos;
                    m_text.insert(0,m_sct->GetTextRange(dirtyStartPos,dirtyStartPos+event.GetLength()));
                }
            }
            break;
        }
    case 8210:  //New text delete
        {
            if(InsideOpenTag(event.GetPosition()) && !dirty)
            {
                dirty = true;
                dirtyType = 0;
                dirtyStartPos = event.GetPosition();
                dirtyEndPos = dirtyStartPos;
                m_text.insert(0,event.GetText());
            }
            else if(dirty == true && dirtyType == 1)
            {
                dirtyEndPos-=1;
                if(dirtyEndPos < dirtyStartPos)
                {
                    dirty = false;
                }
            }
            else if(dirty && dirtyType == 0)
            {
                EvaluateDirty();
                dirty = true;
                dirtyType = 0;
                dirtyStartPos = event.GetPosition();
                dirtyEndPos = dirtyStartPos;
                m_text.insert(0,event.GetText());
            }
            else if(dirty && dirtyType == -1)
            {
                EvaluateDirty();
            }
            else
            {
                UnEvaluateHtml(event.GetText());
            }
            break;
        }
    case 18:    //Continued text delete
        {
            if(dirty && dirtyType == 0)
            {
                if(!insertionOccured)
                {
                    m_text.insert(0,event.GetText());
                    dirtyEndPos = event.GetPosition();
                }
                else
                {
                    insertionEnd-=1;
                    if(insertionEnd < insertionStart)
                    {
                        insertionOccured = false;
                    }
                }
            }
            else if(dirty && dirtyType == 1)
            {
                dirtyEndPos-=1;
                if(dirtyEndPos < dirtyStartPos)
                {
                    dirty = false;
                }
            }
            else if(dirty && dirtyType == -1)
            {
                EvaluateDirty();
            }
            break;
        }
    default:
        cerr<<event.GetModificationType()<<endl;
    }
}

void LanguageHtml::EvaluateDirty()
{
    if(dirty==true)
    {
        if(dirtyType == 1)
        {
            int textStart = 0;
            //find <
            int index = dirtyStartPos;
            while(true)
            {
                if(index < 0)
                    break;
                if(m_sct->GetCharAt(index) == '<')
                {
                    textStart = index;
                    break;
                }
                index--;
            }

            wxString text;
            text = m_sct->GetTextRange(textStart,dirtyStartPos);
            text.Append(m_sct->GetTextRange(dirtyEndPos+1,m_sct->FindText(dirtyEndPos,m_sct->GetTextLength(),">")+1));
            cerr<<"Previous tag = "<<text<<endl;
            UnEvaluateHtml(text);
            cerr<<"Current tag = "<<m_sct->GetTextRange(textStart,m_sct->FindText(dirtyEndPos,m_sct->GetTextLength(),">")+1)<<endl;
            EvaluateHtml(textStart,m_sct->FindText(dirtyEndPos,m_sct->GetTextLength(),">")+1);
        }
        else
        {
            int textStart = 0;

            //find <
            int index = dirtyEndPos-1;
            while(true)
            {
                if(index < 0)
                    break;
                if(m_sct->GetCharAt(index) == '<')
                {
                    textStart = index;
                    break;
                }
                else if(m_sct->GetCharAt(index) == '>')
                {
                    textStart = dirtyEndPos;
                    break;
                }
                index--;
            }

            wxString text = m_sct->GetTextRange(textStart,dirtyEndPos);
            text.Append(m_text);
            if(insertionOccured)
            {
                dirtyEndPos=insertionEnd+1;
            }

            text.Append(m_sct->GetTextRange(dirtyEndPos,m_sct->FindText(dirtyEndPos,m_sct->GetTextLength(),">")+1));
            cerr<<"Previous tag = "<<text<<endl;
            UnEvaluateHtml(text);
            cerr<<"Current current = "<<m_sct->GetTextRange(textStart,m_sct->FindText(dirtyEndPos,m_sct->GetTextLength(),">")+1)<<endl;
            EvaluateHtml(textStart,m_sct->FindText(dirtyEndPos,m_sct->GetTextLength(),">")+1);
        }
        dirty = false;
        insertionOccured = false;
        m_text.Clear();
    }
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
            while(charPos < currentPos)
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

void LanguageHtml::GetTagContent(wxString &tagContent,int position)
{
    if(m_sct->GetCharAt(position) != '>')
        return;
    int index = position-1;

    //skip '>' found within script tag
    std::vector<std::pair<int,int>> scriptPos;
    findTag("script",scriptPos,position);
    for(const auto &i : scriptPos)
    {
        if(m_sct->FindText(i.first,m_sct->GetTextLength(),">") == position)
        {
            break;
        }
        else if(position > i.first && position < i.second)
        {
            return;
        }
        else if(i.second == -1)
            return;
    }
    //end of skip

    while(index >= 0)
    {
        char character = m_sct->GetCharAt(index);
        if(character == '<')
            break;
        tagContent.insert(0,(wxUniChar)m_sct->GetCharAt(index));
        index--;
    }
}

wxString LanguageHtml::ParseTagContent(const wxString &content,std::map<wxString,wxString>&attr)
{
    size_t pos = 0;
    SKIP_SPACE_STRING(content,pos);
    wxString tagName;
    GET_WORD_STRING(content,tagName,pos);
    wxString attribute;
    wxString value;
    while(pos<content.length())
    {
        SKIP_SPACE_STRING(content,pos)
        GET_WORD_STRING(content,attribute,pos);
        SKIP_SPACE_STRING(content,pos);
        if(content[pos] == '=')
        {
            pos+=1;
            SKIP_SPACE_STRING(content,pos);
            GET_STRING(content,value,pos);
        }
        else
        {
            value = wxEmptyString;
        }
        attr[attribute] = value;
        attribute.Clear();
        value.Clear();

        if(!isalnum(content[pos]))
            pos++;
    }
    return tagName;
}

class MyProcess : public wxProcess
{
public:
    MyProcess(){Redirect();}
    void OnTerminate(int pid,int status)
    {
        cerr<<"process terminated"<<endl;
        wxStdInputStream output(*GetInputStream());
        output.unsetf(std::ios::skipws);

        std::vector<css::ast::cssUnit>cssAst;
        css::ParserCss(output,cssAst);
        auto loader = [&](const string &name){allAutoComplete["html"].InsertWord(name);};
        css::LoadClassAndId classAndId(loader);
        classAndId(cssAst);

        wxStdInputStream error(*GetErrorStream());
        string errorStr;
        error>>errorStr;
        cerr<<errorStr;
    }
};

void LanguageHtml::ResolveFileLoc(wxString &fileName)
{
    if(fileName.StartsWith("www") || fileName.StartsWith("http"))
        return;
    wxString dir = wxFileName(m_sct->filePath).GetPath();
    wxString fullPath;
    fullPath.Printf("%s/%s",dir,fileName);
    wxFileName file(fullPath);
    file.MakeAbsolute();
    fileName = file.GetFullPath();
}

void LanguageHtml::EvalTagContent(const wxString &tagName,const std::map<wxString,wxString>&attr)
{
    if(tagName == "style")
    {
        auto iter = attr.find("link");
        if(iter!=attr.end())
        {
            if(iter->second.StartsWith("https"))
            {

                wxArrayString output,error;
                wxString cmd;
                cmd.Printf("python \"script/fileLoader.py\" \"%s\"",iter->second);
                wxExecute(cmd.c_str(),wxEXEC_ASYNC,new MyProcess);
            }
            else
            {
                wxString fileFullPath = iter->second;
                ResolveFileLoc(fileFullPath);

                if(allAutoComplete.find(fileFullPath)==allAutoComplete.end())
                {

                    allAutoComplete[fileFullPath] = AutoCompList();
                    scope.push_back(fileFullPath);

                    wxFileSystem fileOpener;
                    wxFSFile *openFile = fileOpener.OpenFile(fileFullPath);

                    if(openFile)
                    {
                        wxStdInputStream fileStream(*(openFile->GetStream()));
                        std::vector<css::ast::cssUnit>cssAst;
                        css::ParserCss(fileStream,cssAst);
                        auto loader = [&](const string &name){allAutoComplete[fileFullPath].InsertWord(name);};
                        css::LoadClassAndId classAndId(loader);
                        classAndId(cssAst);
                    }
                    else
                    {
                        cerr<<"Unable to open: "<<fileFullPath<<endl;
                    }


                }
            }
        }
    }
}

void LanguageHtml::UnEvalTagContent(const wxString &tagName,const std::map<wxString,wxString> &attr)
{
    if(tagName == "style")
    {
        auto key = attr.find("link");
        if(key!=attr.end())
        {
            wxString styleLoc = key->second;
            ResolveFileLoc(styleLoc);
            auto object = allAutoComplete.find(styleLoc);
            cerr<<"location = "<<styleLoc<<endl;
            if(object != allAutoComplete.end())
            {
                allAutoComplete.erase(object);
                auto result = std::find(scope.begin(),scope.end(),styleLoc);
                if(result != scope.end())
                    scope.erase(result);
            }
            if(allAutoComplete.find(styleLoc)!=allAutoComplete.end())
                cerr<<"Object still exist"<<endl;
            else
                cerr<<"Object deleted"<<endl;
        }
    }
}

void LanguageHtml::GetWordBeforeCursor(wxString &destination)
{
    int index = m_sct->GetCurrentPos()-1;
    while(index >= 0)
    {
        char character = m_sct->GetCharAt(index);
        if(isalnum(character)||character=='_'||character=='-'||character=='$')
        {
            destination.insert(0,(wxUniChar)m_sct->GetCharAt(index));
            index--;
        }
        else
        {
            break;
        }
    }
}

bool LanguageHtml::InsideOpenTag(int position)
{
    std::vector<std::pair<int,int>> scriptPos;
    findTag("script",scriptPos,position);
    if(scriptPos.size()>0)
    {
        std::pair<int,int>lastScript = scriptPos[scriptPos.size()-1];
        if(position > lastScript.first)
        {
            if(lastScript.second == -1)
            {
                int closeBrace = m_sct->FindText(lastScript.first,m_sct->GetTextLength(),">");
                if(position <= closeBrace)
                {
                    return true;
                }
                else
                    return false;
            }
        }
    }

    if((m_sct->GetCharAt(position)) == '>')
    {
        position -= 1;
    }
    else if(m_sct->GetCharAt(position)=='<')
    {
        position +=1;
    }

    int openTag=-1;
    int closeTag=-1;
    int index = position;

    while(true)
    {
        if(index < 0)
            break;
        else if(m_sct->GetCharAt(index) == '/' && m_sct->GetCharAt(index-1)=='<')
        {
            break;
        }
        else if(m_sct->GetCharAt(index) == '>')
        {
            break;
        }
        else if(m_sct->GetCharAt(index) == '<' && m_sct->GetCharAt(index+1)!='/')
        {
            openTag=index;
            break;
        }
        else
        {
            index--;
        }
    }
    index = position;
    while(true)
    {
        if(index > m_sct->GetTextLength())
        {
            break;
        }
        else if(m_sct->GetCharAt(index)=='<')
        {
            break;
        }
        else if(m_sct->GetCharAt(index)=='>')
        {
            closeTag = index;
            break;
        }
        else
        {
            index++;
        }
    }
    if(openTag != -1 && closeTag != -1)
    {
        return true;
    }
    else
        return false;
}


void LanguageHtml::FindTags(std::vector<std::pair<int,int>> &pos,int start,int end)
{
    char currentChar;
    wxString tagName;
    std::pair<int,int>newPos;
    while(start < end)
    {
        start = m_sct->FindText(start,end,"<");
        if(start == -1)
            break;
        if(m_sct->GetCharAt(start+1)=='/')
        {
            start+=2;
            continue;
        }

        else
        {
            newPos.first = start;
            start+=1;
            while(true)
            {
                currentChar = m_sct->GetCharAt(start);
                if(currentChar == ' '||currentChar=='>'
                               ||currentChar=='\n'||currentChar ==0)
                               break;
                tagName.Append(currentChar);
                start+=1;
            }
            int rBracket = m_sct->FindText(start,end,"<");
            start = m_sct->FindText(start,end,">");
            if(rBracket != -1 && start > rBracket)
            {
                start = -1;
            }
            newPos.second = start;
            pos.push_back(newPos);
            if(start != -1 && (tagName == "script" || tagName == "style"))
            {
                start = m_sct->FindText(start,end,"</");
                if(start == -1)
                    break;
                start = m_sct->FindText(start,end,">");
            }
            if(start == -1)
                break;
        }
        start+=1;
    }
}

void LanguageHtml::FindTags(std::vector<std::pair<int,int>>&pos,const wxString &text)
{
    char currentChar;
    wxString tagName;
    std::pair<int,int>newPos;
    int start = 0;
    int end = text.Length();
    while(start < end)
    {

        start = text.find("<",start);
        if(start == -1)
            break;
        if( text[start+1]=='/')
        {
            start+=2;
            continue;
        }

        else
        {
            newPos.first = start;
            start+=1;
            while(true)
            {
                currentChar = text[start];
                if(text[start] == ' '||text[start]=='>'
                               ||text[start]=='\n'||text[start]==0)
                               break;
                tagName.Append(currentChar);
                start+=1;
            }
            int rBracket = text.find("<",start);
            start =text.find(">",start);
            if(rBracket != -1 && start > rBracket)
            {
                start = -1;
            }
            newPos.second = start;
            pos.push_back(newPos);
            if(start != -1 && (tagName == "script" || tagName == "style"))
            {
                start = text.find("</",start);
                if(start == -1)
                    break;
                start = text.find(">",start);
            }
            if(start == -1)
                break;
        }

        start+=1;
    }
}

void LanguageHtml::EvaluateHtml(const wxString &html,int start,int end)
{

}

void LanguageHtml::EvaluateHtml(int start,int end)
{
    std::vector<std::pair<int,int>>htmlTags;
    FindTags(htmlTags,start,end);

    wxString tagContent;
    wxString tagName;
    std::map<wxString,wxString>tagAttr;
    for(const auto &i : htmlTags)
    {
        if(i.second ==-1)
            break;
        tagContent = m_sct->GetTextRange(i.first+1,i.second);
        tagName = ParseTagContent(tagContent,tagAttr);
        EvalTagContent(tagName,tagAttr);
        tagAttr.clear();
    }
}

void LanguageHtml::UnEvaluateHtml(const wxString &text)
{
    std::vector<std::pair<int,int>>htmlTags;
    FindTags(htmlTags,text);

    wxString tagContent;
    wxString tagName;
    std::map<wxString,wxString>tagAttr;
    for(const auto &i : htmlTags)
    {
        if(i.second==-1)
            break;
        tagContent = text.substr(i.first+1,i.second-i.first-1);
        tagName = ParseTagContent(tagContent,tagAttr);

        UnEvalTagContent(tagName,tagAttr);
        tagAttr.clear();
    }
}

namespace myFind{
    using namespace boost::spirit;
    using x3::rule;
    using x3::raw;
    using x3::lit;
    using x3::lexeme;

    char s[15]="helloworld";

    auto skipper = *(x3::ascii::space|(x3::char_-lit(s)));

    typedef std::string::iterator a;

    auto compWord = [&](const auto &ctx){cerr<<x3::_attr(ctx)<<endl;};

    typedef std::istreambuf_iterator<char> base_iterator_type;
    typedef boost::spirit::multi_pass<base_iterator_type> forward_iterator_type;
    typedef classic::position_iterator2<forward_iterator_type> pos_iterator_type;

    x3::rule<class myfind,boost::iterator_range<pos_iterator_type>> myfind;
    auto const myfind_def = raw[*(x3::char_-lit(s))];

    BOOST_SPIRIT_DEFINE(myfind);
}

void LanguageHtml::Find(const wxString &pin)
{
    strcpy(myFind::s,pin.c_str());
    wxStringInputStream strStream(m_sct->GetTextRaw());
    wxStdInputStream textStream(strStream);

    typedef std::istreambuf_iterator<char> base_iterator_type;
    base_iterator_type in_begin(textStream);

    typedef boost::spirit::multi_pass<base_iterator_type> forward_iterator_type;
    forward_iterator_type fwd_begin = boost::spirit::make_default_multi_pass(in_begin);
    forward_iterator_type fwd_end;

    typedef classic::position_iterator2<forward_iterator_type> pos_iterator_type;
    pos_iterator_type position_begin(fwd_begin,fwd_end);
    pos_iterator_type position_end;


    boost::iterator_range<pos_iterator_type> pos;
    boost::spirit::x3::phrase_parse(position_begin,position_end,myFind::myfind,boost::spirit::x3::ascii::space,pos);
    const auto content = pos.end().get_position();
    cerr<<"Line = "<<content.line<<" Column = "<<content.column<<endl;

}

int LanguageHtml::GetOpenTag2(wxString &tagName,int position,int limit)
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
        index--;
    }
    return -1;
}
