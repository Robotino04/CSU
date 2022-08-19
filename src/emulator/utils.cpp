#include "utils.hpp"

#include <iostream>
#include <iomanip>
#include <termcolor/termcolor.hpp>

void HexDump(const std::vector<uint8_t>& bytes, std::ostream& stream, size_t start)
{
    char buff[17];
    size_t i = 0;

    if (start > bytes.size())
        throw std::out_of_range("Invalid memory region");

    stream << std::hex;

    // Process every byte in the data.
    for (i = start; i < bytes.size(); i++)
    {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0)
        {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
            {
                stream << "  " << buff << std::endl;
            }

            // Output the offset.
            stream << termcolor::blue;
            stream << "  " << std::setw(4) << std::setfill('0') << static_cast<unsigned int>(i);
            stream << termcolor::reset;
        }

        // Now the hex code for the specific character.
        stream << termcolor::green;
        stream << " " << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(bytes[i]);
        stream << termcolor::reset;

        // And store a printable ASCII character for later.
        if ((bytes[i] < 0x20) || (bytes[i] > 0x7e))
        {
            buff[i % 16] = '.';
        }
        else
        {
            buff[i % 16] = bytes[i];
        }
        buff[(i % 16) + 1] = '\0';
    }

    stream << std::dec;

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0)
    {
        stream << "   ";
        i++;
    }

    // And print the final ASCII bit.
    stream << "  " << buff << std::endl;
}

uint16_t toAddress(uint8_t a, uint8_t b){
    return (((uint16_t)b) << 8) | ((uint16_t)a);
}