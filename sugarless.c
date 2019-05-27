#include "sugarless.h"

#define SUGARLESS_ALEN 32

const int SUGARLESS_SHORT_OPTION_TOKEN_LEN = strlen(SUGARLESS_SHORT_OPTION_TOKEN);
const int SUGARLESS_LONG_OPTION_TOKEN_LEN = strlen(SUGARLESS_LONG_OPTION_TOKEN);

typedef struct sugarless_flag
{
    char id;
    char const *short_name;
    char const *long_name;
    bool take_arg;
    bool is_exist;
    char const *arg;
    char const *default_arg;
    char const *message;
} Flag;

typedef struct sugarless_command
{
    char const *name;
    struct sugarless_command **subcommands;
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

// utils
void *safe_realloc(void *__ptr, size_t __size, size_t *__alloc_size)
{
    void *alptr = NULL;
    while ((alptr = realloc(__ptr, __size)) == NULL)
        --__size;
    *__alloc_size = __size;
    return alptr;
}
// end utils

Flag *sugarless_flag_create(char id, char const *short_name, char const *long_name, bool take_arg, char const *default_arg)
{
    Flag *flg = malloc(sizeof(*flg));
    flg->id = id;
    flg->short_name = short_name;
    flg->long_name = long_name;
    flg->take_arg = take_arg;
    flg->default_arg = default_arg;
    flg->is_exist = false;
    return flg;
}
Flag *sugarless_flag_copy(const Flag *flag)
{
    Flag *copied = malloc(sizeof(*copied));
    memcpy(copied, flag, sizeof(*copied));
    return copied;
};
void sugarless_flag_set_id(Flag *flag, char id)
{
    flag->id = id;
}
void sugarless_flag_set_short_name(Flag *flag, char const *short_name)
{
    flag->short_name = short_name;
}
void sugarless_flag_set_long_name(Flag *flag, char const *long_name)
{
    flag->long_name = long_name;
}
void sugarless_flag_set_take_arg(Flag *flag, bool take_arg)
{
    flag->take_arg = take_arg;
}
void sugarless_flag_set_default_arg(Flag *flag, char const *default_arg)
{
    flag->default_arg = default_arg;
}
void sugarless_flag_destroy(Flag *flag)
{
    free(flag);
}

Command *sugarless_command_create(char const *name)
{
    Command *cmd = malloc(sizeof(*cmd));
    cmd->name = (char *)name;
    cmd->flags = malloc(sizeof(Flag **) * SUGARLESS_ALEN);
    cmd->numflags = 0;
    cmd->flglen = SUGARLESS_ALEN;
    cmd->subcommands = malloc(sizeof(Command **) * SUGARLESS_ALEN);
    cmd->numsubcommands = 0;
    cmd->sublen = SUGARLESS_ALEN;
    cmd->parsed = false;
    cmd->others = malloc(sizeof(char const **) * SUGARLESS_ALEN);
    cmd->numothers = 0;
    cmd->otherslen = SUGARLESS_ALEN;
    return cmd;
}
Command *sugarless_command_copy(const Command *cmd)
{
    Command *copied = malloc(sizeof(*copied));
    memcpy(copied, cmd, sizeof(*copied));
    copied->flags = malloc(sizeof(Flag **) * cmd->flglen);
    for (int i = 0; i < copied->numflags; i++)
    {
        copied->flags[i] = sugarless_flag_copy(cmd->flags[i]);
    }
    copied->subcommands = malloc(sizeof(Command **) * cmd->sublen);
    for (size_t i = 0; i < copied->numsubcommands; i++)
    {
        copied->subcommands[i] = sugarless_command_copy(copied->subcommands[i]);
    }
    copied->others = malloc(sizeof(char const **) * cmd->otherslen);
    memcpy(copied->others, cmd->others, sizeof(char const **) * cmd->numothers);
    return copied;
}

void sugarless_command_set_name(Command *cmd, char const *name)
{
    cmd->name = name;
}
void sugarless_command_set_flag(Command *cmd, Flag *flag)
{
    if (cmd->numflags == cmd->flglen)
    {
        cmd->flglen *= 2;
        cmd->flags = safe_realloc(cmd->flags, cmd->flglen, (size_t *)&(cmd->flglen));
    }
    cmd->flags[cmd->numflags] = flag;
    ++(cmd->numflags);
}
void sugarless_command_set_sub(Command *cmd, Command *sub)
{
    if (cmd->numsubcommands == cmd->sublen)
    {
        cmd->sublen *= 2;
        cmd->subcommands = safe_realloc(cmd->subcommands, cmd->sublen, (size_t *)&(cmd->sublen));
    }
    cmd->subcommands[cmd->numsubcommands] = sub;
    ++(cmd->numsubcommands);
}
void sugarless_command_destroy(Command *cmd)
{
    free(cmd);
}
void sugarless_set_others(Command *cmd, char const *arg)
{
    if (cmd->numothers == cmd->otherslen)
    {
        cmd->otherslen *= 2;
        cmd->others = safe_realloc(cmd->others, cmd->otherslen, (size_t *)&(cmd->otherslen));
    }
    cmd->others[cmd->numothers] = arg;
    ++(cmd->numothers);
}

bool sugarless_command_has_flag(Command *cmd, char flag_id)
{
    Flag *flg = sugarless_command_get_flag(cmd, flag_id);
    if (flg == NULL)
        return false;
    return flg->is_exist;
}
bool sugarless_command_has_sub(Command *cmd, Command *sub)
{
    return sugarless_command_has_sub_by_name(cmd, sub->name);
}
bool sugarless_command_has_sub_by_name(Command *cmd, char const *name)
{
    Command *sub = sugarless_command_get_sub_by_name(cmd, name);
    if (sub == NULL)
        return false;
    return sub->parsed;
}

Flag *sugarless_command_get_flag(Command *cmd, char flag_id)
{
    for (int i = 0; i < cmd->numflags; ++i)
    {
        if (cmd->flags[i]->id == flag_id)
        {
            return cmd->flags[i];
        }
    }
    return NULL;
}
Command *sugarless_command_get_sub_by_name(Command *cmd, char const *name)
{
    for (int i = 0; i < cmd->numsubcommands; ++i)
    {
        if (!strcmp(cmd->subcommands[i]->name, name))
        {
            return cmd->subcommands[i];
        }
    }
    return NULL;
}
char const **sugarless_command_get_argv(const Command *cmd)
{
    return cmd->others;
}
int sugarless_command_get_argc(const Command *cmd)
{
    return cmd->numothers;
}
char const *sugarless_command_get_flag_arg(Command *cmd, char flag_id)
{
    Flag *flg = sugarless_command_get_flag(cmd, flag_id);
    if (flg == NULL)
        return NULL;
    if (flg->arg == NULL)
        return flg->default_arg;
    return flg->arg;
}
char const *sugarless_command_get_flag_default_arg(Command *cmd, char flag_id)
{
    Flag *flg = sugarless_command_get_flag(cmd, flag_id);
    if (flg == NULL)
        return NULL;
    return flg->default_arg;
}
bool is_long_opt(char const *opt)
{
    return !strncmp(SUGARLESS_LONG_OPTION_TOKEN, opt, SUGARLESS_LONG_OPTION_TOKEN_LEN);
}
bool is_short_opt(char const *opt)
{
    return !strncmp(SUGARLESS_SHORT_OPTION_TOKEN, opt, SUGARLESS_SHORT_OPTION_TOKEN_LEN);
}
bool is_opt(char const *opt)
{
    return is_long_opt(opt) || is_short_opt(opt);
}
bool is_safe_flag(char const *opt)
{
    return !strcmp(SUGARLESS_SAFE_TOKEN, opt);
}
bool accsessable_next(int argc, int i)
{
    return argc > i;
}
Flag *search_long(Command *cmd, char const *arg_name)
{
    for (int i = 0; i < cmd->numflags; ++i)
    {
        if (!strcmp(cmd->flags[i]->long_name, arg_name))
        {
            cmd->flags[i]->is_exist = true;
            return cmd->flags[i];
        }
    }
    return NULL;
}
Flag *search_short(Command *cmd, char const *arg_name)
{
    for (int i = 0; i < cmd->numflags; ++i)
    {
        if (!strncmp(cmd->flags[i]->short_name, arg_name, 1))
        {
            cmd->flags[i]->is_exist = true;
            return cmd->flags[i];
        }
    }
    return NULL;
}
Command *search_sub(Command *cmd, char const *arg)
{
    for (int i = 0; i < cmd->numsubcommands; ++i)
    {
        if (!strcmp(cmd->subcommands[i]->name, arg))
        {
            return cmd->subcommands[i];
        }
    }
    return NULL;
}
bool set_arg(Flag *flg, int *i, int argc, char const *argv[])
{
    if (flg->take_arg)
    {
        if (accsessable_next(argc, *i))
        {
            flg->arg = (char *)argv[++(*i)];
            return true;
        }
        else
        {
            return false;
        }
    }
    return true;
}

bool sugarless_parse(Command *cmd, int argc, char const *argv[])
{
    bool safe = false;
    for (int i = 1; i < argc; ++i)
    {
        printf("IN PARSE %s\n", argv[i]);
        Command *sub;
        if (is_safe_flag(argv[i]))
        {
            safe = true;
            continue;
        }
        else if (!safe && is_long_opt(argv[i]))
        {
            char const *arg_name = argv[i] + SUGARLESS_LONG_OPTION_TOKEN_LEN;
            Flag *flg = search_long(cmd, arg_name);
            set_arg(flg, &i, argc, argv);
        }
        else if (!safe && is_short_opt(argv[i]))
        {
            char const *arg_name = argv[i] + SUGARLESS_SHORT_OPTION_TOKEN_LEN;
            printf("short opt %s\n", arg_name);
            Flag *flg;
            while (strlen(arg_name))
            {
                flg = search_short(cmd, arg_name);
                ++arg_name;
                printf("short opt %s\n", arg_name);
            }
            set_arg(flg, &i, argc, argv);
        }
        else if ((sub = search_sub(cmd, argv[i])) != NULL)
        {
            sugarless_parse(sub, argc - i, &argv[i]);
            break;
        }
        else
        {
            sugarless_set_others(cmd, argv[i]);
        }
        safe = false;
    }
    cmd->parsed = true;
    return true;
}
void sugarless_destroy_all(Command *cmd)
{
    for (int i = 0; i < cmd->numsubcommands; ++i)
    {
        sugarless_destroy_all(cmd->subcommands[i]);
    }
    free(cmd->subcommands);
    for (int i = 0; i < cmd->numflags; ++i)
    {
        sugarless_flag_destroy(cmd->flags[i]);
    }
    free(cmd->flags);
}

void sugarless_flag_print(const Flag *flag)
{
    printf("id:%c\tshort_name:%s\tlong_name:%s\ttake_arg:%d\tis_exist:%d\tdefault_arg:%s\targ:%s\n",
           flag->id,
           flag->short_name,
           flag->long_name,
           flag->take_arg,
           flag->is_exist,
           flag->default_arg,
           flag->arg);
}
void sugarless_command_print(const Command *cmd)
{
    printf("name:%s\tnumsub:%d\tnumflag:%d\tnumothers:%d\tparsed:%d\n",
           cmd->name,
           cmd->numsubcommands,
           cmd->numflags,
           cmd->numothers,
           cmd->parsed);
}
