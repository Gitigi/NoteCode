#ifndef LANGUAGE_H_INCLUDED
#define LANGUAGE_H_INCLUDED

#include <wx/object.h>
#include <wx/stc/stc.h>

#include <vector>


#include "def.h"

#define TERMINATE_TYPE  -1

struct lang_style
{
    int type;
};

//Default Language
class Language : public wxObject
{
public:
    Language(wxStyledTextCtrl *sct);
    virtual void InitializeSCT();
    virtual void OnCharAdded(wxStyledTextEvent &event);
    virtual void OnKeyDown(wxKeyEvent &event);
    virtual void OnNewLine(wxStyledTextEvent &event);
    virtual void OnCursorPositionChange();
    //Determine a bracket default and change their style accordingly
    void StyleBraces();
    virtual void GetWordBeforeCursor(wxString &destination);
    int MyBraceMatch(int pos);
protected:
    wxStyledTextCtrl *m_sct;
};

//cpp language
class LanguageCpp : public Language
{
public:
    LanguageCpp(wxStyledTextCtrl *sct);
    virtual void InitializeSCT();
    virtual void OnCharAdded(wxStyledTextEvent &event);
    virtual void OnKeyDown(wxKeyEvent &event);
    virtual void OnCursorPositionChange();
};

//Java Language
class LanguageJava : public Language
{
public:
    LanguageJava(wxStyledTextCtrl *sct);
    virtual void InitializeSCT();
    virtual void OnCharAdded(wxStyledTextEvent &event);
    virtual void OnKeyDown(wxKeyEvent &event);
};

//JavaScript Language
class LanguageJS : public Language
{
public:
    LanguageJS(wxStyledTextCtrl *sct);
    virtual void InitializeSCT();
    virtual void OnCharAdded(wxStyledTextEvent &event);
    virtual void OnKeyDown(wxKeyEvent &event);
};

//CSS Language
class LanguageCss : public Language
{
public:
    LanguageCss(wxStyledTextCtrl *sct);
    virtual void InitializeSCT();
    virtual void OnCharAdded(wxStyledTextEvent &event);
    virtual void OnKeyDown(wxKeyEvent &event);
};

//Python Language
class LanguagePython : public Language
{
public:
    LanguagePython(wxStyledTextCtrl *sct);
    virtual void InitializeSCT();
    virtual void OnCharAdded(wxStyledTextEvent &event);
    virtual void OnKeyDown(wxKeyEvent &event);
    virtual void OnNewLine(wxStyledTextEvent &event);
};


//Html Language
class LanguageHtml : public Language
{
public:
    LanguageHtml(wxStyledTextCtrl *sct);
    virtual void InitializeSCT();
    virtual void OnCharAdded(wxStyledTextEvent &event);
    virtual void OnKeyDown(wxKeyEvent &event);
    virtual void OnNewLine(wxStyledTextEvent &event);
    virtual void OnCursorPositionChange();
private:
    void findTag(const wxString &tagName,std::vector<std::pair<int,int>> &tagPos,int limit=-1);
    int GetOpenTag(wxString &tagName,int position = -1,int limit = 0);
    int GetNumOpenTag(int position,int limit=0);
    void GetEnclosingTag(wxString &tagName,int position=-1);
};

//XML Language
class LanguageXml : public Language
{
public:
    LanguageXml(wxStyledTextCtrl *sct);
    virtual void InitializeSCT();
    virtual void OnCharAdded(wxStyledTextEvent &event);
    virtual void OnKeyDown(wxKeyEvent &event);
};

class LanguagePhp : public Language
{
public:
    LanguagePhp(wxStyledTextCtrl *sct);
    virtual void InitializeSCT();
    virtual void OnCharAdded(wxStyledTextEvent &event);
    virtual void OnKeyDown(wxKeyEvent &event);
};

#endif // LANGUAGE_H_INCLUDED
