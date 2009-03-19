/* mpc_strtoc -- Read a complex number from a string.

Copyright (C) 2009 Philippe Th\'eveny, Andreas Enge

This file is part of the MPC Library.

The MPC Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The MPC Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the MPC Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "mpc-impl.h"

static void
skip_whitespace (char **p) {
   /* TODO: This function had better be inlined, but it is unclear whether
      the hassle to get this implemented across all platforms is worth it. */
   while (isspace ((unsigned char) **p))
      (*p)++;
}

int
mpc_strtoc (mpc_ptr rop, char *nptr, char **endptr, int base, mpc_rnd_t rnd) {
   char *p, *end;
   int bracketed = 0;

   int inex_re = 0, inex_im = 0;

   if (nptr == NULL || base > 36 || base == 1)
     goto error;

   p = nptr;
   skip_whitespace (&p);

   if (*p == '('){
      bracketed = 1;
      ++p;
   }

   inex_re = mpfr_strtofr (MPC_RE(rop), p, &end, base, MPC_RND_RE (rnd));
   if (end == p)
      goto error;
   p = end;

   if (bracketed) {
     if (!isspace ((unsigned char)*p))
         goto error;

      skip_whitespace (&p);

      inex_im = mpfr_strtofr (MPC_IM(rop), p, &end, base, MPC_RND_IM (rnd));
      if (end == p)
         goto error;
      p = end;

      if (*p != ')')
         goto error;

      p++;
   }
   else
     inex_im = mpfr_set_ui (MPC_IM (rop), 0, MPC_RND_IM(rnd));

   if (endptr != NULL)
      *endptr = p;
   return MPC_INEX (inex_re, inex_im);

error:
   if (endptr != NULL)
      *endptr = nptr;
   mpfr_set_nan (MPC_RE (rop));
   mpfr_set_nan (MPC_IM (rop));
   return -1;
}
