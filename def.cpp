
#include "def.h"
#include <wx/log.h>
#include <wx/string.h>
#include <wx/filename.h>
#include <string>
#include <exception>

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/writer.h"
#include <cstdio>

using namespace rapidjson;

StyleInfo Styles[LAST_STYLE_ELEMENT];
int StylesLength = sizeof(Styles)/sizeof(Styles[0]);
const int StylesNumber = WXSIZEOF(Styles);

std::map<wxString,AutoCompList> allAutoComplete;

int InitStyles()
{
    wxLog::SetActiveTarget(new wxLogStderr);

    FILE *styleConfig = fopen("config.json","r");
    char buffer[65536];
    FileReadStream configStream(styleConfig,buffer,sizeof(buffer));
    Document config;
    config.ParseStream(configStream);
    fclose(styleConfig);

    StyleInfo *style;
    Value &theme = config["theme"];
    for(SizeType i = 0; i < theme.Size();i++)
    {
        style = &(Styles[theme[i]["id"].GetInt()]);
        style->id = i;
        style->name = theme[i]["name"].GetString();
        const Value &background = theme[i]["background"];
        style->background = {background[0].GetInt(),background[1].GetInt(),background[2].GetInt()};
        const Value &foreground = theme[i]["foreground"];
        style->foreground = {foreground[0].GetInt(),foreground[1].GetInt(),foreground[2].GetInt()};
        style->fontname = theme[i]["fontname"].GetString();
        style->fontsize = theme[i]["fontsize"].GetInt();
        style->fontstyle = theme[i]["fontstyle"].GetInt();
    }
    return 1;
}

FileType files_supported[] = {
    { FILE_CPLUS, wxT("C++"), wxT("cpp h cxx CPP H CXX")},
    { FILE_PY, wxT("Python"), wxT("py")},
    { FILE_CSS, wxT("CSS"), wxT("css") },
    { FILE_XML, wxT("XML"), wxT("xml") },
    { FILE_HTML, wxT("HTML"), wxT("html") },
    { FILE_C, wxT("C++"), wxT("c") },
    { FILE_PHP, wxT("PHP"),  wxT("php") },
    { FILE_CSHARP, wxT("C#"), wxT("cp") },
    { FILE_JAVA, wxT("JAVA"), wxT("java") },
    { FILE_JAVASCRIPT, wxT("JavaScript") ,wxT("js") },
    { FILE_TXT, wxT("Text"), wxT("txt") },
    { FILE_SH, wxT("BashScript"), wxT("sh") },
    { FILE_RUBY, wxT("Ruby"), wxT("rb") },
};

int SaveStylesToJsonFile()
{
    Document config;
    config.SetObject();
    Value stylesArray(kArrayType);
    Document::AllocatorType &duplicator = config.GetAllocator();
    //insert value int stylesArray
    for(int i =0 ; i < StylesLength; i++)
    {
        Value styleElement(kObjectType);
        styleElement.AddMember("id",Styles[i].id,duplicator);
        Value name;
        name.SetString(Styles[i].name.c_str(),Styles[i].name.Length(),duplicator);
        styleElement.AddMember("name",name,duplicator);

        Value background(kArrayType);
        background.PushBack(Styles[i].background.red,duplicator);
        background.PushBack(Styles[i].background.green,duplicator);
        background.PushBack(Styles[i].background.blue,duplicator);
        styleElement.AddMember("background",background,duplicator);

        Value foreground(kArrayType);
        foreground.PushBack(Styles[i].foreground.red,duplicator);
        foreground.PushBack(Styles[i].foreground.green,duplicator);
        foreground.PushBack(Styles[i].foreground.blue,duplicator);
        styleElement.AddMember("foreground",foreground,duplicator);

        Value fontname;
        fontname.SetString(Styles[i].fontname.c_str(),Styles[i].fontname.Length(),duplicator);
        styleElement.AddMember("fontname",fontname,duplicator);
        styleElement.AddMember("fontsize",Styles[i].fontsize,duplicator);
        styleElement.AddMember("fontstyle",Styles[i].fontstyle,duplicator);

        stylesArray.PushBack(styleElement,duplicator);
    }

    config.AddMember("theme",stylesArray,duplicator);

    FILE *configFile = fopen("config.json","w");
    char buffer[65536];
    FileWriteStream configStream(configFile,buffer,sizeof(buffer));
    Writer<FileWriteStream> writer(configStream);
    config.Accept(writer);
    fclose(configFile);

    return 1;
}

void AutoCompList::InsertWord(const wxString &word)
{
    root.insert(std::pair<wxString,wordInfo>(word,wordInfo()));
}

void AutoCompList::InsertWord(const wxString &word,const wordInfo &info)
{
    root.insert(std::pair<wxString,wordInfo>(word,info));
}

void AutoCompList::InsertWords(const wxString &words,const char delimiter)
{
    wxString hold_word;
    char letter;

    size_t words_len =   words.Length();
    size_t  n=0;
    while(n < words_len)
    {
        letter = words.GetChar(n);
        if(letter == delimiter)
        {
            if(!hold_word.empty())
                InsertWord(hold_word);

            hold_word.Clear();
        }
        else
        {
            hold_word.Append(letter);
        }
        n+=1;
    }
}

void AutoCompList::GenerateList(const wxString &word,wxString &word_list)
{
    if(root.empty())
        return;

    auto smallLetterBegin = root.lower_bound("a");
    auto iter = root.lower_bound(word.Upper());
    while(true)
    {
        if(iter == root.end()||iter == smallLetterBegin)
        {
            break;
        }
        else if(iter->first.Lower().Contains(word.Lower()))
        {
            word_list.Append(iter->first);
            word_list.Append(" ");
        }
        else
        {
            break;
        }
        iter++;
    }

    iter = root.lower_bound(word.Lower());
    while(true)
    {
        if(iter==root.end())
        {
            break;
        }
        else if(iter->first.Lower().Contains(word.Lower()))
        {
            word_list.Append(iter->first);
            word_list.Append(" ");
        }
        else
        {
            break;
        }
        iter++;
    }

}

void AutoCompList::populateWordInfo(const Value &source,wordInfo &destination)
{
    Value::ConstMemberIterator itr = source.FindMember("id");
    if(itr == source.MemberEnd())
    {
        destination.type = TYPE_UNDEFINED;
        return;
    }
    switch(itr->value.GetInt())
    {
    case TYPE_STRING:
    case TYPE_NUMBER:
    case TYPE_REGEX:
        destination.type = itr->value.GetInt();
        break;

    case TYPE_FUNCTION:
        destination.type = TYPE_FUNCTION;
        destination.returnType = source["returnType"].GetString();
        break;

    case TYPE_ARRAY:
        {
            destination.type = TYPE_ARRAY;
            wordInfo arrayElement;
            const Value &a = source["Array"];
            for(SizeType i=0; i < a.Size();i++)
            {
                populateWordInfo(a[i],arrayElement);
                destination.Array.push_back(arrayElement);
            }
            break;
        }

    case TYPE_OBJECT:
        {
            destination.type = TYPE_OBJECT;
            const Value &a = source["Object"];
            for(Value::ConstMemberIterator i=a.MemberBegin();i != a.MemberEnd();++i)
            {
                wordInfo objectElement;
                populateWordInfo(itr->value,objectElement);
                destination.Object.insert(std::pair<wxString,wordInfo>(i->name.GetString(),objectElement));
            }
            break;
        }

    default:
        destination.type = TYPE_UNDEFINED;
    }
}

void AutoCompList::InsertWordsFromJsonFile(const wxString &fileName)
{
    FILE *openFile = fopen(fileName.c_str(),"r");
    Document json;
    char buffer[65536];
    FileReadStream fileStream(openFile,buffer,sizeof(buffer));
    json.ParseStream(fileStream);
    fclose(openFile);

    if(json.HasParseError())
    {
        wxString msg;
        msg.Printf("Error while parsing the file %s",fileName);
        wxLogMessage(msg);
        return;
    }
    for(Value::ConstMemberIterator itr = json.MemberBegin();itr!=json.MemberEnd();++itr)
    {
        wordInfo info;
        populateWordInfo(itr->value,info);
        InsertWord(itr->name.GetString());
    }
}
