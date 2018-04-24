/* mpc_dot -- Dot product of two arrays of complex numbers.

Copyright (C) 2018 INRIA

This file is part of GNU MPC.

GNU MPC is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

GNU MPC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this program. If not, see http://www.gnu.org/licenses/ .
*/

#include <stdio.h> /* for MPC_ASSERT */
#include "mpc-impl.h"

/* res <- x[0]*y[0] + ... + x[n-1]*y[n-1] */
int
mpc_dot (mpc_ptr res, const mpc_ptr *x, const mpc_ptr *y,
         unsigned long n, mpc_rnd_t rnd)
{
  int inex_re, inex_im;
  mpfr_ptr *t;
  mpfr_t *z;
  unsigned long i;

  z = (mpfr_t *) malloc (2 * n * sizeof (mpfr_t));
  MPC_ASSERT(z != NULL);
  t = (mpfr_ptr *) malloc (2 * n * sizeof(mpfr_t));
  MPC_ASSERT(t != NULL);
  for (i = 0; i < 2 * n; i++)
    t[i] = z[i];
  /* we first store in z[i] the value of Re(x[i])*Re(y[i])
     and in z[n+i] that of -Im(x[i])*Im(y[i]) */
  for (i = 0; i < n; i++)
    {
      mpfr_init2 (z[i], mpfr_get_prec (mpc_realref (x[i]))
                  + mpfr_get_prec (mpc_realref (y[i])));
      mpfr_mul (z[i], mpc_realref (x[i]), mpc_realref (y[i]), MPFR_RNDZ);
      mpfr_init2 (z[n+i], mpfr_get_prec (mpc_imagref (x[i]))
                  + mpfr_get_prec (mpc_imagref (y[i])));
      mpfr_mul (z[n+i], mpc_imagref (x[i]), mpc_imagref (y[i]), MPFR_RNDZ);
      mpfr_neg (z[n+i], z[n+i], MPFR_RNDZ);
    }
  inex_re = mpfr_sum (mpc_realref (res), t, 2 * n, MPC_RND_RE (rnd));
  /* we then store in z[i] the value of Re(x[i])*Im(y[i])
     and in z[n+i] that of Im(x[i])*Re(y[i]) */
  for (i = 0; i < n; i++)
    {
      mpfr_set_prec (z[i], mpfr_get_prec (mpc_realref (x[i]))
                     + mpfr_get_prec (mpc_imagref (y[i])));
      mpfr_mul (z[i], mpc_realref (x[i]), mpc_imagref (y[i]), MPFR_RNDZ);
      mpfr_set_prec (z[n+i], mpfr_get_prec (mpc_imagref (x[i]))
                  + mpfr_get_prec (mpc_realref (y[i])));
      mpfr_mul (z[n+i], mpc_imagref (x[i]), mpc_realref (y[i]), MPFR_RNDZ);
    }
  inex_im = mpfr_sum (mpc_imagref (res), t, 2 * n, MPC_RND_IM (rnd));
  for (i = 0; i < 2 * n; i++)
    mpfr_clear (z[i]);
  free (t);
  free (z);

  return MPC_INEX(inex_re, inex_im);
}
