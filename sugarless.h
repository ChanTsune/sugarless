#pragma once
#ifndef SUGARLESS_H
#define SUGARLESS_H
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SUGARLESS_SHORT_OPTION_TOKEN "-"
#define SUGARLESS_LONG_OPTION_TOKEN "--"
#define SUGARLESS_SAFE_TOKEN "--"

typedef struct sugarless_flag Flag;

typedef struct sugarless_command Command;

Flag *sugarless_flag_create(char id, char const *short_name, char const *long_name, bool take_arg, char const *default_arg);
Flag *sugarless_flag_copy(const Flag *flag);
void sugarless_flag_set_id(Flag *flag, char id);
void sugarless_flag_set_short_name(Flag *flag, char const *short_name);
void sugarless_flag_set_long_name(Flag *flag, char const *long_name);
void sugarless_flag_set_take_arg(Flag *flag, bool take_arg);
void sugarless_flag_set_default_arg(Flag *flag, char const *default_arg);
void sugarless_flag_destroy(Flag *flag);

Command *sugarless_command_create(char const *name);
Command *sugarless_command_copy(const Command *cmd);

void sugarless_command_set_name(Command *cmd, char const *name);
void sugarless_command_set_flag(Command *cmd, Flag *flag);
void sugarless_command_set_sub(Command *cmd, Command *sub);
void sugarless_command_destroy(Command *cmd);

bool sugarless_command_has_flag(Command *cmd, char flag_id);
bool sugarless_command_has_sub(Command *cmd, Command *sub);
bool sugarless_command_has_sub_by_name(Command *cmd, char const *name);

Flag *sugarless_command_get_flag(Command *cmd, char flag_id);
Command *sugarless_command_get_sub_by_name(Command *cmd, char const *name);
char const **sugarless_command_get_argv(const Command *cmd);
int sugarless_command_get_argc(const Command *cmd);
char const *sugarless_command_get_flag_arg(Command *cmd, char flag_id);
char const *sugarless_command_get_flag_default_arg(Command *cmd, char flag_id);

bool sugarless_parse(Command *cmd, int argc, char const *argv[]);

void sugarless_flag_print(const Flag *flag);
void sugarless_command_print(const Command *cmd);
void sugarless_destroy_all(Command *cmd);

#endif // SUGARLESS_H include garde
