//#include <stdio.h>
#include <kernel/tty.h>
#include <kernel/kdef.h>
#include <kernel/dtables.h>
#include <kernel/interrupts.h>
#include <kernel/timer.h>
#include <kernel/kconstants.h>
#include <kernel/paging.h>
#include <kernel/kheap.h>
#include <kernel/klog.h>
#include <kernel/kmm.h>
#include <kernel/kallocators.hpp>
#include <kernel/kuseful.h>
#include <kernel/processing.h>
#include <kernel/kio.h>
#include <kernel/keyboard.h>
#include <kernel/schedualer.h>
#include <kernel/josh.h>

#include <drivers/ata.h>
#include <drivers/ext2.h>
#include <drivers/ps2.h>

#include <kernel/kstdcxx.hpp>
#include <list.hpp>

#include <stdio.h>
#include <string.h>

using namespace kernel;

void divide_by_zero(void*);
__NO_MANGELING void kernel_main(uint32_t);

void BREAKPOINT() 
{
	
}

void loop2()
{
	uint32_t counter = 1;

	while (true)
	{
		auto pid = Processing::GetPid();
		SYNCED_PRINTF_ARGS("Hello from process %d :) loop #%d\n", pid, counter++);
		Timer::Sleep(10);
	}
}

void kernel_stage_2(void)
{
	Processing::SelfProcessInit();

	static bool loop_finished = false;

	if (loop_finished) {
		while (true) {asm volatile("hlt");}
	}

	DISABLE_HARDWARE_INTERRUPTS();
	LOG_S("KERNEL: ", "Initiating stage 2...\n");
	
	Processing::RegisterProcess("shell", (void*)shell::ShellMain);
	Processing::RegisterProcess("loop2", (void*)loop2);
	Processing::Start::Process("shell", Processing::KernelProcess::Priority::High);
	
	auto self_pid = Processing::GetPid();
	Processing::End::Process(self_pid);

	ENABLE_HARDWARE_INTERRUPTS();

	loop_finished = true;
}

uint32_t __stack_top = 0;

__NO_MANGELING void kernel_main(uint32_t stack_top) {
	// the beginning of the stack is now here
	__stack_top = stack_top;

	/**
	 * The initialization order is important 
	 * 1. tty
	 * 2. (gdt/ idt)
	 * 3. memory managment
	 * 5. processing
	 * 4. interrupts
	 * 6. timer
	 * everything else... 
	 *
	 */
	Tty::Initialize();
	Gdt::Initialize();
	Idt::Initialize();
	// after this initialization, operators new and delete can be used
	MemoryManager::Initialize(
		K_HEAP_START, // beginning address
		K_HEAP_START + K_HEAP_INITIAL_SIZE, // end address
		0xCFFFF000, // max address
		false, // running in supervisor mode
		true // is read-write
	);


	auto proc_scheduler = new scheduler::ProcessScheduler();
	Processing::Initialize(kernel_stage_2, scheduler::run_process_scheduler, proc_scheduler);
	
	Interrupts::Initialize();
	//Interrupts::set_handler(1, [](auto) {});
	keyboard::InitializeKeyboard(drivers::ps2::keyboard::KeyboardSource);	
	Timer::start(K_INTERNAL_CLOCK_TICK_RATE);

	for (;;)
	{
		asm volatile (
			"hlt;"
		);
	}
}

void divide_by_zero(void* r)
{
	LOG_A("Divide by zero! %d", ((registers32_t*)r)->eax);
}

// enable pure virtual functions
__NO_MANGELING void __cxa_pure_virtual() {}