#include "parse.h"

bool isFlagSet(const char *flags, char c)
{
    if (!flags) return false;
    for (int i = 0; flags[i]; ++i)
        if (flags[i] == c) return true;
    return false;
}
