;;
;; $Id: link.ss,v 1.44 1997/12/01 18:31:19 robby Exp $
;;

(compound-unit/sig (import [core : mzlib:core^])
  (link [wx : mred:wx^ ((unit/sig () (import)))]
	[mred : mred^ ((reference-unit/sig "linkwx.ss") core wx)])
  (export (open mred)))
