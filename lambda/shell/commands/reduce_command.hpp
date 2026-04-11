#ifndef LAMBDA_SHELL_COMMANDS_REDUCE_COMMAND_HPP
#define LAMBDA_SHELL_COMMANDS_REDUCE_COMMAND_HPP

#include <lambda/shell/common.hpp>

namespace ld::shell {

class ReduceCommand : public shell_t::command {
    code run(std::string_view command, ShellEnv& env) override {
        return code::success;
    }
};

} // namespace ld

#endif//LAMBDA_SHELL_COMMANDS_REDUCE_COMMAND_HPP