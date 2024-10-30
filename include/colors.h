/**
 * Defines ANSI escape codes which can then be used to
 * stylize terminal output if the output is a TTY.
 */
#ifndef KWC_COLORS_H
#define KWC_COLORS_H
#include <stdio.h>
#include <unistd.h>

/**
 * ANSI escape codes for stylizing terminal output.
 */
typedef enum ANSIEscapeCode {
    ANSI_RESET = 0,
    ANSI_BLACK = 30,
    ANSI_RED,
    ANSI_GREEN,
    ANSI_YELLOW,
    ANSI_BLUE,
    ANSI_MAGENTA,
    ANSI_CYAN,
    ANSI_WHITE
} ANSIEscapeCode;

#define ANSI_Escape(C) "\033[" C "m"

/**
 * Converts an ANSI escape code into an escape sequence
 * which can be used in string formatting. This allows for
 * terminal output to be stylized, making it more visually
 * appealing and easier to read.
 */
const char* code2str(ANSIEscapeCode code, FILE* fd) {
    int fd_id = *((int*)fd);
    // If the output file is not a TTY, we avoid returning
    // any ANSI escape codes.
    if (!isatty(fd_id)) return "";
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
#endif
