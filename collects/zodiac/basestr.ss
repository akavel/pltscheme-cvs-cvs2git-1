;;
;; zodiac:structures@
;; $Id: basestr.ss,v 1.1 1997/02/25 16:06:09 krentel Exp $
;;
;; Top-level zodiac structures (outside the hierarchy)
;; and base of zodiac hierarchy.
;;

(unit/sig  zodiac:structures^
  (import)

  (define-struct  origin    (who  how))
  (define-struct  location  (line  column  offset  file))
  (define-struct  period    (location))
  (define-struct  eof       (location))

  (define-struct  zodiac    (origin  start  finish))
  )

