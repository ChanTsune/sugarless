#include "sugarless.hpp"
#include <iostream>

using namespace std;
int main(int argc, char const *argv[])
{
    sugarless::Command git,clone;
    git.flag("overwrite", "w", "overwrite")
        .flag("output", "o", "output", "./a.out")
        .flag("max", "M", "max", true)
        .sub_command("clone",clone,true);

    return 0;
}
