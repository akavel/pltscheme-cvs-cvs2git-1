; $Id: load.ss,v 1.21 1998/11/06 02:43:11 mflatt Exp $

(require-library "macro.ss")
(require-library "cores.ss")

(require-library "zsigs.ss" "zodiac")
(require-library "sigs.ss" "zodiac")

; All this stuff needs to be disappeared.

(define zodiac:system@
  (require-library-unit/sig "link.ss" "zodiac"))
