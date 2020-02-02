#pragma once
#ifndef SUGARLESS_HPP
#define SUGARLESS_HPP

#include <map>
#include <string>
#include <vector>
#include <ostream>
#include <memory>

namespace sugarless
{
class Command;
class Flag
{
public:
    char id;
    char const *short_name;
    char const *long_name;
    bool take_arg;

private:
    bool is_exist;

public:
    char const *arg;
    char const *default_arg;
    Flag &set_arg(int &i, int argc, char const *argv[]);

public:
    Flag(void);
    Flag(char id, char const *short_name, char const *long_name = NULL, bool take_arg = false, char const *default_arg = NULL);
    friend Command;
template <class _Elme, class _Traits>
friend std::basic_ostream<_Elme, _Traits> &operator<<(std::basic_ostream<_Elme, _Traits> &stream, const Flag &self);

};

class Command
{
private:
    char const *name;
    std::map<std::string, std::shared_ptr<Command>> subcommands;
    std::map<char const *, std::shared_ptr<Flag>> sflags;
    std::map<char const *, std::shared_ptr<Flag>> lflags;
    std::map<char, std::shared_ptr<Flag>> flags;
    bool parsed;
    std::vector<char const *> others;

    Flag search_long(char const *);
    Flag search_short(char const *);
    Command search_sub(char const *);

public:
    Command(void);
    Command(char const *name);
    Command &flag(char id, char const *short_name, char const *long_name = NULL, bool take_arg = false, char const *default_arg = NULL);
    Command &set(Flag flg);
    Command &set(Command subcommand);
    bool parse(int argc, char const *argv[]);
    bool has(char flag_id);
    bool has(char const *sub_name);
    char const *get(char flag_id);
    char const *get_default(char flag_id);
    Command get(char const *sub_name);

template <class _Elme, class _Traits>
friend std::basic_ostream<_Elme, _Traits> &operator<<(std::basic_ostream<_Elme, _Traits> &stream, const Command &self);

};

template <class _Elme, class _Traits>
std::basic_ostream<_Elme, _Traits> &operator<<(std::basic_ostream<_Elme, _Traits> &stream, const Flag &self)
{
    stream << self.id << " : " << self.is_exist << std::endl;
    return stream;
}



template <class _Elme, class _Traits>
std::basic_ostream<_Elme, _Traits> &operator<<(std::basic_ostream<_Elme, _Traits> &stream, const Command &self)
{ 
    stream << self.name << ":::" << std::endl;
    for(auto &&flg:self.flags){
        stream << std::boolalpha
                << flg.second << std::endl;
    }

    for(auto &&subc:self.subcommands)
    {
        stream << std::boolalpha
                << subc.second << std::endl 
                << std::noboolalpha;
    }
    return stream;
}

} // namespace sugarless

#endif //include gard SUGARLESS_HPP
