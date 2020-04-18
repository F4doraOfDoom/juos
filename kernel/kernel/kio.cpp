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

void kernel::IO::GetString(char* buffer, uint32_t max_len)
{
    for (uint32_t i = 0; i < max_len; i++)
    {
        char c = GetChar();

        printf("%c", c);

        if (c == '\n') return;

        buffer[i] = c;
    }
}

