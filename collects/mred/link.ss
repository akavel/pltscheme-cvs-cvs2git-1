;;
;; $Id: link.ss,v 1.45 1997/12/04 21:28:32 mflatt Exp $
;;

(compound-unit/sig (import [core : mzlib:core^])
  (link [wx : mred:wx^ ((compound-unit/sig (import) 
					   (link [wx : wx^ (wx@)])
					   (export (unit wx))))]
	[mred : mred^ ((reference-unit/sig "linkwx.ss") core wx)])
  (export (open mred)))
