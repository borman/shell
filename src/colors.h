/*
 * Copyright 2010, Mikhail "borman" Borisov <borisov.mikhail@gmail.com>
 *
 * This file is part of borman's model shell.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


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

