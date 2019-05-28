#include <stdio.h>
#include <stdlib.h>

#include "sugarless.h"

int main(int argc, char const *argv[])
{
    SLCommand *cmd = sugarless_command_create(argv[0]);
    sugarless_command_set_flag(cmd,
                               sugarless_flag_create('h', "h", "help", false, NULL));
    sugarless_command_set_flag(cmd,
                               sugarless_flag_create('v', "v", "version", false, NULL));
    SLCommand *sub = sugarless_command_copy(cmd);
    sugarless_command_set_name(sub, "sub");
    sugarless_command_set_sub(cmd, sub);
    sugarless_command_set_flag(cmd,
                               sugarless_flag_create('i', "i", "input", true, NULL));
    sugarless_command_set_flag(cmd,
                               sugarless_flag_create('o', "o", "output", true, "a.out"));

    sugarless_parse(cmd, argc, argv);
    if (sugarless_command_has_sub_by_name(cmd, "sub"))
    {
        if (sugarless_command_has_flag(sub, 'h'))
        {
            sugarless_command_print(sub);
            sugarless_flag_print(sugarless_command_get_flag(sub, 'h'));
        }
        if (sugarless_command_has_flag(sub, 'v'))
        {
            sugarless_flag_print(sugarless_command_get_flag(sub, 'v'));
        }
        sugarless_destroy_all(cmd);
        return 0;
    }
    if (sugarless_command_has_flag(cmd, 'h'))
    {
        sugarless_command_print(cmd);
        sugarless_flag_print(sugarless_command_get_flag(cmd, 'h'));
    }
    if (sugarless_command_has_flag(cmd, 'v'))
    {
        sugarless_flag_print(sugarless_command_get_flag(cmd, 'v'));
    }
    if (sugarless_command_has_flag(cmd,'i'))
    {
        sugarless_flag_print(sugarless_command_get_flag(cmd, 'i'));
    }
    if (sugarless_command_has_flag(cmd,'o'))
    {
        sugarless_flag_print(sugarless_command_get_flag(cmd, 'o'));
    }

    sugarless_destroy_all(cmd);
    return 0;
}
