;;
;; $Id: stlink.ss,v 1.7 1997/08/08 20:38:41 krentel Exp $
;;
;; Link the gui tester together into compound unit.
;;

(compound-unit/sig

  (import 
    [wx : mred:wx^]
    [testable : mred:testable-window^])

  (link
    [struct : mred:test:struct^
      ((unit/sig mred:test:struct^
	(import)
	(define-struct event (thunk))
        (define-struct (sleep struct:event) (msec))))]
	    
    [global : mred:test:globals^
      ((reference-unit/sig "stglobal.ss") wx testable struct)]

    [run : mred:test:run^
      ((reference-unit/sig "strun.ss") struct)]
    
    [prim : mred:test:primitives^
      ((reference-unit/sig "stprims.ss") wx testable struct global run)]

    [drscheme : mred:test:drscheme^
      ((reference-unit/sig "stdrs.ss") wx struct global)])

  (export
    (open struct)
    (open run)
    (open prim)
    (unit drscheme drs))

  )
