;;
;; $Id: tests.ss,v 1.1 1998/11/19 17:23:42 robby Exp $
;;
;; Signatures for gui tester.
;;

(define-signature framework:test:run-export^
  (run-interval
   number-pending-actions
   reraise-error
   current-eventspaces))

(define-signature framework:test:run-internal^
  (get-active-frame
   get-focused-window
   run-one))

(define-signature framework:test:run^
  ((open framework:test:run-export^)
   (open framework:test:run-internal^)))

(define-signature framework:test:primitives^
  (button-push
   set-check-box!
   set-choice!
   keystroke  
   menu-select
   mouse-click
   new-window))

(define-signature framework:test^
  ((open framework:test:run-export^)
   (open framework:test:primitives^)))
