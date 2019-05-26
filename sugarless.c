#include "sugarless.h"

#define SUGARLESS_ALEN 32

Command sugarless_create_command(char *name)
{
    Command cmd;
    cmd.name = name;
    cmd.flags = malloc(sizeof(Flag *) * SUGARLESS_ALEN);
    cmd.numflags = 0;
    cmd.flglen = SUGARLESS_ALEN;
    cmd.subcommands = malloc(sizeof(Command *) * SUGARLESS_ALEN);
    cmd.numsubcommands = 0;
    cmd.sublen = SUGARLESS_ALEN;
    cmd.parsed = false;
    cmd.others = malloc(sizeof(char const **) * SUGARLESS_ALEN);
    cmd.numothers = 0;
    cmd.otherslen = SUGARLESS_ALEN;
    return cmd;
}

int sugarless_set_subcommand(Command *main, Command *sub)
{
    if (main->numsubcommands == main->sublen)
    {
        main->sublen *= 2;
        main->subcommands = realloc(main->subcommands, main->sublen);
    }
    main->subcommands[main->numsubcommands] = sub;
    ++(main->numsubcommands);
    return 0;
}

int sugarless_set_others(Command *cmd, char const *arg)
{
    printf("sugarless_set_others called\n");
    if (cmd->numothers == cmd->otherslen)
    {
        cmd->otherslen *= 2;
        cmd->others = realloc(cmd->others, cmd->otherslen);
    }
    cmd->others[cmd->numothers] = arg;
    ++(cmd->numothers);
    return 0;
}

int sugarless_destroy_command(Command *cmd)
{
    for (size_t i = 0; i < cmd->subcommands; ++i)
    {
        sugarless_destroy_command(&cmd->subcommands[i]);
    }
    free(cmd->subcommands);
    for (size_t i = 0; i < cmd->numflags; ++i)
    {
        sugarless_destroy_flag(cmd->flags[i]);
    }
    free(cmd->flags);
    return 0;
}

Flag sugarless_create_flag(char id, char *short_namm, char *long_name, bool take_arg, char *default_arg, char *message)
{
    Flag flg;
    flg.id = id;
    flg.short_name = short_namm;
    flg.long_name = long_name;
    flg.take_arg = take_arg;
    flg.default_arg = default_arg;
    flg.message = message;
    flg.is_exist = false;
    return flg;
}

int sugarless_set_flag(Command *cmd, Flag *flg)
{
    if (cmd->numflags == cmd->flglen)
    {
        cmd->flglen *= 2;
        cmd->flags = realloc(cmd->flags, cmd->flglen);
    }
    cmd->flags[cmd->numflags] = flg;
    ++(cmd->numflags);
    return 0;
}

int sugarless_destroy_flag(Flag *flg) { return 0; }

bool is_long_opt(char const *opt)
{
    return !strncmp(SUGARLESS_LONG_OPTION_TOKEN, opt, strlen(SUGARLESS_LONG_OPTION_TOKEN));
}
bool is_short_opt(char const *opt)
{
    return !strncmp(SUGARLESS_SHORT_OPTION_TOKEN, opt, strlen(SUGARLESS_SHORT_OPTION_TOKEN));
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
    for (size_t i = 0; i < cmd->numflags; ++i)
    {
        if (!strcmp(cmd->flags[i]->long_name, arg_name))
        {
            return cmd->flags[i];
        }
    }
    return NULL;
}
Flag *search_short(Command *cmd, char const *arg_name)
{
    for (size_t i = 0; i < cmd->numflags; ++i)
    {
        if (!strcmp(cmd->flags[i]->short_name, arg_name))
        {
            return cmd->flags[i];
        }
    }
    return NULL;
}
Command *search_sub(Command *cmd, char const *arg)
{
    printf("search_sub called %d\n", cmd->numsubcommands);
    for (size_t i = 0; i < cmd->numsubcommands; ++i)
    {
        printf("loop %d", i);
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
            flg->arg = argv[++(*i)];
            return true;
        }
        else
        {
            return false;
        }
    }
    return true;
}

bool sugarless_parse(Command *cmd, int argc, char const *argv[], char **error_message)
{
    bool safe = false;
    for (int i = 0; i < argc; ++i)
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
            char *arg_name = &argv[i] + strlen(SUGARLESS_LONG_OPTION_TOKEN);
            Flag *flg = search_long(cmd, arg_name);
            set_arg(flg, &i, argc, argv);
        }
        else if (!safe && is_short_opt(argv[i]))
        {
            char *arg_name = &argv[i] + strlen(SUGARLESS_SHORT_OPTION_TOKEN);
            Flag *flg;
            while (strlen(arg_name))
            {
                flg = search_short(cmd, arg_name);
                ++arg_name;
            }
            set_arg(flg, &i, argc, argv);
        }
        else if ((sub = search_sub(cmd, argv[i])) != NULL)
        {
            sugarless_parse(sub, argc - i, &argv[i], error_message);
        }
        else
        {
            sugarless_set_others(cmd, argv[i]);
        }
        safe = false;
    }
    return true;
}

bool sugarless_has_flag(Command *cmd, char id)
{
    for (size_t i = 0; i < cmd->numflags; ++i)
    {
        if (cmd->flags[i]->id == id)
        {
            return cmd->flags[i]->is_exist;
        }
    }
    return false;
}

char const *sugarless_get_flag_argument(Flag *flg)
{
    if (flg->arg == NULL)
        return flg->default_arg;
    return flg->arg;
}

char const *sugarless_get_flag_default_argument(Flag *flg)
{
    return flg->default_arg;
}

bool sugarless_has_subcommand(Command *cmd, Command *sub)
{
    for (size_t i = 0; i < cmd->numsubcommands; ++i)
    {
        if (!strcmp(cmd->subcommands[i]->name, sub->name))
        {
            return cmd->subcommands[i]->parsed;
        }
    }
    return false;
}

bool sugarless_has_subcommand_by_name(Command *cmd, char *sub)
{
    for (size_t i = 0; i < cmd->numsubcommands; ++i)
    {
        if (!strcmp(cmd->subcommands[i]->name, sub))
        {
            return cmd->subcommands[i]->parsed;
        }
    }
    return false;
}

void sugarless_show_help(Command *cmd) {}
