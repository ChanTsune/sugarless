# Sugarless
Sugarless is a simple command line parser for C++.  

### How to use

```cpp
include "sugarless.hpp"
```

### Create Command object

```cpp
sugarless::Command cmd;
```

### Flag setting  

```cpp
// short name and long name
cmd.flag("opt1","o","option1");
// only long name
cmd.flag("opt2",NULL,"option2");
// only short name
cmd.flag("opt3","3",NULL);

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
sugarless::Command sub_cmd1;
sugarless::Command sub_cmd2;

cmd.sub_command("sub_command_name",sub_cmd1);
//Inherit parent options
cmd.sub_command("sub_command_name",sub_cmd2,true);
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
After set all of flags and sub commands,Pass `argc`,`argv` of `main` function as it is.  

### Chack options existing in recived command line options


### argument syntax  
Sugarless basically follows the specification of GUN  
Case of the string start with `-` is mean short option.  
Case of the string start with `--` is mean long option.  
If you want to pass the string start with `-` or `--` as an option's argument,
Insert `--` immediately before.

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

#### case of `-`start argumenst  

The next argument of `--` is not recognized as an option.  

If you want to pass the argument `-1` to the` -a` option.  

Bad case  
```
$ app -a -1
```

Correct case  
```
$ app -a -- -1
```
