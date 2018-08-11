#include "sugarless.hpp"
#include <iostream>

using namespace std;
int main(int argc, char const *argv[])
{
    sugarless::Command cmd(std::string("waifu2x++"));
        cmd.argument("in_file", "input file");
        cmd.flag("overwrite", {'w', 'r'}, {"overwrite"}, "overwrite input file")
        .flag("output", {'o'}, {"output"}, "output file name", true, "./a.out")
        .flag("max", {'M'}, {"max"}, "max size", true);

    if(!cmd.parse(argc,argv,sugarless::arg_style::UNIX_STYLE)){
        cout << "parse error" << endl;
    }

    if(!cmd.has("in_file")){
        cout << "infile must be need" << endl;
    }else{
        cout << "in_file is ";
        cout << cmd.get<string>("in_file") << endl;
    }
    
    if (cmd.has("overwrite")) {
        cout << "overwrite option is valid" << endl;
    }
    if(cmd.has("output")){
        cout << cmd.get<string>("output") << endl;
    }
    if(cmd.has("max")){
        cout << "max is" << cmd.get<string>("max") << endl;
    }

    return 0;
}
