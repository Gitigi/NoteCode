#include <boost/spirit/home/x3.hpp>
#include <boost/optional.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/variant.hpp>
#include <vector>
#include <iostream>
#include <string>
#include <functional>
#include <fstream>
#include <istream>

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::vector;
using std::cerr;

using namespace boost::spirit;

using x3::ascii::alnum;
using x3::char_;
using x3::lit;
using x3::lexeme;
using x3::blank;
using x3::omit;
using x3::no_skip;

namespace css {namespace ast {

    struct unit;
    struct at_rule;

    struct cssUnit : x3::variant<x3::forward_ast<unit>
                    ,x3::forward_ast<at_rule>>
    {
        using base_type::base_type;
        using base_type::operator=;
    };

    struct symbol
    {
        string identifier;
    };
    struct htmlTag
    {
        string name;
    };

    struct id
    {
        string name;
    };
    struct cssClass
    {
        string name;
    };

    struct pseudoClass
    {
        string name;
        string parameter;
    };

    struct pseudoElement
    {
        string name;
    };

    struct attribute
    {
        string attr;
        string oper;
        string attrValue;
    };

    struct primarySel : x3::variant<htmlTag,id,cssClass,pseudoClass,pseudoElement,attribute>
    {
        using base_type::base_type;
        using base_type::operator=;
    };

    struct secondarySel
    {
        htmlTag tag;
        std::vector<primarySel> elements;
    };

    struct combinedSel
    {
        secondarySel first;
        std::vector<std::pair<string,secondarySel>>second;
    };

    struct selector
    {
        vector<combinedSel> name;
    };
    struct style
    {
        string type;
        string value;
    };

    struct unit
    {
        selector sel;
        vector<style> styles;
    };

    struct atRegular
    {
        string name;
    };

    struct atNested
    {
        string name;
        std::vector<boost::variant<cssUnit,style>> body;
    };

    struct at_rule : x3::variant<atRegular,atNested>
    {
        using base_type::base_type;
        using base_type::operator=;
    };
}}

BOOST_FUSION_ADAPT_STRUCT(css::ast::symbol,identifier)
BOOST_FUSION_ADAPT_STRUCT(css::ast::cssClass,name)
BOOST_FUSION_ADAPT_STRUCT(css::ast::id,name)
BOOST_FUSION_ADAPT_STRUCT(css::ast::htmlTag,name)
BOOST_FUSION_ADAPT_STRUCT(css::ast::pseudoClass,name,parameter)
BOOST_FUSION_ADAPT_STRUCT(css::ast::pseudoElement,name)
BOOST_FUSION_ADAPT_STRUCT(css::ast::attribute,attr,oper,attrValue)
BOOST_FUSION_ADAPT_STRUCT(css::ast::secondarySel,tag,elements)
BOOST_FUSION_ADAPT_STRUCT(css::ast::combinedSel,first,second)
BOOST_FUSION_ADAPT_STRUCT(css::ast::selector,name)
BOOST_FUSION_ADAPT_STRUCT(css::ast::style,type,value)
BOOST_FUSION_ADAPT_STRUCT(css::ast::unit,sel,styles)
BOOST_FUSION_ADAPT_STRUCT(css::ast::atRegular,name)
BOOST_FUSION_ADAPT_STRUCT(css::ast::atNested,name,body)

namespace css {namespace ast {
    struct printAst
    {
        void operator()(const primarySel &val)const
        {
            boost::apply_visitor(*this,val);
        }
        void operator()(const symbol &val)const
        {
            cerr<<val.identifier;
        }
        void operator()(const id &val)const
        {
            cerr<<"#"<<val.name;
        }
        void operator()(const htmlTag &val)const
        {
            cerr<<val.name;
        }
        void operator()(const cssClass &val)const
        {
            cerr<<"."<<val.name;
        }
        void operator()(const pseudoClass &val)const
        {
            cerr<<":"<<val.name;
            if(!val.parameter.empty())
            {
                cerr<<"("<<val.parameter<<")";
            }
        }
        void operator()(const pseudoElement &val)const
        {
            cerr<<"::"<<val.name;
        }
        void operator()(const attribute &val)const
        {
            cerr<<"["<<val.attr;
            if(!val.oper.empty())
            {
                cerr<<val.oper<<"\""<<val.attrValue<<"\"";
            }
            cerr<<"]";
        }
        void operator()(const secondarySel &val)const
        {
            cerr<<val.tag.name;
            for(const auto &i:val.elements)
            {
                boost::apply_visitor(*this,i);
            }
        }
        void operator()(const combinedSel &val)const
        {
            printAst()(val.first);
            for(const auto &i : val.second)
            {
                cerr<<i.first;
                printAst()(i.second);
            }
        }
        void operator()(const selector &val)const
        {
            size_t length = val.name.size()-1;
            for(size_t i=0;i<=length;i++)
            {
                printAst()(val.name[i]);
                if(i != length)
                    cerr<<",";
            }
        }
        void operator()(const style &val)const
        {
            cerr<<val.type<<":"<<val.value<<";"<<endl;
        }
        void operator()(const unit &val)const
        {
            printAst()(val.sel);
            cerr<<endl<<"{"<<endl;
            for(const auto &i : val.styles)
            {
                printAst()(i);
            }
            cerr<<"}"<<endl;;
        }

        void operator()(const atRegular&val)const
        {
            cerr<<"@"<<val.name;
        }

        void operator()(const atNested &val)const
        {
            cerr<<"@"<<val.name;
            cerr<<endl<<"{"<<endl;
            for(const auto &i : val.body)
            {
                 boost::apply_visitor(*this,i);
            }
            cerr<<"}"<<endl;
        }

        void operator()(const at_rule &val)const
        {
            boost::apply_visitor(*this,val);
        }

        void operator()(const cssUnit &val)const
        {
            boost::apply_visitor(*this,val);
        }
    };
}}

namespace css {namespace parser{

    x3::rule<class symbol,ast::symbol>symbol = "symbol";
    x3::rule<class htmlTag,ast::htmlTag>htmlTag = "htmlTag";
    x3::rule<class id,ast::id>id = "id";
    x3::rule<class cssClass,ast::cssClass>cssClass="cssClass";
    x3::rule<class pseudoClass,ast::pseudoClass>pseudoClass = "pseudoClass";
    x3::rule<class psuedoElement,ast::pseudoElement>pseudoElement = "pseudoElement";
    x3::rule<class attribute,ast::attribute>attribute="attribute";
    x3::rule<class secondarySel,ast::secondarySel>secondarySel = "secondarySel";
    x3::rule<class combinatorOper,string>combinatorOper="combinatorOper";
    x3::rule<class combinedSel,ast::combinedSel>combinedSel = "combinedSel";
    x3::rule<class selector,ast::selector>selector = "selector";
    x3::rule<class style,ast::style>style = "style";
    x3::rule<class unit,ast::unit>unit = "unit";
    x3::rule<class atRegular,ast::atRegular>atRegular="atRegular";
    x3::rule<class atNested,ast::atNested>atNested = "atNested";
    x3::rule<class at_rule,ast::at_rule>at_rule="at_rule";
    x3::rule<class cssUnit,ast::cssUnit>cssUnit = "cssUnit";

    auto symbol_def = x3::lexeme[+(alnum|char_('-')|char_('%')|char_('_'))];
    auto string_def = x3::lexeme[(lit('"')>>*(char_-'"')>>lit('"'))|(lit('\'')>>*(char_-'\'')>>lit('\''))];
    auto htmlTag_def = symbol_def|x3::string("*");
    auto id_def = lit('#')>>symbol_def;
    auto cssClass_def = lit('.')>>symbol_def;

    auto pseudoClass2_def = x3::rule<class pseudoClass2_def,string>{}
        %= char_(':')>>symbol_def>>char_('(')>>*(char_-lit(')'))>>char_(')');

    auto pseudoClass_def = lit(':')>>symbol_def>>-(lit('(')>>(pseudoClass2_def|*(char_-(lit(')')|lit('('))))>>lit(')'));
    auto pseudoElement_def = lit("::")>>symbol_def;

    auto operators = (x3::string("=")|x3::string("|=")|x3::string("*=")|x3::string("~=")
                      |x3::string("^=")|x3::string("$="));
    auto attribute_def = lit("[")>>symbol_def>>-(operators>>(symbol_def|string_def))>>lit("]");
    auto secondarySel_def = lexeme[(htmlTag>>*(id|cssClass|pseudoClass|pseudoElement|attribute))
                            |(-htmlTag>>+(id|cssClass|pseudoClass|pseudoElement|attribute))];

    auto combinatorOper_def = x3::string(">")|x3::string("+")|x3::string("~")|
                            x3::string("*")|x3::attr(std::string(" "));
    auto combinedSel_def = secondarySel >> *(combinatorOper>>secondarySel);
    auto selector_def = combinedSel%lit(',');

    auto string2_def = x3::lexeme[(char_('"')>>*(char_-'"')>>char_('"'))|(char_('\'')>>*(char_-'\'')>>char_('\''))];
    auto symbol2_def = x3::lexeme[+(char_-(lit(';')|lit('}')|lit('"')|lit('\'')))];
    auto style_def = symbol_def>>lit(':')>>lexeme[+(symbol2_def|string2_def)]>>(lit(';')|&lit('}'));

    auto unit_def = selector>>lit('{')>>*style>>lit('}');

    auto atRegular_def = lit('@')>>lexeme[*(char_-(lit(';')|lit('{')))]>>lit(';');
    auto atNested_def = lit('@')>>lexeme[+(char_-lit('{'))]>>lit('{')>>+(style|cssUnit)>>lit('}');
    auto at_rule_def = atRegular|atNested;

    auto cssUnit_def = unit | at_rule;

    BOOST_SPIRIT_DEFINE(symbol,selector,htmlTag,id,cssClass,pseudoClass,pseudoElement,attribute,
                        secondarySel,combinedSel,combinatorOper,style,unit,at_rule,atNested,atRegular,cssUnit);
}}

namespace css{
    void ParserCss(const char* fileName,std::vector<ast::cssUnit> &cssAst)
    {
        auto const comment = lexeme[lit("/*") >> *((char_-'*')|(char_('*')>>!(char_('/'))))>>lit("*/")];
        auto const skipper = (x3::space | comment);

        std::fstream inputFile(fileName);
        if(!inputFile)
        {
            cerr<<"could not file"<<endl;
        }
        inputFile.unsetf(std::ios::skipws);
        boost::spirit::istream_iterator start(inputFile);
        boost::spirit::istream_iterator end;

        x3::phrase_parse(start,end,*css::parser::cssUnit,skipper,cssAst);
        inputFile.close();
    }
    void ParserCss(std::istream &inputStream,std::vector<ast::cssUnit>&cssAst)
    {
        auto const comment = lexeme[lit("/*") >> *((char_-'*')|(char_('*')>>!(char_('/'))))>>lit("*/")];
        auto const skipper = (x3::space | comment);

        boost::spirit::istream_iterator start(inputStream);
        boost::spirit::istream_iterator end;

        x3::phrase_parse(start,end,*css::parser::cssUnit,skipper,cssAst);
    }

    auto defaultLoad = [](const string &name){cerr<<name;};
    struct LoadClassAndId
    {
        LoadClassAndId(std::function<void(const string &)> func=defaultLoad)
        {
            m_func = func;
        }
        std::function<void(const string &)> m_func;
        void operator()(const ast::primarySel &val)const
        {
            boost::apply_visitor(*this,val);
        }
        void operator()(const ast::symbol &val)const
        {

        }
        void operator()(const ast::id &val)const
        {
            m_func(val.name);
        }
        void operator()(const ast::htmlTag &val)const
        {

        }
        void operator()(const ast::cssClass &val)const
        {
            m_func(val.name);
        }
        void operator()(const ast::pseudoClass &val)const
        {

        }
        void operator()(const ast::pseudoElement &val)const
        {

        }
        void operator()(const ast::attribute &val)const
        {
            if(val.attr == "class" && !val.oper.empty())
            {
                m_func(val.attrValue);
            }
        }
        void operator()(const ast::secondarySel &val)const
        {
            for(const auto &i:val.elements)
            {
                boost::apply_visitor(*this,i);
            }
        }
        void operator()(const ast::combinedSel &val)const
        {
            (*this)(val.first);
            for(const auto &i : val.second)
            {
                (*this)(i.second);
            }
        }
        void operator()(const ast::selector &val)const
        {
            size_t length = val.name.size()-1;
            for(size_t i=0;i<=length;i++)
            {
                (*this)(val.name[i]);
            }
        }
        void operator()(const ast::style &val)const
        {

        }
        void operator()(const ast::unit &val)const
        {
            (*this)(val.sel);
        }

        void operator()(const ast::atRegular&val)const
        {
            //cerr<<"@"<<val.name;
        }

        void operator()(const ast::atNested &val)const
        {
            for(const auto &i : val.body)
            {
                 boost::apply_visitor(*this,i);
            }
        }

        void operator()(const ast::at_rule &val)const
        {
            boost::apply_visitor(*this,val);
        }

        void operator()(const ast::cssUnit &val)const
        {
            boost::apply_visitor(*this,val);
        }

        void operator()(const std::vector<ast::cssUnit>&val)const
        {
            for(const auto &i : val)
            {
                boost::apply_visitor(*this,i);
            }
        }
    };
}
