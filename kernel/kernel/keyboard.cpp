#include <kernel/keyboard.h>
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
    printf("Key pressed!\n");
    _key_source(nullptr);
}