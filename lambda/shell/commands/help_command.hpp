#ifndef LAMBDA_PROMPT_COMMANDS_HELP_COMMAND_HPP
#define LAMBDA_PROMPT_COMMANDS_HELP_COMMAND_HPP

#include <cpptools/cli/shell.hpp>

#include <lambda/shell/common.hpp>

namespace ld::shell {

static constexpr std::string_view HelpText =
R"(lambda, a lambda calculus engine
Input may be any of the following:
- Abstraction:     \x.x
- Application:     (x y)
- Declaration:     let 1: \f.\x.(f x)
- Identifier:      1

Expressions may be given names through declarations. A declaration binds an identifier to an expression. Identifiers
which can appear in expressions may or may not refer to an existing declaration.

The engine will parse the input expression and print it back in its most reduced form. # TODO

The following commands are recognized:
- ? or help:           Display this message
- exit:                Exit lambda
- export <path> ...:   Save declarations to a file
- import <path> ...:   Load declarations from a file # TODO
- inspect <id>:        Get info about a declared identifer # TODO: two -> 2 -> \f.\x.(f (f x))
- sub <M> <x> <N>:     In expression M, substitute every free occurrence of x for expression N
- bake <M>:            In expression M, substitute all free variables for the named expressions they each identify

Some commands will act conservatively when their running would lead to conflict or loss. Appending a bang ('!') to the
command name causes the command to force its action regardless of consequences. The following commands are concerned:
- let will not redeclare an identifier already in use, whereas let! will.
- import will not override existing declarations with the imported ones, whereas import! will.
- export will not overwrite an existing file on disk, whereas export! will.
)";

class HelpCommand : public shell_t::command {
    code run(std::string_view command, ShellEnv& env) override {
        env.shell_streams().out << HelpText;
        return code::success;
    }
};

} // namespace ld

#endif//LAMBDA_PROMPT_COMMANDS_HELP_COMMAND_HPP