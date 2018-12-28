# sugarless
Sugarless is a simple command line parser for C++.

## How to use

```cpp
include "sugarless.hpp"
```

## Create Command object

```cpp
auto cmd = sugarless::Command(std::string("example"));
```
Pass the name of your application as an argument.
When no argument is passed Automatically the name of the executable file (argv [0]) is used.
## Flag setting

```cpp
auto opt1 = sugarless::Flag("o","opt1","opt1 is first option");

cmd.flag("opt1",);
```

Create a Command object and set options with the flag method.  
The first argument is the identifier used in the program.  
The second argument is a short option like "-o".  
The third argument is a long option like "--output".  

## Help message setting
```cpp
cmd.flag("opt2",{'2'},{"opt2"},"Help message");
```
The fourth argument of flag method is a description of the option to display in Help.  
The fifth argument specifies whether the option takes an argument or not.  
For the sixth argument, set an optional default argument.
Ignored if the fifth argument is set to false.  
If you set only the first argument, you can get the string directly.  
Since the flag method returns a self reference, the method chain can be used.  