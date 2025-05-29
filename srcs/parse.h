#pragma once

#include <stdbool.h>
#include <string.h>
#include "openssl.h"

bool isFlagSet(char *flags, char c);
bool parseCommand(char *command);
