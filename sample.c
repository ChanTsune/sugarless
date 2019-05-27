#include "sugarless.h"

int main(int argc, char const *argv[])
{
    Command *main = sugarless_command_create(argv[0]);
    sugarless_command_set_flag(main,
                               sugarless_flag_create('h', "h", "help", false, NULL));

    sugarless_parse(main, argc, argv);
    if (sugarless_command_has_flag(main, 'h'))
    {
        sugarless_command_print(main);
        sugarless_flag_print(sugarless_command_get_flag(main, 'h'));
    }
    sugarless_destroy_all(main);
    return 0;
}
