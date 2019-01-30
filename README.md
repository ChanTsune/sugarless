# Sugarless  
Sugarless is a simple command line parser for C++.  

### Installation  

```cpp
include "sugarless.hpp"
```

Include `sugarless.hpp` only.  

### Create Command object  

```cpp
sugarless::Command cmd;
```

### Flag setting  

```cpp
// short name and long name
cmd.flag("opt_id_1","o","option1");
// only long name
cmd.flag("opt_id_2",NULL,"option2");
// only short name
cmd.flag("opt_id_3","3",NULL);

// require argument
cmd.flag("require_argument","r",NULL,true);

// require argument(with default argument)
cmd.flag("set_default_argument1","d","default_arg1",true,"default_value");
```

Create a Command object and set options with the `flag` member function.  
The first argument is the identifier used by other member functions (has,get).  
The second argument is a short option name like "-o".  
If set NULL or empty string (""), skip short name.  
The third argument is a long option name like "--output".  
If set NULL or empty string (""), skip long name.  

### Sub command setting  
Sugarless is supported sub command like `$ git clone ~~~`  
```cpp
sugarless::Command subcmd1;
sugarless::Command subcmd2;

cmd.subcommand("subcommand_name1",subcmd1);
//Inherit parent options
cmd.subcommand("subcommand_name2",subcmd2,true);
```

The first argument is sub command name.  
The second argument is command object.  
The third argument is inheritanc.  
Case of `true`, Inherit options owned by parent.  
Case of `false`, Don't inherit options.  
Default is `false`  


### purse
```cpp
cmd.parse(argc,argv);
```
After set all of flags and sub commands, Pass `argc`,`argv` of `main` function as it is.  

### Chack options existing in recived command line options  

```cpp
cmd.has("opt_id_1")
```
If the command line strings contains `-o` or` --option1`, the above function will return `true`, otherwise it will return` false`.  

### Get received argument of option  

```cpp
cmd.get("require_argument")
```
If an option receives an argument, if there is an option character in the command line strings, it returns a pointer to the received argument string.  
If executed on an option that does not receive an argument, it returns a pointer to the empty string.  

### Chack options existing in recived command line options  

```cpp
cmd.has_subcommand("subcommand_name1")
```
If the command line strings contains `subcommand_name1`, the above function will return `true`, otherwise it will return` false`.  

### argument syntax  
Sugarless basically follows the specification of GUN  
Case of the string start with `-` is mean short option.  
Case of the string start with `--` is mean long option.  
If you want to pass the string start with `-` or `--` as an option's argument, Insert `--` immediately before.  

**Unlike GNU, only the immediately following string is not recognized as an option**.  

### syntax example  

#### options argument
```
$ app -a argument
$ app -a=argument
```
The two above are considered to be identical  

Case of seted `-a` `-b` `-c` options  
```
$ app -a -b -c
$ app -abc
```
The two above are considered to be identical  

You can connect option names and arguments, But it is conditional that the argument string does not contain duplicate characters with other shorts option names.  

Case of seted short options `-a` `-b` `-c` and `-a` recive argument  
```
$ app -astring  -> $ app -a string //OK
$ app -abstring -> $ app -a string -b //OK
$ app -aargument -> $ app -a rgument //NG
is ignored  
```
After `->` is recognition of Sugarless.  

#### sub command  
If you set sub command,  
```
$ app sub -a -b
```
In this case, recognition of sugarless is, -a and -b is sub command's options.  

```
$ app -a sub -a
```
In this case, recognition of sugarless is, -a is an application option, and -b is a sub option.  

#### case of pass an arguments of started by `-`  

The next argument of `--` is not recognized as an option.  

If you want to pass an argument `-1` to ` -a` option.  

Bad case  
```
$ app -a -1
```
In the above case, `-1` is recognized as an option.  


Correct case  
```
$ app -a -- -1
```
In the above case, `-1` is recognized as an argument to `-a`.  

### example code  
Reproduc part of the command of git.  
`clone` and `init`

```cpp
#include "sugarless.hpp"

using namespace sugaless;

int main(int argc, char const *argv[])
{
    Command git,clone,init;

    clone.flag("local","l","local")
         .flag("shared","s","shared")
         .flag("reference",NULL,"reference",true)
         .flag("dissociate",NULL,"dissociate")
         .flag("quiet","q","quiet")
         .flag("depth",NULL,"depth");

    init.flag("quiet","q","quiet")
        .flag("bare",NULL,"bare")
        .flag("tmp",NULL,"template",true)
        .flag("sgd",NULL,"separate-git-dir",true)
        .flag("shared",NULL,"shared");

    git.flag("help",NULL,"help")
        .subcommand("clone",clone,true)
        .subcommand("init",init,true);

    // parse command line options
    git.parse(argc,argv);

    if(git.has_subcommand("init"))
    {//git init
        if(init.has("quite"))
        {// -q or --quiet option

        }
        if(init.has("bare"))
        {// --bare

        }
        if(init.has("tmp"))
        {
            // get tmp argument char * 
            init.get("tmp")
        }
        if(init.has("sgd"))
        {
            // get sgd argument char * 
            init.get("sgd")
        }
        if(init.has("shared"))
        {// shared

        }

    }
    else if(git.has_subcommand("clone"))
    {// git clone
        if(clone.has(...))
        {

        }
        ....

    }
    else
    {
        if(git.has("help"))
        {
            // show help message
        }
    }
    return 0;
}
```