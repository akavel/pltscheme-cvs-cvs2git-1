/*
  MzScheme
  Copyright (c) 2000 Matthew Flatt

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

  libscheme
  Copyright (c) 1994 Brent Benson
  All rights reserved.
*/

#include "schpriv.h"

/* On the Mac, 68K, store the built-in Scheme code as pc-relative */
#if defined(__MWERKS__)
#if !defined(__POWERPC__)
#pragma pcrelstrings on
#endif
#endif

void scheme_add_embedded_builtins(Scheme_Env *env)
{
#if 0

#define EVAL_ONE_STR(str) scheme_eval_string(str, env)
#define EVAL_ONE_SIZED_STR(str, len) scheme_eval_compiled_sized_string(str, len, env)
#define JUST_DEFINED(name)
#define JUST_DEFINED_FUNC(name) JUST_DEFINED(name)
#define JUST_DEFINED_KEY(name) JUST_DEFINED(name)
#define JUST_DEFINED_COND(name) JUST_DEFINED(name)
#define JUST_DEFINED_QQ(name) JUST_DEFINED(name)

#if USE_COMPILED_MACROS
# include "cmacro.inc"
#else
# include "macro.inc"
#endif

#else

  scheme_eval_string("
(define-values (read-eval-print-loop)
 (lambda ()
  (let* ([eeh #f]
         [jump #f]
         [be? #f]
         [rep-error-escape-handler (lambda () (jump))])
    (dynamic-wind
      (lambda () (set! eeh (error-escape-handler))
                 (set! be? (break-enabled))
                 (error-escape-handler rep-error-escape-handler)
                 (break-enabled #f))
      (lambda ()
        (call/ec (lambda (done)
          (let loop ()
            (call/ec (lambda (k)
              (dynamic-wind
                 (lambda ()
                   (break-enabled be?)
                   (set! jump k))
                 (lambda ()
                   (let ([v ((current-prompt-read))])
                     (if (eof-object? v) (done (void)))
                     (call-with-values
                      (lambda () ((current-eval) v))
                      (lambda results (for-each (current-print) results)))))
                 (lambda () 
                   (set! be? (break-enabled))
                   (break-enabled #f)
                   (set! jump #f)))))
            (loop)))))
      (lambda () (error-escape-handler eeh)
                   (break-enabled be?)
                   (set! jump #f)
                   (set! eeh #f))))))
", env);

#endif
}

#if defined(__MWERKS__)
#if !defined(__POWERPC__)
#pragma pcrelstrings reset
#endif
#endif
