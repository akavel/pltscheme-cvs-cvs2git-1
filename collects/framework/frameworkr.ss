;;
;; $Id: frameworkr.ss,v 1.14 1998/11/19 20:53:19 robby Exp $
;;

(compound-unit/sig
  (import [core : mzlib:core^]
	  [mred : mred-interfaces^])
  (link [F : framework^ ((require-relative-library "frameworkc.ss")
			 (core string)
			 (core function)
			 (core pretty-print)
			 (core file)
			 (core thread)
			 mred)])
  (export (open F)))
