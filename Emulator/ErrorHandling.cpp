#include "ErrorHandling.hpp"

void throwError(std::string name, int error_code)
{
    static int error_count = 0;

    if(error_code == NO_ERROR)
    {
        return;
    }
    error_count++;
    std::cout << "Error: " << error_count << " :\n|->\t" << name << " : " << error_code << "\n";

    if(error_code == INTERNAL_SOFTWARE_ERROR || 19 < error_count)
    {
        std::cout << "\n\tMaximum errors have been hit [ " << error_count << " ], quitting program...\n\n";
        exit(0);
    }
}

int breaks_to_take = 0;

void triggerBreakPoint()
{
    breaks_to_take++;
}
void breakThePoint()
{
    if(breaks_to_take)
    {
        std::cout << "Broken the point...\n";
        breaks_to_take--;
    }
}