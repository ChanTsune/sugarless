#include <map>
#include <cstring>
#include <vector>
#include <string>

#include <iostream>

#include "sugarless.hpp"

namespace sugarless
{
char const static *SHORT_OPTION_TOKEN = "-";
char const static *LONG_OPTION_TOKEN = "--";
char const static *SAFE_TOKEN = "--";
const int SHORT_OPTION_TOKEN_LEN = strlen(SHORT_OPTION_TOKEN);
const int LONG_OPTION_TOKEN_LEN = strlen(LONG_OPTION_TOKEN);
const int SAFE_TOKEN_LEN = strlen(SAFE_TOKEN);

Flag::Flag(void) : id('\0'), short_name(NULL), long_name(NULL), take_arg(false), is_exist(false), arg(NULL), default_arg(NULL) {}
Flag::Flag(char id, char const *short_name, char const *long_name, bool take_arg, char const *default_arg)
    : id(id), short_name(short_name), long_name(long_name), take_arg(take_arg), is_exist(false), arg(NULL), default_arg(default_arg){};
Command::Command(void) : name(NULL){};
Command::Command(char const *name) : name(name){};

bool is_long_opt(char const *opt)
{
    return !strncmp(LONG_OPTION_TOKEN, opt, LONG_OPTION_TOKEN_LEN);
}
bool is_short_opt(char const *opt)
{
    return !strncmp(SHORT_OPTION_TOKEN, opt, SHORT_OPTION_TOKEN_LEN);
}
bool is_opt(char const *opt)
{
    return is_long_opt(opt) || is_short_opt(opt);
}
bool is_safe_flag(char const *opt)
{
    return !strcmp(SAFE_TOKEN, opt);
}
bool accsessable_next(int argc, int i)
{
    return argc > i;
}
Flag Command::search_long(char const *arg_name)
{
    Flag empty;
    for (auto &&flag : this->flags)
    {
        if (!strcmp(flag.second.long_name, arg_name))
        {
            this->flags[flag.second.id].is_exist = true;
            return this->flags[flag.second.id];
        }
    }
    return empty;
}
Flag Command::search_short(char const *arg_name)
{
    Flag empty;
    for (auto &&flag : this->flags)
    {
        if (!strcmp(flag.second.short_name, arg_name))
        {
            this->flags[flag.second.id].is_exist = true;
            return this->flags[flag.second.id];
        }
    }
    return empty;
}
Command Command::search_sub(char const *arg)
{
    return this->subcommands[arg];
}
Flag &Flag::set_arg(int &i, int argc, char const *argv[])
{
    if (this->take_arg && accsessable_next(argc, i))
        this->arg = (char *)argv[++i];
    return *this;
}

bool Command::parse(int argc, char const *argv[])
{
    bool safe = false;
    for (int i = 1; i < argc; ++i)
    {
        Command sub;
        if (is_safe_flag(argv[i]))
        {
            safe = true;
            continue;
        }
        else if (!safe && is_long_opt(argv[i]))
        {
            char const *arg_name = argv[i] + LONG_OPTION_TOKEN_LEN;
            Flag flg = this->search_long(arg_name); // 展開しなきゃダメな気がする
            flg.set_arg(i, argc, argv);
        }
        else if (!safe && is_short_opt(argv[i]))
        {
            char const *arg_name = argv[i] + SHORT_OPTION_TOKEN_LEN;
            Flag flg;
            while (strlen(arg_name))
            {
                flg = this->search_short(arg_name); // 展開しなきゃダメな気がする
                ++arg_name;
            }
            flg.set_arg(i, argc, argv);
        }
        else if ((sub = this->search_sub(argv[i])).name != NULL) // 展開しなきゃダメな気がする
        {
            sub.parse(argc - i, &argv[i]);
            break;
        }
        else
        {
            this->others.push_back(argv[i]);
        }
        safe = false;
    }
    this->parsed = true;
    return true;
}
Command &Command::flag(char id, char const *short_name, char const *long_name, bool take_arg, char const *default_arg)
{
    Flag flg(id, short_name, long_name, take_arg, default_arg);
    this->set(flg);
    return *this;
}

Command &Command::set(Flag flg)
{
    this->flags.emplace(flg.id,flg);
    return *this;
}
Command &Command::set(Command subcommand)
{
    this->subcommands.emplace(subcommand.name,subcommand);
    return *this;
}
bool Command::has(char flag_id)
{
    using namespace std;
    cout << "called  : " << this->flags[flag_id].is_exist << endl;
    return this->flags[flag_id].is_exist;
}
bool Command::has(char const *sub_name)
{
    return this->subcommands[sub_name].parsed;
}
char const *Command::get(char flag_id)
{
    Flag flg = this->flags[flag_id];
    if (flg.arg == NULL)
        return flg.default_arg;
    return flg.arg;
}
char const *Command::get_default(char flag_id)
{
    return this->flags[flag_id].default_arg;
}
Command Command::get(char const *sub_name)
{
    return this->subcommands[sub_name];
}

} // namespace sugarless
