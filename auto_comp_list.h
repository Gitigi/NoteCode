
#ifndef AUTO_COMP_LIST_H_INCLUDED
#define AUTO_COMP_LIST_H_INCLUDED


#include <wx/string.h>

#define NUM_ALPHABET 26

struct auto_comp_word
{
    wxString word;
    auto_comp_word *parent;
    auto_comp_word *child;
};

class AutoCompList
{
public:
    AutoCompList();
    ~AutoCompList();
    void InsertWord(wxString word);
    void GenerateList(wxString &word,wxString &word_list);
    auto_comp_word *GetWordNode(wxString &letter);
    int GetWordIndex(wxString &letter);
private:
    auto_comp_word *root[NUM_ALPHABET];
};


#endif
