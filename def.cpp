
#include "def.h"
#include <wx/log.h>
#include <wx/string.h>

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/writer.h"
#include <cstdio>

using namespace rapidjson;

StyleInfo Styles[LAST_STYLE_ELEMENT];
int StylesLength = sizeof(Styles)/sizeof(Styles[0]);

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

void ReverseString(wxString &value)
{
    int length  = value.Len() -1;

    wxString work_space;

    for(int i = length; i >= 0; i--)
    {
        work_space.Append(value.GetChar(i));
    }

    value = work_space;
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

//const StyleInfo Styles[] = {
//    // mySTC_TYPE_DEFAULT
//    {wxT("Default"),
//     wxT("BLACK"), wxT("WHITE"),
//     wxT(""), 10, 0, 0},
//
//    // mySTC_TYPE_WORD1
//    {wxT("Keyword1"),
//     wxT("BLUE"), wxT("WHITE"),
//     wxT(""), 10, mySTC_STYLE_BOLD, 0},
//
//    // mySTC_TYPE_WORD2
//    {wxT("Keyword2"),
//     wxT("MIDNIGHT BLUE"), wxT("WHITE"),
//     wxT(""), 10, 0, 0},
//
//    // mySTC_TYPE_WORD3
//    {wxT("Keyword3"),
//     wxT("CORNFLOWER BLUE"), wxT("WHITE"),
//     wxT(""), 10, 0, 0},
//
//    // mySTC_TYPE_WORD4
//    {wxT("Keyword4"),
//     wxT("CYAN"), wxT("WHITE"),
//     wxT(""), 10, 0, 0},
//
//    // mySTC_TYPE_WORD5
//    {wxT("Keyword5"),
//     wxT("DARK GREY"), wxT("WHITE"),
//     wxT(""), 10, 0, 0},
//
//    // mySTC_TYPE_WORD6
//    {wxT("Keyword6"),
//     wxT("GREY"), wxT("WHITE"),
//     wxT(""), 10, 0, 0},
//
//    // mySTC_TYPE_COMMENT
//    {wxT("Comment"),
//     wxT("FOREST GREEN"), wxT("WHITE"),
//     wxT(""), 10, 0, 0},
//
//    // mySTC_TYPE_COMMENT_DOC
//    {wxT("Comment (Doc)"),
//     wxT("FOREST GREEN"), wxT("WHITE"),
//     wxT(""), 10, 0, 0},
//
//    // mySTC_TYPE_COMMENT_LINE
//    {wxT("Comment line"),
//     wxT("FOREST GREEN"), wxT("WHITE"),
//     wxT(""), 10, 0, 0},
//
//    // mySTC_TYPE_COMMENT_SPECIAL
//    {wxT("Special comment"),
//     wxT("FOREST GREEN"), wxT("WHITE"),
//     wxT(""), 10, mySTC_STYLE_ITALIC, 0},
//
//    // mySTC_TYPE_CHARACTER
//    {wxT("Character"),
//     wxT("KHAKI"), wxT("WHITE"),
//     wxT(""), 10, 0, 0},
//
//    // mySTC_TYPE_CHARACTER_EOL
//    {wxT("Character (EOL)"),
//     wxT("KHAKI"), wxT("WHITE"),
//     wxT(""), 10, 0, 0},
//
//    // mySTC_TYPE_STRING
//    {wxT("String"),
//     wxT("BROWN"), wxT("WHITE"),
//     wxT(""), 10, 0, 0},
//
//    // mySTC_TYPE_STRING_EOL
//    {wxT("String (EOL)"),
//     wxT("BROWN"), wxT("WHITE"),
//     wxT(""), 10, 0, 0},
//
//    // mySTC_TYPE_DELIMITER
//    {wxT("Delimiter"),
//     wxT("ORANGE"), wxT("WHITE"),
//     wxT(""), 10, 0, 0},
//
//    // mySTC_TYPE_PUNCTUATION
//    {wxT("Punctuation"),
//     wxT("ORANGE"), wxT("WHITE"),
//     wxT(""), 10, 0, 0},
//
//    // mySTC_TYPE_OPERATOR
//    {wxT("Operator"),
//     wxT("BLACK"), wxT("WHITE"),
//     wxT(""), 10, mySTC_STYLE_BOLD, 0},
//
//    // mySTC_TYPE_BRACE
//    {wxT("Label"),
//     wxT("VIOLET"), wxT("WHITE"),
//     wxT(""), 10, 0, 0},
//
//    // mySTC_TYPE_COMMAND
//    {wxT("Command"),
//     wxT("BLUE"), wxT("WHITE"),
//     wxT(""), 10, 0, 0},
//
//    // mySTC_TYPE_IDENTIFIER
//    {wxT("Identifier"),
//     wxT("BLACK"), wxT("WHITE"),
//     wxT(""), 10, 0, 0},
//
//    // mySTC_TYPE_LABEL
//    {wxT("Label"),
//     wxT("VIOLET"), wxT("WHITE"),
//     wxT(""), 10, 0, 0},
//
//    // mySTC_TYPE_NUMBER
//    {wxT("Number"),
//     wxT("SIENNA"), wxT("WHITE"),
//     wxT(""), 10, 0, 0},
//
//    // mySTC_TYPE_PARAMETER
//    {wxT("Parameter"),
//     wxT("VIOLET"), wxT("WHITE"),
//     wxT(""), 10, mySTC_STYLE_ITALIC, 0},
//
//    // mySTC_TYPE_REGEX
//    {wxT("Regular expression"),
//     wxT("ORCHID"), wxT("WHITE"),
//     wxT(""), 10, 0, 0},
//
//    // mySTC_TYPE_UUID
//    {wxT("UUID"),
//     wxT("ORCHID"), wxT("WHITE"),
//     wxT(""), 10, 0, 0},
//
//    // mySTC_TYPE_VALUE
//    {wxT("Value"),
//     wxT("ORCHID"), wxT("WHITE"),
//     wxT(""), 10, mySTC_STYLE_ITALIC, 0},
//
//    // mySTC_TYPE_PREPROCESSOR
//    {wxT("Preprocessor"),
//     wxT("GREY"), wxT("WHITE"),
//     wxT(""), 10, 0, 0},
//
//    // mySTC_TYPE_SCRIPT
//    {wxT("Script"),
//     wxT("DARK GREY"), wxT("WHITE"),
//     wxT(""), 10, 0, 0},
//
//    // mySTC_TYPE_ERROR
//    {wxT("Error"),
//     wxT("RED"), wxT("WHITE"),
//     wxT(""), 10, 0, 0},
//
//    // mySTC_TYPE_UNDEFINED
//    {wxT("Undefined"),
//     wxT("ORANGE"), wxT("WHITE"),
//     wxT(""), 10, 0, 0}
//
//};

const int StylesNumber = WXSIZEOF(Styles);
