#include "../language.h"
#include "../def.h"
#include <wx/stc/stc.h>
#include <wx/chartype.h>
#include <wx/log.h>
#include "../edit.h"
#include <math.h>

#include <iostream>
using std::cerr;
using std::endl;

#define MAX(first,second,ans) {if(second > first) ans=second;else ans=first;}

//Default Language
Language::Language(Edit *sct)
:dirty(false),insertionOccured(false)
{
    m_sct = sct;

    //Clear current style
    m_sct->StyleClearAll();

    m_sct->AutoCompSetAutoHide(true);
    m_sct->AutoCompSetIgnoreCase(true);
    m_sct->AutoCompSetCaseInsensitiveBehaviour(wxSTC_CASEINSENSITIVEBEHAVIOUR_RESPECTCASE);

    m_sct->AutoCompSetChooseSingle(true);

    m_sct->StyleSetBackground(wxSTC_STYLE_LINENUMBER,*wxWHITE);
    m_sct->SetFoldMarginColour(true,*wxWHITE);
    m_sct->SetFoldMarginHiColour(true,*wxWHITE);

    m_sct->SetMultipleSelection(true);
    m_sct->SetAdditionalCaretsVisible(false);
    m_sct->SetAdditionalSelBackground(*wxRED);

    m_sct->StyleSetForeground(wxSTC_STYLE_BRACEBAD,*wxRED);
    m_sct->StyleSetForeground(wxSTC_STYLE_BRACELIGHT,*wxBLUE);
}
void Language::InitializeSCT()
{
    m_sct->SetLexer(wxSTC_LEX_NULL);
}

void Language::OnLoad()
{
    m_sct->Colourise(0,-1);
}

void Language::OnCharAdded(wxStyledTextEvent &event)
{
    AutoCompBraces();
    //StyleBraces();

    if(m_sct->GetCharAt(m_sct->GetCurrentPos()-1)=='\n')
    {
        EvaluateDirty();
        wxString text = m_sct->GetLine(m_sct->GetCurrentLine()-1);
        cerr<<"Executed Text: "<<text.BeforeFirst('\n')<<endl;
    }
}

void Language::OnKeyDown(wxKeyEvent &event)
{

}

void Language::OnCursorPositionChange()
{
    //StyleBraces();
    EvaluateDirty();
}

void Language::OnNewLine(wxStyledTextEvent &event)
{
    AutoIndent(m_sct->GetCurrentLine());
    AlignBraceAfterEnter();
}

void Language::AutoIndent(int line)
{
    int indentation = m_sct->GetLineIndentation(line-1);
    m_sct->SetLineIndentation(line,indentation);
    int posReturn = FirstCharAtLinePos(line);
    m_sct->GotoPos(posReturn);
}

void Language::AutoIndentWithBrace(int line)
{
    int pos = m_sct->PositionFromLine(line);
    int openCBracket = GetOpenBrace(pos,'{');
    int openRBracket = GetOpenBrace(pos,'[');
    int openBracket =GetOpenBrace(pos,'(');

    int bracket;
    //set bracket to the maximum value of the three brackets
    MAX(openCBracket,openRBracket,bracket);
    MAX(bracket,openBracket,bracket);
    cerr<<"bracket = "<<bracket<<endl;
    if(bracket == -1)
    {
        int prevLine = line - 1;
        int indentation = 0;
        int openBracketPos = GetOpenBrace(m_sct->PositionFromLine(line-1),'(');
        int openRBracketPos = GetOpenBrace(m_sct->PositionFromLine(line-1),'[');
        int openBracePos;
        // change the posititon of prevLine to a line that does start in '(' and ']' bracket
        //and also set the indentation to indentation of prevLine;
        while(true)
        {
            cerr<<"openBracketPos = "<<openBracketPos<<" openRBracketPos = "<<openRBracketPos<<endl;
            MAX(openBracketPos,openRBracketPos,openBracePos);
            if(openBracePos == -1)
            {
                indentation = m_sct->GetLineIndentation(prevLine);
                break;
            }
            else if(openBracketPos != -1)
                prevLine = m_sct->LineFromPosition(openBracketPos);
            else if(openRBracketPos != -1)
                prevLine = m_sct->LineFromPosition(openRBracketPos);

            openBracketPos = GetOpenBrace(openBracePos-1,'(');
            openRBracketPos = GetOpenBrace(openRBracketPos-1,'[');

        }
        m_sct->SetLineIndentation(line,indentation);
        m_sct->GotoPos(FirstCharAtLinePos(line));
    }
    else if(bracket == openCBracket)
    {
        int indentation = m_sct->GetLineIndentation(m_sct->LineFromPosition(openCBracket));
        m_sct->SetLineIndentation(line,indentation+m_sct->GetTabWidth());
        m_sct->GotoPos(FirstCharAtLinePos(line));
    }
    else if(bracket == openBracket)
    {
        int bracketLine = m_sct->LineFromPosition(openBracket);
        int indentation = m_sct->GetLineIndentation(bracketLine);
        //indentation = indentation + (openBracket - FirstCharAtLinePos(bracketLine));
        m_sct->SetLineIndentation(line,indentation+m_sct->GetTabWidth());
        m_sct->GotoPos(FirstCharAtLinePos(line));
    }
    else if(bracket == openRBracket)
    {
        int bracketLine = m_sct->LineFromPosition(openRBracket);
        int indentation = m_sct->GetLineIndentation(bracketLine);
        //indentation = indentation + (openRBracket - FirstCharAtLinePos(bracketLine));
        m_sct->SetLineIndentation(line,indentation+m_sct->GetTabWidth());
        m_sct->GotoPos(FirstCharAtLinePos(line));
    }
    else
    {
        cerr<<"bracket unknown = "<<bracket<<endl;
    }
}

void Language::OnModification(wxStyledTextEvent &event)
{
    switch(event.GetModificationType())
    {
    case 1040:  //About to insert Text
        break;
    case 8209:  //New insertion Point
        {
            wxString lineContent = m_sct->GetLine(m_sct->GetCurrentLine());
            if((lineContent.Length() - event.GetLength()) != 0)
            {
                if(!dirty)
                {
                    dirty = true;
                    dirtyType = 1;
                    dirtyStartPos = event.GetPosition();
                    dirtyEndPos = dirtyStartPos+event.GetLength()-1;
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
        break;
    case 8210:  //New text delete
        {
            if(!dirty)
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
            break;
        }
    default:
        cerr<<event.GetModificationType()<<endl;
    }
}

void Language::EvaluateDirty()
{
    if(dirty==true)
    {
        if(dirtyType == 1)
        {
            int line = m_sct->LineFromPosition(dirtyStartPos);
            int textStart = m_sct->PositionFromLine(line);
            wxString text;
            text = m_sct->GetTextRange(textStart,dirtyStartPos);
            text.Append(m_sct->GetTextRange(dirtyEndPos+1,m_sct->GetLineEndPosition(m_sct->LineFromPosition(dirtyEndPos))));
            cerr<<"Previous Line = "<<text.BeforeFirst('\n')<<endl;
            cerr<<"Current Line = "<<m_sct->GetLine(line).BeforeFirst('\n')<<endl;
        }
        else
        {
            int line = m_sct->LineFromPosition(dirtyEndPos);
            wxString text = m_sct->GetTextRange(m_sct->PositionFromLine(line),dirtyEndPos);
            text.Append(m_text);
            if(insertionOccured)
            {
                dirtyEndPos=insertionEnd+1;
            }
            text.Append(m_sct->GetTextRange(dirtyEndPos,m_sct->GetLineEndPosition(line)));
            cerr<<"Previous line = "<<text.BeforeFirst('\n')<<endl;
            cerr<<"Current line = "<<m_sct->GetLine(line).BeforeFirst('\n')<<endl;
        }
        dirty = false;
        insertionOccured = false;
        m_text.Clear();
    }
}

char Language::GetBraceComplement(char brace)
{
    static wxString openingBraces("([{<");
    static wxString closingBraces(")]}>");

    if(openingBraces.Contains(wxString::Format("%c",brace)))
    {
        return closingBraces[openingBraces.find(brace)];
    }
    else if(closingBraces.Contains(wxString::Format("%c",brace)))
    {
        return openingBraces[closingBraces.find(brace)];
    }
    else
        return 0;
}

void Language::AutoCompBraces()
{
    enum {OPENING_BRACE,CLOSING_BRACE};

    static wxString openingBraces("([{");
    static wxString closingBraces(")]}");


    int currentPos = m_sct->GetCurrentPos();
    char enteredBrace=m_sct->GetCharAt(currentPos-1);

    if(openingBraces.Contains(enteredBrace))
    {
        if(!isalnum(m_sct->GetCharAt(currentPos)) || m_sct->GetCharAt(currentPos)==0)
        {
            m_sct->InsertText(m_sct->GetCurrentPos(),
                              wxString::Format("%c",GetBraceComplement(enteredBrace)));
        }
    }
    else if(closingBraces.Contains(enteredBrace))
    {
        if(m_sct->GetCharAt(currentPos) == enteredBrace)
        {
            m_sct->DeleteRange(currentPos,1);
        }
        else
        {
            int oppositeBrace = GetOpenBrace(currentPos-1,'{');
            if(oppositeBrace != -1)
            {
                int indentation = m_sct->GetLineIndentation(m_sct->LineFromPosition(oppositeBrace));
                m_sct->SetLineIndentation(m_sct->LineFromPosition(currentPos),indentation);
            }
            else
            {
                oppositeBrace = GetOpenBrace(currentPos-1,'(');
                if(oppositeBrace != -1)
                {
                    int oppositeBraceLine = m_sct->LineFromPosition(oppositeBrace);
                    int indentation = m_sct->GetLineIndentation(oppositeBraceLine);
                    indentation = indentation + (oppositeBrace - FirstCharAtLinePos(oppositeBraceLine));
                    m_sct->SetLineIndentation(m_sct->LineFromPosition(currentPos),indentation);
                }
                else
                {
                    oppositeBrace = GetOpenBrace(currentPos-1,'[');
                    if(oppositeBrace != -1)
                    {
                        int oppositeBraceLine = m_sct->LineFromPosition(oppositeBrace);
                        int indentation = m_sct->GetLineIndentation(oppositeBraceLine);
                        indentation = indentation + (oppositeBrace - FirstCharAtLinePos(oppositeBraceLine));
                        m_sct->SetLineIndentation(m_sct->LineFromPosition(currentPos),indentation);
                    }
                }
            }
        }
    }
}

char Language::FirstChatAtLine(int line)
{
    int pos = m_sct->PositionFromLine(line);
    int lineEnd = m_sct->GetLineEndPosition(line);
    char character=0;
    while(pos < lineEnd)
    {
        character = m_sct->GetCharAt(pos);
        if(isprint(character))
            break;
        pos++;
    }
    return character;
}

int Language::FirstCharAtLinePos(int line)
{
    int pos = m_sct->PositionFromLine(line);
    int lineEnd = m_sct->GetLineEndPosition(line);
    char character=0;
    while(pos < lineEnd)
    {
        character = m_sct->GetCharAt(pos);
        if(isgraph(character))
            break;
        pos++;
    }

    return pos;
}

//only call after \n is entered
void Language::AlignBraceAfterEnter()
{
    int currentPos = m_sct->GetCurrentPos();
    int currentLine = m_sct->GetCurrentLine();
    char enteredChar = m_sct->GetCharAt(currentPos);

    int correspondingBrace;
    int correspondingBraceLine;
    if(enteredChar == '}')
    {
        correspondingBrace = m_sct->BraceMatch(currentPos);
        correspondingBraceLine = m_sct->LineFromPosition(correspondingBrace);
        if(correspondingBrace != -1)
        {
            m_sct->InsertText(currentPos,"\n");
            int newIndentation = m_sct->GetLineIndentation(correspondingBraceLine);
            m_sct->SetLineIndentation(currentLine,newIndentation+m_sct->GetTabWidth());
            m_sct->SetLineIndentation(currentLine+1,newIndentation);
            m_sct->GotoPos(m_sct->GetLineEndPosition(currentLine));
        }
    }
}

void Language::StyleBraces()
{

    static bool unmachedBrace=false;
    static int lastBraceBadLight;


    static const wxString braces(wxT("{}[]()<>"));
    int currentPos = m_sct->GetCurrentPos()-1;
    int nextBracket=0;
    char character = (char) m_sct->GetCharAt(currentPos);

    if(braces.find(character) != wxString::npos)
    {
        nextBracket = BraceMatch(currentPos);
        //nextBracket = m_sct->BraceMatch(currentPos);
        //MyBraceMatch is used to fill in for the short coming of the brace Match
        //if(nextBracket == -1){nextBracket = MyBraceMatch(currentPos);}
        if(nextBracket == -1)
        {
            m_sct->BraceBadLight(currentPos);
            unmachedBrace = true;
            lastBraceBadLight = currentPos;
        }
        else
        {
            m_sct->BraceHighlight(currentPos,nextBracket);
            unmachedBrace = false;
        }
    }
    else if(!unmachedBrace)
    {
        //remove all the brace highlight if the char at current position in not a brace
        m_sct->BraceBadLight(-1);
    }
    else
    {
        //check if the last bracebadlight is still a brace and unbadbracehightlight it if not
        if(braces.find((char) m_sct->GetCharAt(lastBraceBadLight)) == wxString::npos)
            m_sct->BraceBadLight(-1);
    }

}

int Language::BraceMatch(int pos)
{
//    if(m_sct->GetCharAt(pos-1)!='"'&& m_sct->GetCharAt(pos-1)!='\'')
//        m_sct->Colourise(pos < 0?0:pos,pos+2);
    if(m_sct->GetStyleAt(pos-1) == 0)
        m_sct->Colourise(pos<0?0:pos,pos+2);
    return m_sct->BraceMatch(pos);
}

//only use with opening braces i.e ({[<
int Language::GetOpenBrace(int pos,char brace)
{
    if(pos < 0)
        return -1;
    wxString braceStr;
    braceStr.Printf("%c",brace);
    wxString braceComplement;
    braceComplement.Printf("%c",GetBraceComplement(brace));

    m_sct->SetUndoCollection(false);
    m_sct->InsertText(pos,braceComplement);
    int braceCompComp = BraceMatch(pos);
    m_sct->DeleteRange(pos,1);
    m_sct->SetUndoCollection(true);

    return braceCompComp;
}

int Language::MyBraceMatch(int pos)
{

    static const wxString openingBraces(wxT("<[({"));
    static const wxString closingBraces(wxT(">])}"));
    static const wxString braces(wxT("()[]{}<>"));
    char brace = m_sct->GetCharAt(pos);
    if(braces.find(brace) == wxString::npos)
        return -1;

    int index = pos;
    char correspondingBrace;
    int nextedBrace = 0;
    char characterHolder;
    if(openingBraces.find(brace)!=wxString::npos)
    {
        //move the index to the next position since value of current index is the brace itself
        index += 1;
        correspondingBrace =(char) closingBraces.GetChar(openingBraces.find(brace));
        while(index < m_sct->GetLength())
        {
            characterHolder = (char) m_sct->GetCharAt(index);
            if(characterHolder == correspondingBrace && nextedBrace == 0)
            {
                return index;
            }
            else if(characterHolder == correspondingBrace)
            {
                nextedBrace-=1;
                index+=1;
            }
            else if(characterHolder==brace)
            {
                nextedBrace+=1;
                index+=1;
            }
            else
            {
                index+=1;
            }
        }
        return -1;
    }
    else
    {
        //move the index to the next position since value of current index is the brace itself
        index -= 1;
        correspondingBrace = (char)openingBraces.GetChar(closingBraces.find(brace));
        while(index >= 0)
        {
            characterHolder = (char) m_sct->GetCharAt(index);
            if(characterHolder == correspondingBrace && nextedBrace == 0)
            {
                return index;
            }
            else if(characterHolder == correspondingBrace)
            {
                nextedBrace-=1;
                index-=1;
            }
            else if(characterHolder==brace)
            {
                nextedBrace+=1;
                index-=1;
            }
            else
            {
                index-=1;
            }
        }
        return -1;

    }
    return -1;
}

void Language::GetWordBeforeCursor(wxString &destination)
{
    int index = m_sct->GetCurrentPos()-1;
    while(index >= 0)
    {
        char character = m_sct->GetCharAt(index);
        if(isalnum(character)||character=='_')
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

int Language::SearchPrev(const wxString &pin)
{
    int currentPos = m_sct->GetCurrentPos();
    m_sct->GotoPos(currentPos - 1);
    m_sct->SearchAnchor();
    int pos = m_sct->SearchPrev(wxSTC_FIND_WORDSTART,pin);
    m_sct->GotoPos(currentPos);
    return pos;
}

void Language::CommentToggle()
{
    std::cout<<"Toggle comment called"<<std::endl;
    if(commentSym.IsEmpty())
        return;
    int start = m_sct->LineFromPosition(m_sct->GetSelectionStart());
    int end = m_sct->LineFromPosition(m_sct->GetSelectionEnd());
    
    bool comment = false;
    wxString lineContent;
    for(int i = start; i <= end; i++)
    {
        lineContent = m_sct->GetLine(i);
        if(lineContent.Strip(wxString::stripType::both).StartsWith(commentSym))
        {
        }
        else
        {
            comment = true;
            break;
        }
    }
    
    if(comment)
    {
        Comment();
    }
    else{
        Uncomment();
    }
}

void Language::Comment()
{
    if(commentSym.IsEmpty())
        return;
    int start = m_sct->LineFromPosition(m_sct->GetSelectionStart());
    int end = m_sct->LineFromPosition(m_sct->GetSelectionEnd());
    
    for(int i = start; i <= end; i++)
    {
        m_sct->InsertText(m_sct->PositionFromLine(i),commentSym);
    }
    
    m_sct->SetSelectionStart(m_sct->PositionFromLine(start));
    m_sct->SetSelectionEnd(m_sct->PositionFromLine(end));
}

void Language::Uncomment()
{
    if(commentSym.IsEmpty())
        return;
    int start = m_sct->LineFromPosition(m_sct->GetSelectionStart());
    int end = m_sct->LineFromPosition(m_sct->GetSelectionEnd());
    
    wxString lineContent;
    for(int i = start; i <= end; i++)
    {
        lineContent = m_sct->GetLine(i);
        if(lineContent.Strip(wxString::stripType::both).StartsWith(commentSym))
        {
            m_sct->DeleteRange(FirstCharAtLinePos(i),commentSym.Length());
        }
    }
}
