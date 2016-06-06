#include "auto_comp_list.h"
#include <wx/log.h>

AutoCompList::AutoCompList()
{
    //Initialize the list to null
    for(int i = 0; i < NUM_ALPHABET; i++)
    {
        root[i] = NULL;
    }
}

AutoCompList::~AutoCompList()
{
    for(int i = 0; i < NUM_ALPHABET; i++)
    {
        auto_comp_word *node = root[i];
        while(true)
        {
            if(node == NULL)
                break;
            auto_comp_word *child_node = node->child;
            delete node;
            node = child_node;
        }
    }
}

auto_comp_word *AutoCompList::GetWordNode(wxString &letter)
{
    int index = ( (int) letter.Lower().GetChar(0) ) - 97;
    return root[index];
}

int AutoCompList::GetWordIndex(wxString &letter)
{
    return (( (int) letter.Lower().GetChar(0) ) - 97 );
}

void AutoCompList::InsertWord(wxString word)
{

    auto_comp_word *data = new auto_comp_word;
    data->word = word;
    data->child = NULL;
    data->parent = NULL;

    int char_index = GetWordIndex(word);

    if(root[char_index] == NULL)
    {
        root[char_index] = data;
        return;
    }

    auto_comp_word **node = &(root[char_index]);

    int cmp_result;
    auto_comp_word **parent=NULL;

    while(true)
    {

        if(*node == NULL)
        {
            data->parent = *parent;
            (*parent)->child = data;
            data->parent->child = data;
        }

        wxString current_word = (*node)->word;

        cmp_result = current_word.Cmp(word);

        if(cmp_result > 0)
        {
            if((*node)->parent == NULL)
            {
                (*node)->parent = data;
                data->child = *node;

                *node = data;
                return;
            }

            else
            {
                data->parent = (*node)->parent;
                data->child = *node;

                ((*node)->parent)->child = data;
                (*node)->parent = data;
                return;
            }
        }

        else if(cmp_result < 0)
        {
            if((*node)->child == NULL)
            {
                (*node)->child = data;
                data->parent = (*node);
                return;
            }

            else
                node = &((*node)->child);
        }

        else        //word to insert is equal to current word
            return;

    }
}

void AutoCompList::GenerateList(wxString &word,wxString &word_list)
{

    auto_comp_word * node = GetWordNode(word);
    while(true)
    {
        if(node == NULL)
            break;

        if(node->word.Contains(word))
        {

            word_list.Append(node->word);
            word_list.Append(wxT(" "));
        }

        node = node->child;
    }

}
