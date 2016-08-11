#ifndef LANGUAGE_H_INCLUDED
#define LANGUAGE_H_INCLUDED

#include <wx/object.h>
#include <wx/stc/stc.h>

#include <vector>


#include "def.h"
class Edit;

#define TERMINATE_TYPE  -1

struct lang_style
{
    int type;
};

//Default Language
class Language : public wxObject
{
public:
    Language(Edit *sct);
    virtual void InitializeSCT();
    virtual void OnCharAdded(wxStyledTextEvent &event);
    virtual void OnKeyDown(wxKeyEvent &event);
    virtual void OnNewLine(wxStyledTextEvent &event);
    virtual void OnCursorPositionChange();
    virtual void OnModification(wxStyledTextEvent &event);
    virtual void OnLoad(){}
    virtual void OnUpdateUI(wxStyledTextEvent &event){}
    //Determine a bracket default and change their style accordingly
    void StyleBraces();
    void AutoCompBraces();
    void AlignBraceAfterEnter();
    char FirstChatAtLine(int line);
    int FirstCharAtLinePos(int line);
    void AutoIndent(int line);
    virtual void GetWordBeforeCursor(wxString &destination);
    int SearchPrev(const wxString &pin);
    int BraceMatch(int pos);
    int MyBraceMatch(int pos);
protected:
    Edit *m_sct;
private:
    //Test
    bool dirty;
    bool insertionOccured;
    int dirtyStartPos;
    int insertionStart;
    int insertionEnd;
    int deletionStart;
    int deletionEnd;
    int dirtyEndPos;
    int dirtyType;
    wxString m_text;
    virtual void EvaluateDirty();
};

//cpp language
class LanguageCpp : public Language
{
public:
    LanguageCpp(Edit *sct);
    virtual void InitializeSCT();
    virtual void OnCharAdded(wxStyledTextEvent &event);
    virtual void OnKeyDown(wxKeyEvent &event);
    virtual void OnCursorPositionChange();
    virtual void OnNewLine(wxStyledTextEvent &event);
};

//Java Language
class LanguageJava : public Language
{
public:
    LanguageJava(Edit *sct);
    virtual void InitializeSCT();
    virtual void OnCharAdded(wxStyledTextEvent &event);
    virtual void OnKeyDown(wxKeyEvent &event);
    virtual void OnNewLine(wxStyledTextEvent &event);
};

//JavaScript Language
class LanguageJS : public Language
{
public:
    LanguageJS(Edit *sct);
    virtual void InitializeSCT();
    virtual void OnCharAdded(wxStyledTextEvent &event);
    virtual void OnKeyDown(wxKeyEvent &event);
    virtual void OnNewLine(wxStyledTextEvent &event);
};

//CSS Language
class LanguageCss : public Language
{
public:
    LanguageCss(Edit *sct);
    virtual void InitializeSCT();
    virtual void OnCharAdded(wxStyledTextEvent &event);
    virtual void OnKeyDown(wxKeyEvent &event);
};

//Python Language
class LanguagePython : public Language
{
public:
    LanguagePython(Edit *sct);
    virtual void InitializeSCT();
    virtual void OnCharAdded(wxStyledTextEvent &event);
    virtual void OnKeyDown(wxKeyEvent &event);
    virtual void OnNewLine(wxStyledTextEvent &event);
};


//Html Language
class LanguageHtml : public Language
{
public:
    LanguageHtml(Edit *sct);
    virtual void InitializeSCT();
    virtual void OnCharAdded(wxStyledTextEvent &event);
    virtual void OnKeyDown(wxKeyEvent &event);
    virtual void OnNewLine(wxStyledTextEvent &event);
    virtual void OnCursorPositionChange();
    virtual void OnModification(wxStyledTextEvent &event);
    virtual void OnLoad();
    virtual void OnUpdateUI(wxStyledTextEvent &event);
    virtual void GetWordBeforeCursor(wxString &destination);
private:

    void OnCharAddedHtml(wxStyledTextEvent &event);
    void OnNewLineHtml(wxStyledTextEvent &event);
    void OnCursorPositionChangeHtml();
    void OnModificationHtml(wxStyledTextEvent &event);


    bool InsideOpenTag(int position);
    void findTag(const wxString &tagName,std::vector<std::pair<int,int>> &tagPos,int limit=-1);
    void FindTags(std::vector<std::pair<int,int>> &pos,int start,int end);
    void FindTags(std::vector<std::pair<int,int>> &pos,const wxString &text);
    void EvaluateHtml(const wxString &html,int start=-1,int end=-1);
    void EvaluateHtml(int start,int end);
    void UnEvaluateHtml(const wxString &text);
    void GetTagContent(wxString &tagContent,int position);
    wxString ParseTagContent(const wxString &content,std::map<wxString,wxString> &attr);
    void EvalTagContent(const wxString &tagName,const std::map<wxString,wxString>&attr);
    void UnEvalTagContent(const wxString &tagName,const std::map<wxString,wxString>&attr);
    int GetOpenTag(wxString &tagName,int position = -1,int limit = 0);
    int GetOpenTag2(wxString &tagName,int position = -1,int limit = 0);
    int GetNumOpenTag(int position,int limit=0);
    void GetEnclosingTag(wxString &tagName,int position=-1);
    void LoadCss(const wxString fileName);
    void ResolveFileLoc(wxString &fileName);

    int currentLang;
    LanguageCss langCss;
    LanguageJS langJs;

    std::vector<wxString>scope;
    void Find(const wxString &pin);
    //provide info for detecting which part of the document has changed
    bool dirty;
    bool insertionOccured;
    int dirtyStartPos;
    int insertionStart;
    int insertionEnd;
    int deletionStart;
    int deletionEnd;
    int dirtyEndPos;
    int dirtyType;
    wxString m_text;
    void EvaluateDirty();
};

//XML Language
class LanguageXml : public Language
{
public:
    LanguageXml(Edit *sct);
    virtual void InitializeSCT();
    virtual void OnCharAdded(wxStyledTextEvent &event);
    virtual void OnKeyDown(wxKeyEvent &event);
};

class LanguagePhp : public Language
{
public:
    LanguagePhp(Edit *sct);
    virtual void InitializeSCT();
    virtual void OnCharAdded(wxStyledTextEvent &event);
    virtual void OnKeyDown(wxKeyEvent &event);
};

#endif // LANGUAGE_H_INCLUDED
