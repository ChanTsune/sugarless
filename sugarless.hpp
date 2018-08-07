#ifndef SUGARLESS_HPP
#define SUGARLESS_HPP

#include <vector>
#include <tuple>
#include <unordered_map>
#include <string>
#include <regex>

namespace sugarless{
enum arg_type{
    EQUAL_TYPE,
    SPACE_TYPE
};

typedef std::tuple<std::regex, std::regex, std::string, bool, std::string, bool, std::string> cmd_tuple;
class Command{
  private:
    std::unordered_map<std::string, cmd_tuple > flag_items_m;
    std::string license_txt_m;
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
    bool __equalArgSet(cmd_tuple &_Tuple, std::string &in_argv_i, std::string &target_arg);
    bool _space_parse(int argc, const char *argv[]);
    bool __spaceArgSet(cmd_tuple &_Tuple, int argc, const char *argv[], int &i, std::string &in_argv_i);

  public:
    Command(void){};
    Command(std::string &license_text) : license_txt_m(license_text) {}
    Command(std::string &&license_text) : license_txt_m(license_text) {}
    bool parse(int argc, const char *argv[], int arg_type=SPACE_TYPE);
    Command &flag(std::string tag_name,std::vector<char> short_name={},std::vector<std::string> long_name={},std::string description_message="",bool need_arg=false,std::string default_val="");
    bool has(std::string tag_name);
    template<class T>
    T get(std::string tag_name);
    //====//
    void show_help(bool with_license=false);
    std::string get_help(void);
    void get_help(std::string &dst);
    void show_license(void);
    std::string get_license(void);
    void get_license(std::string &dst);
    //====//

};

inline bool accessable_next_argv(int argc,int now){
    return argc > now + 1;
}

bool Command::parse(int argc,const char *argv[],int arg_type){

    switch (arg_type)
    {
        case EQUAL_TYPE:
            return _equal_parse(argc,argv);
        case SPACE_TYPE:
            return _space_parse(argc,argv);
        default:
            fprintf_s(stderr, "sugarless::Command::parse : received an invalid %d value for the third argument.",arg_type);
            break;
    }
    return 0;
}

bool Command::__simple_string_parse(std::string &simple_string){
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
                fprintf(stderr, "%s option need argment.\n", target.c_str());
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
            fprintf(stderr, "%s option do not need argment.\n", target.c_str());
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
            if (std::regex_match(next_arg, std::regex("-(.+)"))) //引数パラメータの形をとっているか確認
            {                                                    //-X又は--xxの形式をとっていた場合
                if (std::get<DEF_VAL>(_Tuple).empty())
                { //デフォルト引数が設定されていない場合
                    fprintf_s(stderr, "%s option need argment.\n", in_argv_i.c_str());
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
                fprintf_s(stderr, "%s option need argment.\n", in_argv_i.c_str());
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
                        fprintf(stderr, "-%s option need argment.\n", target.c_str());
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
        if (isLongName(in_argv_i,sub_much))//long name
        {
            std::string target = sub_much.str(1);//マッチさせる対象
            for (auto &f : flag_items_m)//ここのループに改善の余地あり
            {
                if (std::regex_match(target, std::get<LONG_NAME>(f.second)))//long nameで設定した文字列のいずれかに当てはまるか確認
                {
                    if(!__spaceArgSet(f.second,argc,argv,i,in_argv_i))
                    {
                        return 0;
                    }
                    std::get<IS_EXIST>(f.second) = true;
                }
            }
        }
        else if(isShortName(in_argv_i,sub_much))//short name
        {
            std::string target = sub_much.str(1);//マッチさせる対象
            for(auto &f : flag_items_m)
            {
                if (std::regex_match(target, std::get<SHORT_NAME>(f.second)))
                {
                    if (!__spaceArgSet(f.second, argc, argv, i, in_argv_i))
                    {
                        return 0;
                    }
                    std::get<IS_EXIST>(f.second) = true;
                }
            }
        }
        else if(isCompoundShortName(in_argv_i,sub_much))//複合short name
        {
            if(!__compound_short_name_parse(sub_much.str(1)))
            {
                return 0;
            }
        }
        else//simple string
        {
            if (!__simple_string_parse(in_argv_i))
            {
                return 0;
            }
        }
    }//end for
    return 1;
}

bool Command::_equal_parse(int argc, const char *argv[]){
    
    for(int i = 1; i < argc; i++)
    {
        std::string in_argv_i(argv[i]);
        std::smatch sub_much;
        if (isLongName(in_argv_i, sub_much)) //long
        {
            std::string target = sub_much.str(1);
            std::string target_arg = sub_much.str(3);
            for (auto &f : flag_items_m)
            {
                if (std::regex_match(target, std::get<LONG_NAME>(f.second))) //long nameで設定した文字列のいずれかに当てはまるか確認
                {
                    if (!__equalArgSet(f.second, target, target_arg))
                    {
                        return 0;
                    }
                    std::get<IS_EXIST>(f.second) = true;
                }
            }
        }
        else if (isShortName(in_argv_i, sub_much)) //short
        {
            std::string target = sub_much.str(1); //マッチさせる対象
            std::string target_arg = sub_much.str(3);
            for (auto &f : flag_items_m)
            {
                if (std::regex_match(target, std::get<SHORT_NAME>(f.second)))
                {
                    if (!__equalArgSet(f.second,target,target_arg))
                    {
                        return 0;
                    }
                    std::get<IS_EXIST>(f.second) = true;
                }
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
                fprintf(stderr, "%s can not given argment.\n", in_argv_i.c_str());
                return 0;
            }
        }
        else// simple string
        {
            if (!__simple_string_parse(in_argv_i))
            {
                return 0;
            }
        }
    }
    return 1;
}

Command &Command::flag(std::string tag_name, std::vector<char> short_name, std::vector<std::string> long_name, std::string description_message, bool need_arg, std::string default_val){
    std::string str;
    if(short_name.size() != 0){
        str.push_back('(');
        for (auto &c : short_name)
        {
            str.push_back(c);
            str.push_back('|');
        }
        str.pop_back();
        str.push_back(')');
        fprintf_s(stderr,"%s\n",str.c_str());
    }
    std::regex short_names(str.begin(), str.end());
    str.clear();
    if(long_name.size() !=0){
        str.push_back('(');
        for (auto &c : long_name)
        {
            str += c;
            str.push_back('|');
        }
        str.pop_back();
        str.push_back(')');
        fprintf_s(stderr, "%s\n", str.c_str());
    }
    std::regex long_names(str.begin(), str.end());
    flag_items_m[tag_name] = std::make_tuple(short_names,long_names,description_message,need_arg,default_val,0,"");
    return *this;
}
bool Command::has(std::string tag_name){
    return std::get<IS_EXIST>(flag_items_m[tag_name]);
}

template<class T>
T Command::get(std::string tag_name){
    return (T)std::get<ARG_VAL>(flag_items_m[tag_name]);
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