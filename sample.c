#include "sugarless.h"


int main(int argc, char const *argv[])
{
    Command cmd = sugarless_create_command(argv[0]);
    printf("cmd.name = %s\n",cmd.name);
    Flag h = sugarless_create_flag('h', "h", "help", false, NULL, NULL);
    Flag v = sugarless_create_flag('v',"v","version",false,NULL,NULL);
    Flag i = sugarless_create_flag('i',"i","input",true,NULL,"input file");
    Flag o = sugarless_create_flag('o',"o","output",true,NULL,"output file");
    printf("h.id = %c\n",h.id);
    sugarless_set_flag(&cmd,&h);
    sugarless_set_flag(&cmd, &v);
    sugarless_set_flag(&cmd, &i);
    sugarless_set_flag(&cmd, &o);
    printf("cmd.numflags = %d\n", cmd.numflags);

    sugarless_parse(&cmd,argc,argv,NULL);
    printf("h.long_name = %s\n", h.long_name);

    if(sugarless_has_flag(&cmd,'h'))
    {
        sugarless_show_help(&cmd);
        printf("has h\n");
    }
    printf("h.take_arg = %d\n", h.take_arg);
    return 0;
}
