;;
;; $Id: tests.ss,v 1.2 1998/11/19 20:53:19 robby Exp $
;;
;; Signatures for gui tester.
;;

(define-signature framework:test:run^
  (run-interval
   number-pending-actions
   reraise-error
   run-one))

(define-signature framework:test:primitives^
  (current-eventspaces

   button-push
   set-check-box!
   set-choice!
   keystroke  
   menu-select
   mouse-click
   new-window))

(define-signature framework:test^
  ((open framework:test:run^)
   (open framework:test:primitives^)))
