;;
;; $Id: frameworkr.ss,v 1.18 1999/07/16 05:11:06 robby Exp $
;;

(compound-unit/sig
  (import [core : mzlib:core^]
	  [mred : mred^])
  (link [keys : framework:keys^ ((require-relative-library "keys.ss"))]
	[test : framework:test^ ((require-relative-library "testr.ss") mred keys)]
	[f : frameworkc^ ((require-relative-library "frameworkc.ss")
			  (core string)
			  (core function)
			  (core pretty-print)
			  (core file)
			  (core thread)
			  mred
			  keys
			  test)])
  (export
   (unit keys)
   (unit test)
   (open f)))
