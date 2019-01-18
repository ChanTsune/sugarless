#pragma once
#ifndef SUGARLESS_HPP
#define SUGARLESS_HPP

#include <map>
#include <string>
#include <cstring>
#include <vector>
#include <ostream>

namespace sugarless
{
template<class _Elme>
inline bool startswith(const srd::basic_string<_Elme> target,const srd::basic_string<_Elme> suffix, int start=0, int end=INT_MAX)
{
  int len = target.size();
  int sublen = suffix.size();
  const _Elme *self = target.c_str();
  const _Elme *sub = suffix.c_str();

  if (start + sublen > len)
  {
    return false;
  }
  if (end - start >= sublen)
  {
    return (!std::memcmp(self + start * sizeof(_Elme), sub, sublen * sizeof(_Elme)));
  }
  return false;
}

class Flag
{
    public:
    std::string short_name;
    std::string long_name;
    std::string argument;
    bool require_argument;
    bool exist;
    Flag(void){};
    Flag(const char* short_name,const char * long_name,bool require_argument,const char * argument)
    {
        this->short_name = short_name;
        this->long_name = long_name;
        this->require_argument = require_argument;
        this->argument = argument;
        this->exist = false;
    }
};

class Command
{
  private:
    std::map<std::string,Flag,std::less<>> flags;
    std::map<std::string,Command,std::less<>> sub_commands;
    bool exist;

  public:
    std::vector<char *> others;

    bool parse(int argc,char const* argv[],int position=1);
    Command &flag(const char *id, const char *short_name, const char *long_name, bool require_argument=false, const char * default_argument="");
    Command &flag(const char *id, const char *short_name, const char *long_name, const char * default_argument);
    bool has(const char *id);
    const char *get(const char *id);

    Command &sub_command(const char *command_name,Command &cmd,bool inheritanc=false);
    bool has_sub_command(const char*command_name);
    Command get_subcommand(const char *command_name);

    template <class _Elme, class _Traits>
    friend std::basic_ostream<_Elme, _Traits> &operator<<(std::basic_ostream<_Elme, _Traits> &stream, const Command &self);
};


/* operator */
template <class _Elme, class _Traits>
std::basic_ostream<_Elme, _Traits> &operator<<(std::basic_ostream<_Elme, _Traits> &stream, const Command &self)
{
    stream  << "Parse style : " << self << std::endl
            << "Options state :" << std::endl
            << std::boolalpha
            << " tag name\t| exist\t| value\t| default value" << std::endl;


    stream << std::noboolalpha;
    return stream;
}

/* end operator */


bool Command::parse(int argc,char const* argv[],int position)
{
    bool is_arg = false; // -- がよく前に指定された場合T、次は必ず引数として振る舞うかあるいはothersに突っ込まれる
    bool req_arg = false;//直前のオプションが引数を必要とする場合にT
    bool set_default = false; // 直前のオプションにデフォルト引数が指定されている時にT
    std::string previous_flag_id="";
    for(; i < argc; ++position)
    {
        std::string strargv(argv[position]);
        if(!is_arg && strargv == "--")
        {
            is_arg = true;
            continue;
        }
        if(startswith(strargv,"--"))// long
        {
            for(auto&& flg:this->flags)
            {
                if (startswith(strargv,flg.second.long_name,2))
                {
                    flg.second.exist = true;
                    
                    if (flg.second.require_argument)
                    {
                        if (flg.second.long_name.size() > strargv.size() - 2)
                        {//ここで＝分割指定と＝無し結合指定を探す
                           split(strargv.substr(2),"=",l,r);//--部分を取り除いたコマンドラインからのロング名
                           //右側が空でなければそれを引数の値として確定する
                           //右側が空、かつ左側の長さとロング名の長さが同じ場合は--long=のような変な指定の仕方なのでエラー
                           //右側が空、かつ左がロング名より長い場合は左側の先頭からロング名の長さ分削ったものを引数の値とする
                        }
                        else//綺麗にロング名だけを指定された場合
                        {
                            req_arg = true;
                            if (/* デフォルト引数を持っていた場合*/) {
                                set_default = true;
                            }
                        }
                    }
                    else //引数を要求しないオプション
                    {//フラグを全て無効にする

                    }
                    continue;
                }
                
            }
        }
        else if ()// short
        {
            /* code */
        }
        
        else
        {
            for(auto&& sb:this->sub_commands)
            {
                if(sb.first==strargv)
                {
                    return sb.second.parse(argc,argv,position);
                }
            }
            this->others.push_back(argv[position]);
        }
    }
    return 1;
}

Command &Command::flag(const char *id, const char *short_name, const char *long_name, bool require_argument, const char * default_argument)
{
    //TODO:NULL chack frase
    this->flags[std::string(id)] = Flag(short_name,long_name,require_argument,default_argument);
    return *this;
}
Command &Command::flag(const char *id, const char *short_name, const char *long_name, const char * default_argument)
{
    this->flags[std::string(id)] = Flag(short_name,long_name,true,default_argument);
    return *this;
}
bool Command::has(const char *id)
{
    return this->flags[std::string(id)].exist;
}
const char *Command::get(const char *id)
{
    return this->flags[std::string(id)].argument.c_str();
}

Command &Command::sub_command(const char *command_name,Command &cmd,bool inheritanc)
{
    // TODO: refactor map merge
    if (inheritanc) {
        for(auto&& flg:this->flags)
        {
            cmd.flags[flg.first] = flg.second;
        }
    }
    this->sub_commands[std::string(command_name)] = cmd;
    return *this;
}
bool Command::has_sub_command(const char*command_name)
{
    return this->sub_commands[std::string(command_name)].exist;
}

Command Command::get_subcommand(const char *command_name)
{
    return this->sub_commands[std::string(command_name)];
}




}// end nemaspace sugarless

#endif //include gard SUGARLESS_HPP