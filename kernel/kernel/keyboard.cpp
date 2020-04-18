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

    auto current_proc_input_buffer = Processing::GetInputBuffer();

    if (current_proc_input_buffer == nullptr)
    {
        return;
    }

    current_proc_input_buffer->enqueue(key);

    printf("Added key %c to buffer;\n", key.character);
}