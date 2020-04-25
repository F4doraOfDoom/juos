#include <kernel/josh.h>
#include <kernel/processing.h>
#include <kernel/kio.h>
#include <kernel/vfs.h>
#include <kernel/schedualer.h>

#include <drivers/jfs.h>
#include <drivers/ata.h>
#include <string_utils.hpp>

using namespace kernel;

uint32_t processes_running = 0;
Processing::KernelProcess::ID last_pid;
kernel::StorageDeviceHandler* fs_storage = nullptr;
drivers::jfs::JuosFileSystem* fs = nullptr; 

static void StartProcess(Vector<String>&)
{
    DISABLE_HARDWARE_INTERRUPTS();
    last_pid = Processing::Start::Process("loop2", Processing::KernelProcess::Priority::High);
    ENABLE_HARDWARE_INTERRUPTS();

    printf("Launched procesess pid #%d\n", last_pid);
    processes_running++;
};

static void EndProcess(Vector<String>&)
{
    DISABLE_HARDWARE_INTERRUPTS();
	Processing::End::Process(last_pid);
	ENABLE_HARDWARE_INTERRUPTS();

	printf("Stopped proccess %d\n", last_pid);
	processes_running--;
};

static void MakeFs(Vector<String>& args)
{
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
}

// gets arguments
using Command = void (*)(Vector<String>&);

struct CommandMap
{
    const char*         command;
    Command             func;
} command_map[] = {
    {"START", StartProcess},
    {"END", EndProcess},
    {"MAKEFS", MakeFs}
};


void shell::ShellMain()
{
    Processing::SelfProcessInit();

	//uint32_t counter = 0;

	char buffer[100];

	while (true)
	{
		printf("(%d processes) >>> ", processes_running);
		memset(buffer, 0, 100);
		kernel::IO::GetString(buffer, 99);

		String str(buffer);
        auto args = std::string::split(str, ' ');
		for (uint32_t i = 0; i < 3; i++)
        {
            if (args[0].compare(command_map[i].command))
            {
                command_map[i].func(args);
                break;
            }
        }
	}
}