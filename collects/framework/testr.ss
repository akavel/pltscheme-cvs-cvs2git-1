;;
;; $Id: testr.ss,v 1.3 1998/11/19 21:24:36 robby Exp $
;;
;; Link the gui tester together into compound unit.
;;
;; keymap (keys.ss) must be evaled before stprims.ss,
;; so mred:shifted-key-list is defined.
;;   

(compound-unit/sig

  (import [mred : mred-interfaces^]
	  [keys : framework:keys^])

  (link
    [run : framework:test:run^
      ((require-unit/sig "test-run.ss") mred)]
    [prim : framework:test:primitives^
      ((require-unit/sig "test-prims.ss") mred keys run)])

  (export
    (open run)
    (open prim)))
