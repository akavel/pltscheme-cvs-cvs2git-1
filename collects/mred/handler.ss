; File Formats and Modes

  (unit/sig mred:handler^
    (import [wx : wx^]
	    [mred:constants : mred:constants^]
	    [mred:gui-utils : mred:gui-utils^]
	    [mred:editor-frame : mred:editor-frame^]
	    [mred:finder : mred:finder^]
	    [mred:group : mred:group^]
	    [mred:hyper-frame : mred:hyper-frame^]
	    [mred:canvas : mred:canvas^]
	    [mred:container : mred:container^]
	    [mred:edit : mred:edit^]
	    [mred:preferences : mred:preferences^]
	    [mred:test : mred:testable-window^]
	    [mzlib:file : mzlib:file^])
	    
    (mred:debug:printf 'invoke "mred:handler@")

    (define-struct handler (name extension handler))

    (define format-handlers '())
    (define mode-handlers '())

    ; Inserting format & mode handlers
    (define make-insert-handler
      (letrec ([string-list?
		(lambda (l)
		  (cond
		   [(null? l) #t]
		   [(not (pair? l)) #f]
		   [else
		    (and (string? (car l))
			 (string-list? (cdr l)))]))])
	(lambda (who name extension handler)
	  (cond
	   [(not (string? name))
	    (error who "name was not a string")]
	   [(and (not (procedure? extension))
		 (not (string? extension))
		 (not (string-list? extension)))
	    (error who
		   "extension was not a string, list of strings, or a predicate")]
	   [(not (procedure? handler))
	    (error who "handler was not a function")]
	   [else (make-handler name
			       extension
			       handler)]))))
    
    (define insert-format-handler
      (lambda args
	(set! format-handlers 
	      (cons (apply make-insert-handler 'insert-format-handler args)
		    format-handlers))))
    
    (define insert-mode-handler
      (lambda args
	(set! mode-handlers 
	      (cons (apply make-insert-handler 'insert-mode-handler args)
		    mode-handlers))))

    ; The default mode: 
    (insert-mode-handler "None" () (lambda args #f))

    ; Finding format & mode handlers
    (define find-handler
      (lambda (name handlers)
	(let/ec exit
	  (let ([extension (if (string? name)
			       (or (mzlib:file:filename-extension name)
				   "")
			       "")])
	    (for-each
	     (lambda (handler)
	       (let ([ext (handler-extension handler)])
		 (when (or (and (procedure? ext)
				(ext name))
			   (and (string? ext)
				(string=? ext extension))
			   (and (pair? ext)
				(ormap (lambda (ext) 
					 (string=? ext extension))
				       ext)))
		   (exit (handler-handler handler)))))
	     handlers)
	    #f))))
    
    (define find-format-handler
      (lambda (name)
	(find-handler name format-handlers)))
    
    (define find-mode-handler
      (lambda (name)
	(find-handler name mode-handlers)))

    ; Finding format & mode handlers by name
    (define find-named-handler
      (lambda (name handlers)
	(let loop ([l handlers])
	  (cond
	   [(null? l) #f]
	   [(string-ci=? (handler-name (car l)) name)
	    (handler-handler (car l))]
	   [else (loop (cdr l))]))))
    
    (define find-named-format-handler
      (lambda (name)
	(find-named-handler name format-handlers)))
    
    (define find-named-mode-handler
      (lambda (name)
	(find-named-handler name mode-handlers)))

    (define edit-file-consult-group (make-parameter #t))

    ; Open a file for editing
    (define edit-file
      (opt-lambda (filename
		   [make-default
		    (lambda (filename)
		      (make-object mred:editor-frame:editor-frame%
				   filename #t))]
		   [consult-group? (edit-file-consult-group)])
	(mred:gui-utils:show-busy-cursor
	 (lambda ()
	   (if filename
	       (let ([already-open (and consult-group?
					(send mred:group:the-frame-group
					      locate-file
					      filename))])
		 (if already-open
		     (begin
		       (send already-open show #t)
		       already-open)
		     (let ([handler
			    (if (string? filename)
				(find-format-handler filename)
				#f)])
		       (if handler
			   (handler filename)
			   (make-default filename)))))
	       (make-default filename))))))
    
    (wx:application-file-handler edit-file)

    (define get-url-from-user
      (lambda ()
	(let* ([frame (make-object mred:container:dialog-box% 
				   (or (mred:test:test:get-active-frame)
				       null)
				   "Choose URL" #t)]
	       [main (make-object mred:container:vertical-panel% frame)]
	       [one-line (make-object mred:canvas:one-line-canvas% main)]
	       [valid? #f]
	       [ok-callback (lambda x (set! valid? #t) (send frame show #f))]
	       [answer (make-object mred:edit:return-edit% ok-callback)]
	       [bottom (make-object mred:container:horizontal-panel% main)]
	       [space (make-object mred:container:horizontal-panel% bottom)]
	       [bookmarks (mred:preferences:get-preference 'mred:bookmarks)]
	       [bk-choice
		(make-object mred:container:choice% bottom
			     (lambda (box evt)
			       (let ([which (send evt get-command-int)])
				 (when (<= 0 which)
				   (send* answer
				     (begin-edit-sequence)
				     (erase)
				     (insert (list-ref bookmarks which))
				     (end-edit-sequence)))))
			     "Bookmarks" -1 -1 -1 -1 bookmarks)]
	       [browse (make-object mred:container:button%
				    bottom
				    (lambda x
				      (let ([ans 
					     (parameterize ([mred:finder:dialog-parent-parameter
							     frame])
					       (mred:finder:get-file))])
					(when ans
					  (send* answer
					    (begin-edit-sequence)
					    (erase)
					    (insert "file:")
					    (insert ans)
					    (end-edit-sequence)))))
				    "Browse...")]
	       [cancel (make-object mred:container:button% bottom
				    (lambda x 
				      (send frame show #f))
				    "Cancel")]
	       [ok (make-object mred:container:button% bottom
				ok-callback
				"Ok")])
	  (let ([w (max (send ok get-width)
			(send cancel get-width)
			(send browse get-width))])
	    (send ok user-min-width w)
	    (send cancel user-min-width w)
	    (send browse user-min-width w))
	  (unless (null? bookmarks)
	    (send answer insert (car bookmarks))
	    (send answer set-position 0 -1))
	  (send one-line set-focus)
	  (send one-line set-media answer)
	  (send frame set-size -1 -1 20 20)
	  (send frame centre wx:const-both)
	  (send frame show #t)
	  (and valid? 
	       (send answer get-text)))))

    (define open-url
      (opt-lambda ([input-url #f])
	(let ([url (or input-url (get-url-from-user))])
	  (and url
	      (make-object mred:hyper-frame:hyper-view-frame% url)))))
    
    ; Query the user for a file and then edit it
    (define open-file
      (lambda ()
	(let ([file 
	       (parameterize ([mred:finder:dialog-parent-parameter
			       (mred:test:test:get-active-frame)])
		 (mred:finder:get-file))])
	  (and file
	       (edit-file file))))))
