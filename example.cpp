#include "sugarless.hpp"
#include <iostream>

using namespace std;
int main(int argc, char const *argv[])
{
    sugarless::Command git,clone;
    git.flag("overwrite", "w", "overwrite")
        .flag("output", "o", "output", "./a.out")
        .flag("max", "M", "max", true)
        .flag("min","m","min",true)
        .sub_command("clone",clone,true);

    int i;
    if (! (i = git.parse(argc,argv)) ) {
        sugarless::get_error_message(i);
    }
    


    cout << git << endl;

    
    if (clone.has("max")) {
        cout << "clone max:" << endl;
        cout << clone.get("max") << endl;
    }


    return 0;
}
