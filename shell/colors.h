#ifndef COLORS_H
#define COLORS_H

#define ESC "\x1b"
#ifdef USE_TERMINAL_COLORS

#define TERM_RED ESC "[0;31;40m"
#define TERM_GREEN ESC "[0;32;40m"
#define TERM_NORMAL ESC "[0;37;40m"

#else /* USE_TERMINAL_COLORS */

#define TERM_RED ""
#define TERM_GREEN ""
#define TERM_NORMAL ""

#endif /* USE_TERMINAL_COLORS */


#endif /* COLORS_H */

