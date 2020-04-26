/**
 * @file josh.h
 * @author Jonathan Uklisty
 * @brief JOSH - Juos human Oriented Shell - a shell designed for humans 
 * @version 0.1
 * @date 2020-04-26
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef KERNEL_SHELL_H
#define KERNEL_SHELL_H

#include <kernel/kdef.h>

#include <kernel/data_structures/vector.hpp>
#include <kernel/data_structures/string.hpp>

using kernel::data_structures::String;
using kernel::data_structures::Vector;

NAMESPACE_BEGIN(kernel)

    NAMESPACE_BEGIN(shell)

        struct CmdResult
        {
            enum class Status
            {
                FAILURE,
                SUCCESS,
                ARGS_FAIL, // not enough args
            };

            Status  status;
        };

        using ShellArgsType     = Vector<String>&;

        using ShellCommandType  = CmdResult (*)(ShellArgsType);

        MACRO_T(auto, CMD_SUCCESS, CmdResult{CmdResult::Status::SUCCESS});
        MACRO_T(auto, CMD_FAILURE, CmdResult{CmdResult::Status::FAILURE});
        MACRO_T(auto, CMD_ARGS_FAIL, CmdResult{CmdResult::Status::ARGS_FAIL});


        #define DECLARE_SHELL_COMMAND(name, n_args) \
            constexpr int __cmd_nargs_##name = n_args; \
            kernel::shell::CmdResult name(kernel::shell::ShellArgsType args)

        #define SHELL_COMMAND_ASSERT_ARGS(name) \
            if (args.size() < __cmd_nargs_##name) \
                { return CMD_ARGS_FAIL; }
        
        #define IMPL_SHELL_COMMAND(name, code) \
            kernel::shell::CmdResult name(kernel::shell::ShellArgsType args) { \
                using kernel::shell::CMD_ARGS_FAIL;\
                using kernel::shell::CMD_FAILURE;\
                using kernel::shell::CMD_SUCCESS;\
                SHELL_COMMAND_ASSERT_ARGS(name); \
                code \
            }            

        void ShellMain();

    NAMESPACE_END(shell)

NAMESPACE_END(kernel)

#endif // KERNEL_SHELL_H