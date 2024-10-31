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

/**
 * Converts an ANSI escape code into an escape sequence
 * which can be used in string formatting. This allows for
 * terminal output to be stylized, making it more visually
 * appealing and easier to read.
 */
const char* code2str(ANSIEscapeCode code, FILE* fd);
#endif
