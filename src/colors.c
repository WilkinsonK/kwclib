#include <stdio.h>

#include "colors.h"

#define ANSI_Escape(C) "\033[" C "m"

const char* code2str(ANSIEscapeCode code, FILE* fd) {
    // If the output file is not a TTY, we avoid returning
    // any ANSI escape codes.
    if (!isatty(fileno(fd))) return "";
    switch (code) {
        case ANSI_RESET:   return ANSI_Escape( "0");
        case ANSI_BLACK:   return ANSI_Escape("30");
        case ANSI_RED:     return ANSI_Escape("31");
        case ANSI_GREEN:   return ANSI_Escape("32");
        case ANSI_YELLOW:  return ANSI_Escape("33");
        case ANSI_BLUE:    return ANSI_Escape("34");
        case ANSI_MAGENTA: return ANSI_Escape("35");
        case ANSI_CYAN:    return ANSI_Escape("36");
        case ANSI_WHITE:   return ANSI_Escape("37");
    }
}
