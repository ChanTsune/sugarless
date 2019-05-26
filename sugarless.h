#pragma once
#ifndef SUGARLESS_H
#define SUGARLESS_H
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SUGARLESS_SHORT_OPTION_TOKEN "-"
#define SUGARLESS_LONG_OPTION_TOKEN  "--"
#define SUGARLESS_SAFE_TOKEN         "--"

typedef struct flag
{
    char id;
    char *short_name;
    char *long_name;
    bool take_arg;
    bool is_exist;
    char *arg;
    char *default_arg;
    char *message;
} Flag;

typedef struct command
{
    char *name;
    struct command **subcommands;
    int numsubcommands;
    int sublen;
    Flag **flags;
    int numflags;
    int flglen;
    bool parsed;
    char const **others;
    int otherslen;
    int numothers;
} Command;

Command sugarless_create_command(char *name);

int sugarless_set_subcommand(Command *main, Command *sub);

int sugarless_destroy_command(Command *cmd);

Flag sugarless_create_flag(char id, char *short_namm, char *long_name, bool take_arg, char *default_arg, char *message);

int sugarless_set_flag(Command *cmd, Flag *flg);

int sugarless_destroy_flag(Flag *flg);

bool sugarless_parse(Command *cmd, int argc, char const *argv[], char **error_message);

bool sugarless_has_flag(Command *cmd, char id);

char const *sugarless_get_flag_argument(Flag *flg);

char const *sugarless_get_flag_default_argument(Flag *flg);

bool sugarless_has_subcommand(Command *cmd, Command *sub);

bool sugarless_has_subcommand_by_name(Command *cmd, char *sub);

void sugarless_show_help(Command *cmd);

#endif // SUGARLESS_H include garde
