;; SirMail: Simple Imap Reader for Mail
;;          (with a mail composer, too)

(module sirmail mzscheme
  (require (lib "unitsig.ss")
	   (lib "class.ss")
	   (lib "mred-sig.ss" "mred")
	   (lib "mred.ss" "mred")
           (lib "framework.ss" "framework")
	   (lib "list.ss"))
  
  (require "sirmails.ss"
	   "pref.ss"
	   "sirmailr.ss")

  (require (lib "imap-sig.ss" "net")
	   (lib "smtp-sig.ss" "net")
	   (lib "head-sig.ss" "net")
	   (lib "base64-sig.ss" "net")
	   (lib "mime-sig.ss" "net")
	   (lib "qp-sig.ss" "net")
	   (lib "imap.ss" "net")
	   (lib "smtp.ss" "net")
	   (lib "head.ss" "net")
	   (lib "base64.ss" "net")
	   (lib "mime.ss" "net")
	   (lib "qp.ss" "net"))

  (require (lib "hierlist-sig.ss" "hierlist")
	   (lib "hierlist.ss" "hierlist"))

  ;; Constants:
  (define inbox-name "Inbox")
  (define default-mailbox-options null)

  ;; ------------------------------------------------------------
  ;; Every window (reader or sender) is in it's own
  ;; eventspace. Each should terminate by calling `exit'.
  ;; We install an exit handler so that we only actually
  ;; exit when the last window is closed.

  (define prim-exit (exit-handler))
  (define exit-eventspaces null)
  (define exit-sema (make-semaphore 1))
  (define (exit-sirmail where)
    (let ([evtsp (current-eventspace)])
      ;; Lock is because a separate process might be calling exit
      ;;  or starting up
      (semaphore-wait exit-sema)
      (set! exit-eventspaces (remq evtsp exit-eventspaces))
      (when (null? exit-eventspaces)
	(save-prefs)
	(prim-exit 0))
      (semaphore-post exit-sema)))

  ;; This function is called to start a new window:
  (define (start-new-window thunk)
    (define evtsp (make-eventspace))
    (parameterize ([current-eventspace evtsp])
      (semaphore-wait exit-sema)
      (set! exit-eventspaces (cons evtsp exit-eventspaces))
      (semaphore-post exit-sema)
      (queue-callback
       (lambda ()
	 (exit-handler (lambda (x) (exit-sirmail "a")))
	 (let ([eeh (error-escape-handler)])
	   (error-escape-handler
	    (lambda () 
	      (unless (pair? (get-top-level-windows))
		;; Didn't start up...
		(exit-sirmail "b"))
	      (eeh))))
	 (thunk)
	 (yield 'wait)
	 (exit-sirmail "c")))))

  ;; Reader windows -----------------------------------------------------------
  
  ;; This function uses `start-new-window' to open a reader window.
  ;; A reader window is implemented by an instance of the sirmail@ unit.
  (define open-mailbox
    (case-lambda
     [(mailbox-name) (open-mailbox mailbox-name default-mailbox-options)]
     [(mailbox-name mailbox-options)
      (start-new-window
       (lambda ()
	 (invoke-unit/sig
	  sirmail@
	  sirmail:environment^
	  mred^
	  net:imap^
	  net:smtp^
	  net:head^
	  net:base64^
	  net:mime^
	  net:qp^
	  hierlist^)))]))

  ;; There's only one Folders window ----------------------------------------
  
  (require "optionr.ss"
	   "folderr.ss")

  (define folders-window #f)
  (define folders-lock (make-semaphore 1))
  
  (define (with-folders-lock t)
    (dynamic-wind
     (lambda () (semaphore-wait folders-lock))
     t
     (lambda () (semaphore-post folders-lock))))
  
  (define (open-folders-window)
    (with-folders-lock
     (lambda ()
       (if folders-window
           (send folders-window show #t)
           (let ([shutdown-folders-window
                  (lambda ()
                    (with-folders-lock
                     (lambda ()
                       (set! folders-window #f)
                       (exit-sirmail "d"))))]
                 [mailbox-name inbox-name]
                 [mailbox-options default-mailbox-options])
             (start-new-window
              (lambda ()
                (set! folders-window
                      (invoke-unit/sig
                       (compound-unit/sig
                         (import [env : sirmail:environment^]
                                 [s : (shutdown-folders-window)]
                                 [mred : mred^]
                                 [imap : net:imap^]
                                 [hierlist : hierlist^])
                         (link [options : sirmail:options^
                                        (option@
                                         env
                                         imap
                                         mred)]
                               [folder : ()
                                       (folder@
                                        env
                                        s
                                        options
                                        mred imap
                                        hierlist)])
                         (export))
                       sirmail:environment^
                       (shutdown-folders-window)
                       mred^
                       net:imap^
                       hierlist^)))))))))
     
  (define (get-active-folder)
    (with-folders-lock
     (lambda ()
       (and folders-window
            (send folders-window get-mailbox-name)))))

  ;; Set Quit handler to try closing all windows --------------------

  (define asking-for-quit? #f)

  (application-quit-handler
   (lambda ()
     (if asking-for-quit?
	 (let ([l (get-top-level-windows)])
	   (when (pair? l)
	     ;; Createa thread because it's probably a dialog...
	     (thread (lambda () (send (car l) show #t)))))
	 (dynamic-wind
	     (lambda () (set! asking-for-quit? #t))
	     (lambda ()
	       (when (= 1 (message-box/custom
			   "Confirm Quit"
			   "Really quit?"
			   "Quit" "Cancel" #f
			   #f '(default=1)
			   2))
		 (let ([l (begin
			    (semaphore-wait exit-sema)
			    (begin0
			     exit-eventspaces
			     (semaphore-post exit-sema)))])
		   (for-each
		    (lambda (evtsp)
		      (parameterize ([current-eventspace evtsp])
			(queue-callback
			 (lambda ()
			   (let ([f (get-top-level-edit-target-window)])
			     (when (and f (f . is-a? . frame%))
			       (when (send f can-close?)
				 (send f on-close)
				 (send f show #f)))))
			 #f)))
		    l))))
	     (lambda () (set! asking-for-quit? #f))))))

  ;; We start by opening "Inbox" ----------------------------------------
  
  (open-mailbox inbox-name)

  ;; Wait for an explicit exit
  (yield (make-semaphore)))
