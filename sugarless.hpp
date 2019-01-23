#pragma once
#ifndef SUGARLESS_HPP
#define SUGARLESS_HPP

#include <map>
#include <string>
#include <vector>
#include <ostream>


namespace sugarless
{
const static std::string empty_str("");
const static std::string short_token("-");
const static std::string connect_token("=");
const static std::string long_token("--");
const static std::string argument_flag("--");

template<class _Elme>
inline size_t partition(std::basic_string<_Elme> target,std::basic_string<_Elme> sep, std::basic_string<_Elme> &dst1, std::basic_string<_Elme> &dst2, std::basic_string<_Elme> &dst3)
{
  size_t index = target.find(sep);
  if (index == std::basic_string<_Elme>::npos)
  {
    dst1 = target;
    dst2 = empty_str;
    dst3 = empty_str;
  }
  else
  {
    dst1 = target.substr(0, index);
    dst2 = sep;
    dst3 = target.substr(index + sep.size());
  }
  return index;
}

template<class _Elme>
inline bool startswith(const std::basic_string<_Elme> target,const std::basic_string<_Elme> suffix, int start=0, int end=INT_MAX)
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
    char short_name;
    std::string long_name;
    std::string argument;
    bool require_argument;
    bool exist;
    Flag(void){};
    Flag(const char short_name,const char * long_name,bool require_argument,const char * argument)
    {
        this->short_name = short_name;
        this->long_name = long_name;
        this->require_argument = require_argument;
        this->argument = argument;
        this->exist = false;
    }
};

enum
{
    OK,
    MISSING_ARGUMENT,
    INVALID_FLAG,
};

const static char * OK_MESSAGE = "parse complete";
const static char * MISSING_ARGUMENT_MESSAGE = "require argument";
const static char * INVALID_FLAG_MESSAGE = " invalid option";

const char * get_error_message(int errornumber)
{
    switch (errornumber)
    {
        case OK:
            return OK_MESSAGE;
        case MISSING_ARGUMENT:
            return MISSING_ARGUMENT_MESSAGE;
        case INVALID_FLAG:
            return INVALID_FLAG_MESSAGE;
        default:
            break;
    }
    return OK_MESSAGE;
}

class Command
{
  private:
    std::map<std::string,Flag,std::less<>> flags;
    std::map<std::string,Command*,std::less<>> sub_commands;
    bool exist;

    bool is_long_name(std::string &name);
    bool is_short_name(std::string &name);
    bool is_opt(std::string &name);

  public:
    std::vector<char *> others;

    int parse(int argc,char const* argv[],int position=1);
    Command &flag(const char *id, const char short_name, const char *long_name, bool require_argument=false, const char * default_argument=empty_str.c_str());
    Command &flag(const char *id, const char short_name, const char *long_name, const char * default_argument);
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
    for(auto &&subc:self.sub_commands)
    {
        stream << std::boolalpha
                << subc.first << " ::: " << std::endl
                << *(subc.second) << std::endl 
                << std::noboolalpha;
    }
    
    for(auto &&flg: self.flags )
    {
        stream << std::boolalpha
                << flg.first << " : " << flg.second.exist << std::endl
                << "argumens : " << flg.second.argument << std::endl
                << std::noboolalpha;
    }
    return stream;
}

/* end operator */


inline bool Command::is_long_name(std::string &name)
{
    return startswith(name, long_token);
}

inline bool Command::is_short_name(std::string &name)
{
    return startswith(name,short_token);
}

inline bool Command::is_opt(std::string &name)
{
    return this->is_short_name(name) || this->is_short_name(name);
}


int Command::parse(int argc,char const* argv[],int position)
{
    bool forced_arg = false; // -- がよく前に指定された場合T、次は必ず引数として振る舞うかあるいはothersに突っ込まれる
    bool req_arg = false;//直前のオプションが引数を必要とする場合にT
    bool maybe_arg = false; // デフォルト引数持ちでかつ引数が指定されてない場合に引数かも知れない

    std::string previous_flag_id = empty_str;//直前のオプションが引数を要求する場合のフラグID
    for(; position < argc; ++position)
    {
        std::string strargv(argv[position]);
        if(!forced_arg && strargv == argument_flag)
        {
            forced_arg = true;
            continue;
        }
        if(forced_arg)
        {
            goto ARGUMENTS;
        }
        if(this->is_long_name(strargv))
        {// long
            for(auto&& flg:this->flags)
            {
                if (startswith(strargv,flg.second.long_name,long_token.size() ))
                {// 引数の先頭がロング名と一致する場合
                    flg.second.exist = true;
                    
                    if (flg.second.require_argument)
                    {// 引数を要求するオプションの場合
                        //ここで＝分割指定と＝無し結合指定を探す
                        std::string l,r,sep;
                        partition(strargv.substr(long_token.size() ),connect_token,l,sep,r);//--部分を取り除いたコマンドラインからのロング名
                        if (!r.empty())
                        {//右側が空でなければそれを引数の値として確定する
                            flg.second.argument = r;
                        }// end if
                        else if (r.empty() && l.size() > flg.second.long_name.size() )
                        {//右側が空、かつ左がロング名より長い場合は左側の先頭からロング名の長さ分削ったものを引数の値とする
                            flg.second.argument = l.substr(flg.second.long_name.size());
                        }// end else if
                        else
                        {//綺麗にロング名だけを指定された場合
                            previous_flag_id = flg.first;
                            req_arg = true;
                            if (! flg.second.argument.empty())
                            {// 引数を要求するオプションでかつ、デフォルト引数が設定されている場合
                             // この場合は、次がオプション形式の書き方をしていなければその値を引数とする
                                maybe_arg = true;
                            }
                        }// end else
                    }// end if
                    break;// ロング名とマッチしているからそれ以上の比較は必要ないためループ終了
                }// end if
            }// end for
        }// end if
        else if (this->is_short_name(strargv))// short
        {
            size_t slen = strargv.size();
            bool req_arg_s = false;
            for(size_t i = short_token.size(); i < slen; ++i)
            {
                bool short_muched = false;
                for(auto&& flg:this->flags)
                {
                    if (strargv[i] == flg.second.short_name)
                    {
                        flg.second.exist = true;
                        req_arg_s = flg.second.require_argument;
                        previous_flag_id = flg.first;
                        short_muched = true;
                        break;
                    }
                }
                if(!short_muched && req_arg_s)
                {//直前のオプションが引数をとる場合
                    // 現在のstrargv[i]が他のショート名に含まれていなければそれ以降を引数とする
                    this->flags[previous_flag_id].argument = strargv.substr(i);
                    break;
                }
            }
            req_arg = req_arg_s;
        }
        else if(req_arg)
        {
            this->flags[previous_flag_id].argument = strargv;
        }
        else
        {//何のオプションにも該当しない場合
        
            for(auto&& sb:this->sub_commands)
            {//いずれのオプションにもマッチしなかった場合にサブコマンドの可能性を探る
                if(sb.first==strargv)
                {
                    sb.second->exist = true;
                    return sb.second->parse(argc,argv,position);
                }
            }
            ARGUMENTS:
            if (maybe_arg)
            {//直前のオプションが引数を要求しているかつ、デフォルト引数が設定されている場合
                if(!this->is_opt(strargv))
                {
                    this->flags[previous_flag_id].argument = strargv;
                }
                maybe_arg = false;
            }
            else if (req_arg)
            {//直前のオプションが引数を要求する場合
                this->flags[previous_flag_id].argument = strargv;
                if(!this->is_opt(strargv))
                {
                    return MISSING_ARGUMENT;
                }
                maybe_arg = false;
                req_arg = false;
            }
            else
            {//全てに当てはまらない
                char * tmp = const_cast<char*>(argv[position]);
                this->others.push_back(tmp);
            }
        }
    }
    return OK;
}

Command &Command::flag(const char *id, const char short_name, const char *long_name, bool require_argument, const char * default_argument)
{
    //TODO:NULL chack frase
    this->flags[std::string(id)] = Flag(short_name,long_name,require_argument,default_argument);
    return *this;
}
Command &Command::flag(const char *id, const char short_name, const char *long_name, const char * default_argument)
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
    this->sub_commands[std::string(command_name)] = &cmd;
    return *this;
}
bool Command::has_sub_command(const char*command_name)
{
    return this->sub_commands[std::string(command_name)]->exist;
}

Command Command::get_subcommand(const char *command_name)
{
    return *(this->sub_commands[std::string(command_name)]);
}




}// end nemaspace sugarless

#endif //include gard SUGARLESS_HPP