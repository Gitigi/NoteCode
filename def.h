#ifndef DEF_H_INCLUDED
#define DEF_H_INCLUDED

#include <wx/string.h>

//! style bits types
#define mySTC_STYLE_BOLD 1
#define mySTC_STYLE_ITALIC 2
#define mySTC_STYLE_UNDERL 4
#define mySTC_STYLE_HIDDEN 8

//! general style types
#define mySTC_TYPE_DEFAULT 0

#define mySTC_TYPE_WORD1 1
#define mySTC_TYPE_WORD2 2
#define mySTC_TYPE_WORD3 3
#define mySTC_TYPE_WORD4 4
#define mySTC_TYPE_WORD5 5
#define mySTC_TYPE_WORD6 6

#define mySTC_TYPE_COMMENT 7
#define mySTC_TYPE_COMMENT_DOC 8
#define mySTC_TYPE_COMMENT_LINE 9
#define mySTC_TYPE_COMMENT_SPECIAL 10

#define mySTC_TYPE_CHARACTER 11
#define mySTC_TYPE_CHARACTER_EOL 12
#define mySTC_TYPE_STRING 13
#define mySTC_TYPE_STRING_EOL 14

#define mySTC_TYPE_DELIMITER 15

#define mySTC_TYPE_PUNCTUATION 16

#define mySTC_TYPE_OPERATOR 17

#define mySTC_TYPE_BRACE 18

#define mySTC_TYPE_COMMAND 19
#define mySTC_TYPE_IDENTIFIER 20
#define mySTC_TYPE_LABEL 21
#define mySTC_TYPE_NUMBER 22
#define mySTC_TYPE_PARAMETER 23
#define mySTC_TYPE_REGEX 24
#define mySTC_TYPE_UUID 25
#define mySTC_TYPE_VALUE 26

#define mySTC_TYPE_PREPROCESSOR 27
#define mySTC_TYPE_SCRIPT 28

#define mySTC_TYPE_ERROR 29

#define TAB_SIZE 4

#include <wx/string.h>

extern void ReverseString(wxString &value);

enum STYLE_ELEMENTS
{
    DEFAULT,
    WORD1,
    WORD2,
    WORD3,
    WORD4,
    WORD5,
    WORD6,
    COMMENT,
    COMMENT_DOC,
    COMMENT_LINE,
    COMMENT_SPECIAL,
    CHARACTER,
    CHARACTER_EOL,
    STRING,
    STRING_EOL,
    DELIMITER,
    PUNCTUATION,
    OPERATOR,
    BRACE,
    COMMAND,
    IDENTIFIER,
    LABEL,
    NUMBER,
    PARAMETER,
    REGEX,
    UIID,
    VALUE,
    PREPROCESSOR,
    SCRIPT,
    ERROR,
    UNDEFINED,
    DECORATOR,
    CLASSNAME,
    TRIPLEDOUBLE,
    VERBATIM,
    LAST_STYLE_ELEMENT,
};

enum SUPPORTED_FILES{
    FILE_CPLUS = 0,
    FILE_PY,
    FILE_CSS,
    FILE_XML,
    FILE_HTML,
    FILE_C,
    FILE_PHP,
    FILE_CSHARP,
    FILE_JAVA,
    FILE_JAVASCRIPT,
    FILE_TXT,
    FILE_SH,
    FILE_RUBY,
    FILE_TOTOAL,
};

enum MYID
{
    myID_HIGLIGHTLANG = wxID_HIGHEST,
    myID_HIGLIGHTLANGFIRST,
    myID_HIGLIGHTLANGLAST = myID_HIGLIGHTLANGFIRST+ FILE_TOTOAL,
    myID_DWELLSTART,
    myID_DWELLEND,
    ID_DEFAULT = wxID_HIGHEST,
    ID_LAST_STYLE_ELEMENT = (ID_DEFAULT+LAST_STYLE_ELEMENT)*2,
};

struct FileType{
    int file_type;
    wxString name;
    wxString ext;
};



extern FileType files_supported[];


//struct StyleInfo
//{
//    const wxChar *name;
//    const wxChar *foreground;
//    const wxChar *background;
//    const wxChar *fontname;
//    int fontsize;
//    int fontstyle;
//    int lettercase;
//};

struct colourFormat
{
    int red;
    int green;
    int blue;
};

struct StyleInfo
{
    int id;
    wxString name;
    colourFormat background;
    colourFormat foreground;
    wxString fontname;
    int fontsize;
    int fontstyle;
};

//extern const StyleInfo Styles[];
extern const int StylesNumber;
extern StyleInfo Styles[];
extern int InitStyles();
extern int SaveStylesToJsonFile();
extern int StylesLength;

#endif // DEF_H_INCLUDED
