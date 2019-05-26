#include "sugarless.h"


int main(int argc, char const *argv[])
{
    Command cmd = sugarless_create_command(argv[0]);
    printf("cmd.name = %s\n",cmd.name);
    printf("cmd.numsubcommands = %d\n", cmd.numsubcommands);
    Flag h = sugarless_create_flag('h', "h", "help", false, NULL, NULL);
    Flag v = sugarless_create_flag('v',"v","version",false,NULL,NULL);
    Flag i = sugarless_create_flag('i',"i","input",true,NULL,"input file");
    Flag o = sugarless_create_flag('o',"o","output",true,NULL,"output file");
    printf("h.id = %c\n",h.id);
    sugarless_set_flag(&cmd,&h);
    printf("cmd.numsubcommands = %d\n", cmd.numsubcommands);
    sugarless_set_flag(&cmd, &v);
    printf("cmd.numsubcommands = %d\n", cmd.numsubcommands);
    sugarless_set_flag(&cmd, &i);
    printf("cmd.numsubcommands = %d\n", cmd.numsubcommands);
    sugarless_set_flag(&cmd, &o);
    printf("cmd.numsubcommands = %d\n", cmd.numsubcommands);
    printf("h.short_name = %s\n", h.short_name);
    printf("cmd.numsubcommands = %d\n", cmd.numsubcommands);

    sugarless_parse(&cmd,argc,argv,NULL);
    printf("h.long_name = %s\n", h.long_name);

    if(sugarless_has_flag(&cmd,'h'))
    {
        sugarless_show_help(&cmd);
    }
    printf("h.take_arg = %d\n", h.take_arg);
    return 0;
}
