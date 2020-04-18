#include <kernel/kio.h>
#include <kernel/processing.h>

char kernel::IO::GetChar()
{
    Processing::LockInputBuffer();

    auto input_buffer = Processing::GetInputBuffer();
    
    while (input_buffer->isempty())
    {
        asm volatile ("hlt;");
    }

    auto key = input_buffer->dequeue();

    Processing::UnlockInputBuffer();

    return key.character;
}
