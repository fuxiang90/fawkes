
/***************************************************************************
 *  macros.h - important macros
 *
 *  Created: Mon Dec 03 13:52:05 2007
 *  Copyright  2007  Tim Niemueller [www.niemueller.de]
 *
 *  $Id$
 *
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1307, USA.
 */

#ifndef __CORE_MACROS_H_
#define __CORE_MACROS_H_

// from http://blog.rlove.org/2005_10_01_archive.html
// Note that __GNUC__ is also set appropriately by the Intel compiler
#if __GNUC__ >= 3
# define __inline	inline __attribute__ ((__always_inline__))
# define __pure		__attribute__ ((__pure__))
# define __const_	__attribute__ ((__const__))
# define __noreturn	__attribute__ ((__noreturn__))
# define __malloc	__attribute__ ((__malloc__))
# define __must_check	__attribute__ ((__warn_unused_result__))
# define __deprecated	__attribute__ ((__deprecated__))
# define __used		__attribute__ ((__used__))
# define __unused	__attribute__ ((__unused__))
# define __packed	__attribute__ ((__packed__))
# define __aligned(x)   __attribute__ ((__aligned__ (x)))
# define likely(x)	__builtin_expect (!!(x), 1)
# define unlikely(x)	__builtin_expect (!!(x), 0)
#else
# define __inline	/* no inline */
# define __pure		/* no pure */
# define __const_	/* no const */
# define __noreturn	/* no noreturn */
# define __malloc	/* no malloc */
# define __must_check	/* no warn_unused_result */
# define __deprecated	/* no deprecated */
# define __used		/* no used */
# define __unused	/* no unused */
# define __packed	/* no packed */
# define __aligned(x)   /* no align */
# define likely(x)	(x)
# define unlikely(x)	(x)
#endif

#endif