#include <kernel/keyboard.h>
#include <kernel/processing.h>
#include <kernel/kcommon.h>

using namespace kernel;

static keyboard::KeyboardCallback _key_source;

void keyboard::InitializeKeyboard(KeyboardCallback source)
{
    // TODO driver specific initialization
    inb(0x60);
    _key_source = source;
    Interrupts::set_handler(33, keyboard::KeyboardHandler);
}


void keyboard::KeyboardHandler(void*)
{
    auto key = _key_source(nullptr);

    if (key.error)
    {
        return;
    }

    auto current_proc_input_buffer = Processing::GetInputBuffer();

    if (current_proc_input_buffer == nullptr)
    {
        return;
    }

    DISABLE_HARDWARE_INTERRUPTS();

    current_proc_input_buffer->enqueue(key);

    ENABLE_HARDWARE_INTERRUPTS();

    //printf("Added key %c to buffer;\n", key.character);
}