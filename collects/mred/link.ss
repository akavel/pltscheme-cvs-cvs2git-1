;;
;; $Id: link.ss,v 1.48 1998/04/20 20:49:21 robby Exp $
;;

(compound-unit/sig (import [core : mzlib:core^])
  (link [wx : wx^ (wx@)]
	[mred : mred^ ((require-unit/sig "linkwx.ss") core wx)])
  (export (open mred)))
