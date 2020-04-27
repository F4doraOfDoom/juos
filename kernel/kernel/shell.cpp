#include <kernel/josh.h>
#include <kernel/processing.h>
#include <kernel/kio.h>
#include <kernel/vfs.h>
#include <kernel/schedualer.h>
#include <kernel/tty.h>

#include <drivers/jfs.h>
#include <drivers/ata.h>
#include <string_utils.hpp>

using namespace kernel;

uint32_t processes_running = 0;
Processing::KernelProcess::ID last_pid;
kernel::StorageDeviceHandler* fs_storage = nullptr;
drivers::jfs::JuosFileSystem* fs = nullptr; 

 DECLARE_SHELL_COMMAND(StartProcess, 1);

 IMPL_SHELL_COMMAND(StartProcess,
    DISABLE_HARDWARE_INTERRUPTS();
    last_pid = Processing::Start::Process("loop2", Processing::KernelProcess::Priority::High);
    ENABLE_HARDWARE_INTERRUPTS();

    printf("Launched procesess pid #%d\n", last_pid);
    processes_running++;

    return CMD_SUCCESS;
)


 DECLARE_SHELL_COMMAND(EndProcess, 1);
 IMPL_SHELL_COMMAND(EndProcess, 
    DISABLE_HARDWARE_INTERRUPTS();
	Processing::End::Process(last_pid);
	ENABLE_HARDWARE_INTERRUPTS();

	printf("Stopped proccess %d\n", last_pid);
	processes_running--;

    return CMD_SUCCESS;
)

 DECLARE_SHELL_COMMAND(MakeFs, 1);
 IMPL_SHELL_COMMAND(MakeFs, 
    if (fs) 
    {
        printf("Deleting old fs...\n");
        delete fs;
        delete fs_storage;
    }

    fs_storage = ata::create_device();
    fs = new drivers::jfs::JuosFileSystem(fs_storage);
    fs->MakeNewFs();

    printf("Made new fs\n");
    
    return CMD_SUCCESS;
);

 DECLARE_SHELL_COMMAND(MakeFile, 2);
 IMPL_SHELL_COMMAND(MakeFile, 
    if (fs)
    {
        fs->CreateFile(args[1]);
        return CMD_SUCCESS;
    }
    else
    {
        printf("No file system found.\n");
        return CMD_FAILURE;
    }
)

DECLARE_SHELL_COMMAND(Echo, 1);
IMPL_SHELL_COMMAND(Echo, 
    for (uint32_t i = 1; i < args.size(); i++)
    {
        printf("%s", args[i].getText());
    }

    printf("\n");
    return CMD_SUCCESS;
)

struct CommandMap
{
    const char*                 command;
    shell::ShellCommandType     func;
} command_map[] = {
    {"START", StartProcess},
    {"END", EndProcess},
    {"MAKEFS", MakeFs},
    {"TOUCH", MakeFile},
    {"ECHO", Echo}
};

auto shell_banner0 = " \
                $&\x9\x0                WELCOME TO                 $&\xF\x0\n\
                $&\x0\x0    /$$$$$ /$$   /$$  /$$$$$$   /$$$$$$    $&\xF\x0\n\
                $&\x1\x0   |__  $$| $$  | $$ /$$__  $$ /$$__  $$   $&\xF\x0\n\
                $&\x2\x0      | $$| $$  | $$| $$  \\ $$| $$  \\__/ $&\xF\x0\n\
                $&\x3\x0      | $$| $$  | $$| $$  | $$|  $$$$$$    $&\xF\x0\n\
                $&\x4\x0 /$$  | $$| $$  | $$| $$  | $$ \\____  $$  $&\xF\x0\n\
                $&\x5\x0| $$  | $$| $$  | $$| $$  | $$ /$$  \\ $$  $&\xF\x0\n\
                $&\x6\x0|  $$$$$$/|  $$$$$$/|  $$$$$$/|  $$$$$$/   $&\xF\x0\n\
                $&\x7\x0 \\______/  \\______/  \\______/  \\______/$&\xF\x0\n\
$&\x8\x0";

auto shell_banner1 = " \
                $&\x8\x0                WELCOME TO                 $&\xF\x0\n\
                $&\x9\x0    /$$$$$ /$$   /$$  /$$$$$$   /$$$$$$    $&\xF\x0\n\
                $&\x0\x0   |__  $$| $$  | $$ /$$__  $$ /$$__  $$   $&\xF\x0\n\
                $&\x1\x0      | $$| $$  | $$| $$  \\ $$| $$  \\__/ $&\xF\x0\n\
                $&\x2\x0      | $$| $$  | $$| $$  | $$|  $$$$$$    $&\xF\x0\n\
                $&\x3\x0 /$$  | $$| $$  | $$| $$  | $$ \\____  $$  $&\xF\x0\n\
                $&\x4\x0| $$  | $$| $$  | $$| $$  | $$ /$$  \\ $$  $&\xF\x0\n\
                $&\x5\x0|  $$$$$$/|  $$$$$$/|  $$$$$$/|  $$$$$$/   $&\xF\x0\n\
                $&\x6\x0 \\______/  \\______/  \\______/  \\______/$&\xF\x0\n\
$&\x7\x0";

auto shell_banner2 = " \
                $&\x7\x0                WELCOME TO                 $&\xF\x0\n\
                $&\x8\x0    /$$$$$ /$$   /$$  /$$$$$$   /$$$$$$    $&\xF\x0\n\
                $&\x9\x0   |__  $$| $$  | $$ /$$__  $$ /$$__  $$   $&\xF\x0\n\
                $&\x0\x0      | $$| $$  | $$| $$  \\ $$| $$  \\__/ $&\xF\x0\n\
                $&\x1\x0      | $$| $$  | $$| $$  | $$|  $$$$$$    $&\xF\x0\n\
                $&\x2\x0 /$$  | $$| $$  | $$| $$  | $$ \\____  $$  $&\xF\x0\n\
                $&\x3\x0| $$  | $$| $$  | $$| $$  | $$ /$$  \\ $$  $&\xF\x0\n\
                $&\x4\x0|  $$$$$$/|  $$$$$$/|  $$$$$$/|  $$$$$$/   $&\xF\x0\n\
                $&\x5\x0 \\______/  \\______/  \\______/  \\______/$&\xF\x0\n\
$&\x6\x0";

auto shell_banner3 = " \
                $&\x6\x0                WELCOME TO                 $&\xF\x0\n\
                $&\x7\x0    /$$$$$ /$$   /$$  /$$$$$$   /$$$$$$    $&\xF\x0\n\
                $&\x8\x0   |__  $$| $$  | $$ /$$__  $$ /$$__  $$   $&\xF\x0\n\
                $&\x9\x0      | $$| $$  | $$| $$  \\ $$| $$  \\__/ $&\xF\x0\n\
                $&\x0\x0      | $$| $$  | $$| $$  | $$|  $$$$$$    $&\xF\x0\n\
                $&\x1\x0 /$$  | $$| $$  | $$| $$  | $$ \\____  $$  $&\xF\x0\n\
                $&\x2\x0| $$  | $$| $$  | $$| $$  | $$ /$$  \\ $$  $&\xF\x0\n\
                $&\x3\x0|  $$$$$$/|  $$$$$$/|  $$$$$$/|  $$$$$$/   $&\xF\x0\n\
                $&\x4\x0 \\______/  \\______/  \\______/  \\______/$&\xF\x0\n\
$&\x5\x0";

auto shell_banner4 = " \
                $&\x5\x0                WELCOME TO                 $&\xF\x0\n\
                $&\x6\x0    /$$$$$ /$$   /$$  /$$$$$$   /$$$$$$    $&\xF\x0\n\
                $&\x7\x0   |__  $$| $$  | $$ /$$__  $$ /$$__  $$   $&\xF\x0\n\
                $&\x8\x0      | $$| $$  | $$| $$  \\ $$| $$  \\__/ $&\xF\x0\n\
                $&\x9\x0      | $$| $$  | $$| $$  | $$|  $$$$$$    $&\xF\x0\n\
                $&\x0\x0 /$$  | $$| $$  | $$| $$  | $$ \\____  $$  $&\xF\x0\n\
                $&\x1\x0| $$  | $$| $$  | $$| $$  | $$ /$$  \\ $$  $&\xF\x0\n\
                $&\x2\x0|  $$$$$$/|  $$$$$$/|  $$$$$$/|  $$$$$$/   $&\xF\x0\n\
                $&\x3\x0 \\______/  \\______/  \\______/  \\______/$&\xF\x0\n\
$&\x4\x0";

auto shell_banner5 = " \
                $&\x4\x0                WELCOME TO                 $&\xF\x0\n\
                $&\x5\x0    /$$$$$ /$$   /$$  /$$$$$$   /$$$$$$    $&\xF\x0\n\
                $&\x6\x0   |__  $$| $$  | $$ /$$__  $$ /$$__  $$   $&\xF\x0\n\
                $&\x7\x0      | $$| $$  | $$| $$  \\ $$| $$  \\__/ $&\xF\x0\n\
                $&\x8\x0      | $$| $$  | $$| $$  | $$|  $$$$$$    $&\xF\x0\n\
                $&\x9\x0 /$$  | $$| $$  | $$| $$  | $$ \\____  $$  $&\xF\x0\n\
                $&\x0\x0| $$  | $$| $$  | $$| $$  | $$ /$$  \\ $$  $&\xF\x0\n\
                $&\x1\x0|  $$$$$$/|  $$$$$$/|  $$$$$$/|  $$$$$$/   $&\xF\x0\n\
                $&\x2\x0 \\______/  \\______/  \\______/  \\______/$&\xF\x0\n\
$&\x3\x0";

auto shell_banner6 = " \
                $&\x3\x0                WELCOME TO                 $&\xF\x0\n\
                $&\x4\x0    /$$$$$ /$$   /$$  /$$$$$$   /$$$$$$    $&\xF\x0\n\
                $&\x5\x0   |__  $$| $$  | $$ /$$__  $$ /$$__  $$   $&\xF\x0\n\
                $&\x6\x0      | $$| $$  | $$| $$  \\ $$| $$  \\__/ $&\xF\x0\n\
                $&\x7\x0      | $$| $$  | $$| $$  | $$|  $$$$$$    $&\xF\x0\n\
                $&\x8\x0 /$$  | $$| $$  | $$| $$  | $$ \\____  $$  $&\xF\x0\n\
                $&\x9\x0| $$  | $$| $$  | $$| $$  | $$ /$$  \\ $$  $&\xF\x0\n\
                $&\x0\x0|  $$$$$$/|  $$$$$$/|  $$$$$$/|  $$$$$$/   $&\xF\x0\n\
                $&\x1\x0 \\______/  \\______/  \\______/  \\______/$&\xF\x0\n\
$&\x2\x0";

auto shell_banner7 = " \
                $&\x2\x0                WELCOME TO                 $&\xF\x0\n\
                $&\x3\x0    /$$$$$ /$$   /$$  /$$$$$$   /$$$$$$    $&\xF\x0\n\
                $&\x4\x0   |__  $$| $$  | $$ /$$__  $$ /$$__  $$   $&\xF\x0\n\
                $&\x5\x0      | $$| $$  | $$| $$  \\ $$| $$  \\__/ $&\xF\x0\n\
                $&\x6\x0      | $$| $$  | $$| $$  | $$|  $$$$$$    $&\xF\x0\n\
                $&\x7\x0 /$$  | $$| $$  | $$| $$  | $$ \\____  $$  $&\xF\x0\n\
                $&\x8\x0| $$  | $$| $$  | $$| $$  | $$ /$$  \\ $$  $&\xF\x0\n\
                $&\x9\x0|  $$$$$$/|  $$$$$$/|  $$$$$$/|  $$$$$$/   $&\xF\x0\n\
                $&\x0\x0 \\______/  \\______/  \\______/  \\______/$&\xF\x0\n\
$&\x1\x0";

auto shell_banner8 = " \
                $&\x1\x0                WELCOME TO                 $&\xF\x0\n\
                $&\x2\x0    /$$$$$ /$$   /$$  /$$$$$$   /$$$$$$    $&\xF\x0\n\
                $&\x3\x0   |__  $$| $$  | $$ /$$__  $$ /$$__  $$   $&\xF\x0\n\
                $&\x4\x0      | $$| $$  | $$| $$  \\ $$| $$  \\__/ $&\xF\x0\n\
                $&\x5\x0      | $$| $$  | $$| $$  | $$|  $$$$$$    $&\xF\x0\n\
                $&\x6\x0 /$$  | $$| $$  | $$| $$  | $$ \\____  $$  $&\xF\x0\n\
                $&\x7\x0| $$  | $$| $$  | $$| $$  | $$ /$$  \\ $$  $&\xF\x0\n\
                $&\x8\x0|  $$$$$$/|  $$$$$$/|  $$$$$$/|  $$$$$$/   $&\xF\x0\n\
                $&\x9\x0 \\______/  \\______/  \\______/  \\______/$&\xF\x0\n\
$&\x0\x0";

auto shell_banner9 = " \
                $&\x0\x0                WELCOME TO                 $&\xF\x0\n\
                $&\x1\x0    /$$$$$ /$$   /$$  /$$$$$$   /$$$$$$    $&\xF\x0\n\
                $&\x2\x0   |__  $$| $$  | $$ /$$__  $$ /$$__  $$   $&\xF\x0\n\
                $&\x3\x0      | $$| $$  | $$| $$  \\ $$| $$  \\__/ $&\xF\x0\n\
                $&\x4\x0      | $$| $$  | $$| $$  | $$|  $$$$$$    $&\xF\x0\n\
                $&\x5\x0 /$$  | $$| $$  | $$| $$  | $$ \\____  $$  $&\xF\x0\n\
                $&\x6\x0| $$  | $$| $$  | $$| $$  | $$ /$$  \\ $$  $&\xF\x0\n\
                $&\x7\x0|  $$$$$$/|  $$$$$$/|  $$$$$$/|  $$$$$$/   $&\xF\x0\n\
                $&\x8\x0 \\______/  \\______/  \\______/  \\______/$&\xF\x0\n\
$&\x9\x0";

void shell::ShellMain()
{
    Processing::SelfProcessInit();

    const char* animation_frames[] = {shell_banner0, shell_banner1, shell_banner2, shell_banner3, shell_banner4, shell_banner5, shell_banner6, shell_banner7, shell_banner8, shell_banner9};
	//uint32_t counter = 0;
    for (uint32_t i = 0; i < 3; i++)
    {
        for (uint32_t j = 0; j < 10; j++)
        {
            Tty::Clean();
            printf(animation_frames[j]);
            Timer::Sleep(2);
        }
    }

    // return color to white
    printf("$&\xF\x0");

	char buffer[100];

	while (true)
	{
		printf("(%d processes) >>> ", processes_running);
		memset(buffer, 0, 100);
		kernel::IO::GetString(buffer, 99);

        auto args = std::string::split(buffer, ' ');
        bool found_command = false;
		for (uint32_t i = 0; i < 5; i++)
        {
            auto command_name = command_map[i].command;
            if (args[0].compare(command_name))
            {
                auto ret = command_map[i].func(args);
                
                switch (ret.status)
                {
                    case shell::CmdResult::Status::SUCCESS:
                        // Say nothing for success
                    break;

                    case shell::CmdResult::Status::FAILURE:
                        printf("$&\x0\x4%s - Unknown fail.$&\xF\x0\n", command_name);
                    break;

                    case shell::CmdResult::Status::ARGS_FAIL:
                        printf("$&\x0\x4%s - Not enough args supplied.$&\xF\x0\n", command_name);
                    break;

                    default:
                        printf("%s - Returned unknown code (%d)\n", command_name, ret.status);
                    break;
                }

                found_command = true;
                break;
            }
        }

        if (!found_command)
        {
            printf("$&\x0\x4 Command %s not found.$&\xF\x0\n", args[0].c_str());
        }

        kernel::IO::Flush();
	}
}