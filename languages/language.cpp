#include "../language.h"
#include "../def.h"
#include <wx/stc/stc.h>
#include <wx/chartype.h>
#include <wx/log.h>
#include "../edit.h"

//Default Language
Language::Language(wxStyledTextCtrl *sct)
{
    m_sct = sct;

    //Clear current style
    m_sct->StyleClearAll();

    m_sct->AutoCompSetAutoHide(true);
    m_sct->AutoCompSetFillUps(wxT("\n"));
    m_sct->AutoCompSetIgnoreCase(true);
    m_sct->AutoCompSetCaseInsensitiveBehaviour(wxSTC_CASEINSENSITIVEBEHAVIOUR_RESPECTCASE);

    m_sct->AutoCompStops(wxT(". (/[\\"));
    m_sct->AutoCompSetChooseSingle(true);

    m_sct->StyleSetBackground(wxSTC_STYLE_LINENUMBER,*wxWHITE);
    m_sct->SetMultipleSelection(true);
    m_sct->SetAdditionalCaretsVisible(false);
    m_sct->SetAdditionalSelBackground(*wxRED);

    m_sct->StyleSetForeground(wxSTC_STYLE_BRACEBAD,*wxRED);
    m_sct->StyleSetForeground(wxSTC_STYLE_BRACELIGHT,*wxBLUE);
}

void Language::InitializeSCT()
{

}

void Language::OnCharAdded(wxStyledTextEvent &event)
{
    StyleBrackets();
}

void Language::OnKeyDown(wxKeyEvent &event)
{

}

void Language::OnCursorPositionChange()
{
    StyleBrackets();
}

void Language::StyleBrackets()
{
    wxLog::SetActiveTarget(new wxLogStderr);
    wxString msg;

    static bool unmachedBrace=false;
    static int lastBraceBadLight;


    static const wxString braces(wxT("{}[]()<>"));
    int currentPos = m_sct->GetCurrentPos() - 1;
    int nextBracket=0;
    char character = (char) m_sct->GetCharAt(currentPos);
    if(braces.find(character) != wxString::npos)
    {
        nextBracket = MyBraceMatch(currentPos);
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

void Language::OnNewLine(wxStyledTextEvent &event)
{

}
