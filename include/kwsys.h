/**
 * Defines the most common utilities used by this library
 * for operations such as:
 * - Outputting logs to stdout and stderr.
 * - Managing heap resources.
 */
#ifndef KWC_SYSTEM_H
#define KWC_SYSTEM_H
#include <stdio.h>
#include <string.h>

// -------------------------------------------------------
// IO Operations
// -------------------------------------------------------
// Macros defines here determine the procedure of how to
// write to file descriptors such as stderr and stdout.
// These are meant to be used throughout this library for
// the purposes of relaying information out to the
// terminal. This allows the organization of information
// that is used primarily for debugging/triage.
// -------------------------------------------------------
#include "colors.h"
/**
 * Writes a prefix to the output file. If the output is a
 * TTY, then the prefix is stylized using ANSI escape
 * codes.
*/
#define FPrefixF(FILE, P, COLOR) fprintf(FILE, "%s%s%s", code2str(COLOR, FILE), P, code2str(ANSI_RESET, FILE))
/**
 * The root logging procedure. When a log entry is written
 * to the output file, the output is prefixed with entry
 * metadata.
*/
#define FOutputF(FILE, P, COLOR, ...) { \
    fprintf(FILE, "<"); \
    FPrefixF(FILE, P, COLOR); \
    fprintf(FILE, " "); \
    fprintf(FILE, "%s:%s:%d> ", __FILE__, __func__, __LINE__); \
    fprintf(FILE, __VA_ARGS__); \
}

/**
 * Writes an error message to the target file.
 */
#define FErrorF(FILE, ...) FOutputF(FILE, "ERROR ", ANSI_RED, __VA_ARGS__)
/**
 * Writes an error message to stderr.
 */
#define ErrorF(...) FErrorF(stderr, __VA_ARGS__)

#if DEBUG > 0
/**
 * Writes an info message to the target file.
 */
#define FInfoF(FILE, ...) FOutputF(FILE, "INFO  ", ANSI_BLUE, __VA_ARGS__)
#else
/**
 * Writes an info message to the target file.
 */
#define FInfoF(FILE, ...)
#endif
/**
 * Writes an info message to stdout.
 */
#define InfoF(...) FInfoF(stdout, __VA_ARGS__)

#if DEBUG > 1
/**
 * Writes a debug message to the target file.
 */
#define FDebugF(FILE, ...) FOutputF(FILE, "DEBUG ", ANSI_YELLOW, __VA_ARGS__)
#else
/**
 * Writes a debug message to the target file.
 */
#define FDebugF(FILE, ...)
#endif
/**
 * Writes a debug message to stderr.
 */
#define DebugF(...) FDebugF(stderr, __VA_ARGS__)

#if DEBUG > 2
/**
 * Writes a system level message to the target file.
 */
#define FSystemF(FILE, ...) FOutputF(FILE, "SYSTEM", ANSI_CYAN, __VA_ARGS__)
#else
/**
 * Writes a system level message to the target file.
 */
#define FSystemF(FILE, ...)
#endif
/**
 * Writes a system level message to stderr.
 */
#define SystemF(...) FSystemF(stderr, __VA_ARGS__)
#endif // Pragma
