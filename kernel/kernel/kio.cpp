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

void kernel::IO::Flush()
{
    Processing::LockInputBuffer();

    auto input_buffer = Processing::GetInputBuffer();

    while (!input_buffer->isempty())
    {
        input_buffer->dequeue();
    }

    Processing::UnlockInputBuffer();
}

void kernel::IO::GetString(char* buffer, uint32_t max_len)
{
    for (uint32_t i = 0; i < max_len;)
    {
        char c = GetChar();

        switch (c)
        {
            case '\n':
                putchar('\n');
                return;
            break;

            case '\b':
                if (i > 0)
                {
                    putchar('\b');
                    i--;
                }
            break;

            case kernel::IO::EOT:
                return;
            break;

            default:
                putchar(c);
                buffer[i] = c;
                i++;
            break;
        }
        
    }
}

