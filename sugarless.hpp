﻿#pragma once
#ifndef SUGARLESS_HPP
#define SUGARLESS_HPP

#include <initializer_list>
#include <tuple>
#include <unordered_map>
#include <string>
#include <regex>

namespace sugarless{
enum arg_style
{
    EQUAL_STYLE,
    SPACE_STYLE,
    UNIX_STYLE
};

typedef std::tuple<std::regex, std::regex, std::string, bool, std::string, bool, std::string> cmd_tuple;
class Command{
  private:
    bool auto_help_m;
    std::unordered_map<std::string, cmd_tuple> flag_items_m;
    std::string app_name_m;
    std::string arguments_text_m;
    std::string options_text_m;
    enum flag_index
    {
        SHORT_NAME,
        LONG_NAME,
        D_MESSAGE,
        NEED_ARG,
        DEF_VAL,
        IS_EXIST,
        ARG_VAL
    };
    bool isLongName(const std::string &_Str,std::smatch &_Re)
    {
        return std::regex_match(_Str, _Re, std::regex("--(.+?)(=(.+))?"));
    }
    bool isShortName(const std::string &_Str,std::smatch &_Re)
    {
        return std::regex_match(_Str, _Re, std::regex("-(.)(=(.+))?"));
    }
    bool isCompoundShortName(const std::string &_Str, std::smatch &_Re)
    {
        return std::regex_match(_Str, _Re, std::regex("-(.+?)(=(.+))?"));
    }
    bool __simple_string_parse(std::string &simple_string);
    bool __compound_short_name_parse(std::string &compound_short_name);
    bool _equal_parse(int argc, const char *argv[]);
    template<size_t INDEX>
    bool __equalIsMatch(std::string &target, std::string &target_arg);
    bool __equalArgSet(cmd_tuple &_Tuple, std::string &in_argv_i, std::string &target_arg);
    bool _space_parse(int argc, const char *argv[]);
    template<size_t INDEX>
    bool __spaceIsMatch(std::string &target_Str, int argc, const char *argv[], int &i, std::string &in_argv_i);
    bool __spaceArgSet(cmd_tuple &_Tuple, int argc, const char *argv[], int &i, std::string &in_argv_i);
    bool _unix_parse(int argc,const char *argv[]);
    void _auto_help(bool is_auto_help);

  public:
    Command(bool auto_help=true):auto_help_m(auto_help){
        _auto_help(auto_help);
    };
    Command(std::string &app_name,bool auto_help=true) : app_name_m(app_name) ,auto_help_m(auto_help){
        _auto_help(auto_help);
    }
    Command(std::string &&app_name,bool auto_help=true) : app_name_m(app_name),auto_help_m(auto_help){
        _auto_help(auto_help);
    }
    bool parse(int argc, const char *argv[], int arg_style=UNIX_STYLE);
    Command &argument(std::string tag_name, std::string description_message="",std::string default_val="");
    Command &flag(std::string tag_name, std::initializer_list<char> short_name={}, std::initializer_list<std::string> long_name={}, std::string description_message="", bool need_arg=false, std::string default_val="");
    bool has(std::string tag_name);
    template<class T>
    T get(std::string tag_name);
    void show_help();
    std::string get_help(void);
    void get_help(std::string &dst);

};

void Command::_auto_help(bool is_auto_help)
{
    if(is_auto_help)
    {
        flag("help",{'h'},{"help"},"show this message");
    }
}

inline bool accessable_next_argv(int argc,int now)
{
    return argc > now + 1;
}
template<typename ... Args>
inline auto eprintf_s(const char* format_str,Args const & ... args)
{
    return fprintf_s(stderr,format_str,args ...);
}
bool Command::parse(int argc,const char *argv[],int arg_style)
{
    bool result;
    if(app_name_m.empty())
    {
        app_name_m = std::string(argv[0]);
    }
    switch (arg_style)
    {
        case EQUAL_STYLE:
            result = _equal_parse(argc,argv);
            break;
        case SPACE_STYLE:
            result = _space_parse(argc,argv);
            break;
        case UNIX_STYLE:
            result = _unix_parse(argc,argv);
            break;
        default:
            eprintf_s("sugarless::Command::parse : received an invalid %d value for the third argument.\n the third argument must be EQUAL_STYLE, SPACE_STYLE or UNIX_STYLE.", arg_style);
            result = false;
            break;
    }
    if (auto_help_m && (!result || has("help")))
    {
        show_help();
    }
    return result;
}

bool Command::__simple_string_parse(std::string &simple_string)
{
    for (auto &f : flag_items_m)
    {
        if (!std::get<SHORT_NAME>(f.second).mark_count() && !std::get<LONG_NAME>(f.second).mark_count() && std::get<ARG_VAL>(f.second).empty())
        {
            std::get<ARG_VAL>(f.second) = simple_string;
            std::get<IS_EXIST>(f.second) = true;
            return 1;
        }
    }
    return 0;
}

bool Command::__equalArgSet(cmd_tuple &_Tuple, std::string &target, std::string &target_arg)
{
    if (std::get<NEED_ARG>(_Tuple)) //引数を必要とするオプションか確認
    {
        if (target_arg.empty())
        {
            if (std::get<DEF_VAL>(_Tuple).empty())
            {
                fprintf(stderr, "%s option need argument.\n", target.c_str());
                return 0;
            }
            else
            {
                std::get<ARG_VAL>(_Tuple) = target_arg;
            }
        }
        else
        {
            std::get<ARG_VAL>(_Tuple) = target_arg;
        }
    }
    else
    {
        if (!target_arg.empty())
        {
            fprintf(stderr, "%s option do not need argument.\n", target.c_str());
            return 0;
        }
    }
    return 1;
}

bool Command::__spaceArgSet(cmd_tuple &_Tuple,int argc,const char*argv[],int &i,std::string &in_argv_i)
{
    if (std::get<NEED_ARG>(_Tuple)) //引数を必要とするオプションか確認
    {
        if (accessable_next_argv(argc, i)) //次の要素にアクセスできるか確認
        {                                  //できる場合
            std::string next_arg(argv[++i]);
            if(next_arg == "--")
            {
                if(accessable_next_argv(argc,i))
                {
                    std::get<ARG_VAL>(_Tuple) = std::string(argv[++i]);
                }
                else
                {
                    eprintf_s("Invalid '--' received.");
                }
            }
            else if (std::regex_match(next_arg, std::regex("-(.+)"))) //引数パラメータの形をとっているか確認
            {                                                    //-X又は--xxの形式をとっていた場合
                if (std::get<DEF_VAL>(_Tuple).empty())
                { //デフォルト引数が設定されていない場合
                    eprintf_s("%s option need argument.\n", in_argv_i.c_str());
                    return 0;
                }
                else
                { //されている場合
                    std::get<ARG_VAL>(_Tuple) = std::get<DEF_VAL>(_Tuple);
                }
            }
            else
            { //-や--で始まらない通常文字列の場合
                std::get<ARG_VAL>(_Tuple) = next_arg;
            }
        }
        else
        { //出来ない場合
            if (std::get<DEF_VAL>(_Tuple).empty())
            { //デフォルト引数が設定されていない場合
                eprintf_s("%s option need argument.\n", in_argv_i.c_str());
                return 0;
            }
            else
            { //されている場合
                std::get<ARG_VAL>(_Tuple) = std::get<DEF_VAL>(_Tuple);
            }
        }
    }
    return 1;
}
template<size_t INDEX>
bool Command::__equalIsMatch(std::string &target, std::string &target_arg)
{
    for (auto &f : flag_items_m)
    {
        if (std::regex_match(target, std::get<INDEX>(f.second))) //設定した文字列のいずれかに当てはまるか確認
        {
            if (!__equalArgSet(f.second, target, target_arg))
            {
                return 0;
            }
            std::get<IS_EXIST>(f.second) = true;
            return 1;
        }
    }
    eprintf_s("Unknown option %s is ignored\n",target.c_str());
    return 1;
}
template<size_t INDEX>
bool Command::__spaceIsMatch(std::string &target_Str,int argc,const char *argv[],int &i,std::string &in_argv_i)
{
    for (auto &f : flag_items_m)
    {
        if (std::regex_match(target_Str, std::get<INDEX>(f.second))) //設定した文字列のいずれかに当てはまるか確認
        {
            if (!__spaceArgSet(f.second, argc, argv, i, in_argv_i))
            {
                return 0;
            }
            std::get<IS_EXIST>(f.second) = true;
            return 1;
        }
    }
    eprintf_s("Unknown option %s is ignored\n", target_Str.c_str());
    return 1;
}

bool Command::__compound_short_name_parse(std::string &compound_short_name)
{
    for (int i = 0; i < compound_short_name.size(); ++i)
    {
        std::string target = {compound_short_name[i]};
        for (auto &f : flag_items_m)
        {
            if (std::regex_match(target, std::get<SHORT_NAME>(f.second)))
            {
                if (std::get<NEED_ARG>(f.second)) //引数が必要なオプションかどうか
                {
                    if (std::get<DEF_VAL>(f.second).empty())
                    {
                        fprintf(stderr, "%s option need argument.\n", target.c_str());
                        return 0;
                    }
                    else
                    {
                        std::get<ARG_VAL>(f.second) = std::get<DEF_VAL>(f.second);
                    }
                }
                std::get<IS_EXIST>(f.second) = true;
            }
        }
    }
    return 1;
}

bool Command::_space_parse(int argc, const char *argv[])
{
    for(int i = 1; i < argc; i++)
    {
        std::string in_argv_i(argv[i]);
        std::smatch sub_much;
        if (in_argv_i != "--")
        {
            if (isLongName(in_argv_i, sub_much)) //long name
            {
                if (!__spaceIsMatch<LONG_NAME>(sub_much.str(1), argc, argv, i, in_argv_i))
                {
                    return 0;
                }
            }
            else if (isShortName(in_argv_i, sub_much)) //short name
            {
                if (!__spaceIsMatch<SHORT_NAME>(sub_much.str(1), argc, argv, i, in_argv_i))
                {
                    return 0;
                }
            }
            else if (isCompoundShortName(in_argv_i, sub_much)) //複合short name
            {
                if (!__compound_short_name_parse(sub_much.str(1)))
                {
                    return 0;
                }
            }
            else //simple string
            {
                if (!__simple_string_parse(in_argv_i))
                {
                    return 0;
                }
            }
        }
        else if(accessable_next_argv(argc,i))
        {
            in_argv_i = std::string(argv[++i]);
            if (!__simple_string_parse(in_argv_i))
            {
                return 0;
            }
        }
        else
        {
            eprintf_s("Invalid '--' received.");
            return 0;
        }
    }//end for
    return 1;
}

bool Command::_equal_parse(int argc, const char *argv[])
{
    for(int i = 1; i < argc; i++)
    {
        std::string in_argv_i(argv[i]);
        std::smatch sub_much;
        if(in_argv_i != "--")
        {
            if (isLongName(in_argv_i, sub_much)) //long
            {
                if (!__equalIsMatch<LONG_NAME>(sub_much.str(1), sub_much.str(3)))
                {
                    return 0;
                }
            }
            else if (isShortName(in_argv_i, sub_much)) //short
            {
                if (!__equalIsMatch<SHORT_NAME>(sub_much.str(1), sub_much.str(3)))
                {
                    return 0;
                }
            }
            else if (isCompoundShortName(in_argv_i, sub_much)) //compound short
            {
                if (sub_much.str(3).empty())
                {
                    if (!__compound_short_name_parse(sub_much.str(1)))
                    {
                        return 0;
                    }
                }
                else
                {
                    fprintf(stderr, "%s can not given argument.\n", in_argv_i.c_str());
                    return 0;
                }
            }
            else // simple string
            {
                if (!__simple_string_parse(in_argv_i))
                {
                    return 0;
                }
            }
        }
        else if (accessable_next_argv(argc, i))
        {
            in_argv_i = std::string(argv[++i]);
            if (!__simple_string_parse(in_argv_i))
            {
                return 0;
            }
        }
        else
        {
            eprintf_s("Invalid '--' received.");
            return 0;
        }
    }
    return 1;
}
bool Command::_unix_parse(int argc, const char *argv[])
{
    for(int i = 1; i < argc; i++)
    {
        std::string in_argv_i(argv[i]);
        std::smatch sub_much;
        if (in_argv_i != "--")
        {
            if (isLongName(in_argv_i, sub_much)) //long
            {
                if (!__equalIsMatch<LONG_NAME>(sub_much.str(1), sub_much.str(3)))
                {
                    return 0;
                }
            }
            else if (isShortName(in_argv_i, sub_much)) //short
            {
                if (!__spaceIsMatch<SHORT_NAME>(sub_much.str(1), argc, argv, i, in_argv_i))
                {
                    return 0;
                }
            }
            else if (isCompoundShortName(in_argv_i, sub_much)) //compound short
            {
                if (sub_much.str(3).empty())
                {
                    if (!__compound_short_name_parse(sub_much.str(1)))
                    {
                        return 0;
                    }
                }
                else
                {
                    fprintf(stderr, "%s can not given argument.\n", in_argv_i.c_str());
                    return 0;
                }
            }
            else // simple string
            {
                if (!__simple_string_parse(in_argv_i))
                {
                    return 0;
                }
            }
        }
        else if (accessable_next_argv(argc, i))
        {
            in_argv_i = std::string(argv[++i]);
            if (!__simple_string_parse(in_argv_i))
            {
                return 0;
            }
        }
        else
        {
            eprintf_s("Invalid '--' received.");
            return 0;
        }
    }
    return 1;
}

Command &Command::argument(std::string tag_name, std::string description_message, std::string default_val)
{
    return flag(tag_name, {}, {}, description_message, false, default_val);
}

Command &Command::flag(std::string tag_name, std::initializer_list<char> short_name, std::initializer_list<std::string> long_name, std::string description_message, bool need_arg, std::string default_val)
{
    std::string tmp;
    std::string str;
    if(short_name.size() != 0){
        str.push_back('(');
        for (auto &c : short_name)
        {
            tmp.push_back('-');
            tmp.push_back(c);
            tmp.push_back(' ');
            //for regex process
            str.push_back(c);
            str.push_back('|');
        }
        str.pop_back();
        str.push_back(')');
    }
    std::regex short_names(str.begin(), str.end());
    str.clear();
    if(long_name.size() !=0){
        str.push_back('(');
        for (auto &c : long_name)
        {
            tmp += "--";
            tmp += c;
            tmp.push_back(' ');
            //for regex process
            str += c;
            str.push_back('|');
        }
        str.pop_back();
        str.push_back(')');
    }
    if (!description_message.empty())
    {
        tmp += "\t:";
        tmp += description_message;
    }
    std::regex long_names(str.begin(), str.end());
    flag_items_m[tag_name] = std::make_tuple(short_names,long_names,description_message,need_arg,default_val,0,"");
    
    if (short_name.size() || long_name.size()) {
        options_text_m += tmp;
        options_text_m.push_back('\n');
    }
    else
    {
        arguments_text_m += tag_name;
        arguments_text_m += tmp;
        arguments_text_m.push_back('\n');
    }
        

    return *this;
}
bool Command::has(std::string tag_name){
    return std::get<IS_EXIST>(flag_items_m[tag_name]);
}

template<class T>
T Command::get(std::string tag_name){
    return T(std::get<ARG_VAL>(flag_items_m[tag_name]));
}
void Command::show_help(void)
//今後の課題　表示の成形
{
    eprintf_s("%s\n",get_help().c_str());
}

std::string Command::get_help(void)
{
    std::string text;
    text += app_name_m + "\n\n";
    if(!arguments_text_m.empty())
    {
        text += "Argments:\n";
        text += arguments_text_m;
        text += "\n";
    }
    if(!options_text_m.empty())
    {
        text += "Options:\n";
        text += options_text_m;
        text += "\n";
    }
    return text;
}
void Command::get_help(std::string &dst)
{
    dst.swap(get_help());
}

/* Templates Specialization */
template<>
int Command::get<int>(std::string tag_name){
    return std::stol(std::get<ARG_VAL>(flag_items_m[tag_name]));
}
template<>
unsigned int Command::get<unsigned int>(std::string tag_name){
    return std::stoul(std::get<ARG_VAL>(flag_items_m[tag_name]));
}
template<>
std::string Command::get<std::string>(std::string tag_name){
    return std::get<ARG_VAL>(flag_items_m[tag_name]);
}


}// end nemaspace sugarless
#endif //include gard SUGARLESS_HPP