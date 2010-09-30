#ifndef COLORS_H
#define COLORS_H

#define ESC "\x1b"
#ifdef USE_TERMINAL_COLORS

#define TERM_FG_BLACK   ESC "[30m"
#define TERM_FG_RED     ESC "[31m"
#define TERM_FG_GREEN   ESC "[32m"
#define TERM_FG_BROWN   ESC "[33m"
#define TERM_FG_BLUE    ESC "[34m"
#define TERM_FG_MAGENTA ESC "[35m"
#define TERM_FG_CYAN    ESC "[36m"
#define TERM_FG_WHITE   ESC "[37m"

#define TERM_BOLD       ESC "[1m"
#define TERM_NORMAL     ESC "[0m"

#else /* USE_TERMINAL_COLORS */

#define TERM_FG_BLACK   
#define TERM_FG_RED     
#define TERM_FG_GREEN   
#define TERM_FG_BROWN   
#define TERM_FG_BLUE    
#define TERM_FG_MAGENTA 
#define TERM_FG_CYAN    
#define TERM_FG_WHITE   

#define TERM_BOLD       
#define TERM_NORMAL     

#endif /* USE_TERMINAL_COLORS */


#endif /* COLORS_H */

