/*
    This file is an extension which is only used in the benlibc.
    Enter the following line of code at the beginning of your main()
    function:

    benix_GetCLIArgs();

    It will create two variables:
    - int argc: parameter count
    - char** argv: parameters
*/

#pragma once

#define benix_GetCLIArgs()              struct CommandLineArguments* __cliParameters = (struct CommandLineArguments*)CLIARGS_ADDRESS; \
                                        int argc = __cliParameters->argc; \
                                        char** argv = __cliParameters->argv;

#define CLIARGS_ADDRESS                 0x200000

struct CommandLineArguments {
    int argc;
    char** argv;
};