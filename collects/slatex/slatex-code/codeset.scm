;codeset.scm
;SLaTeX Version 2.4
;Displays the typeset code made by SLaTeX
;(c) Dorai Sitaram, Rice U., 1991, 1995

(eval-within slatex 

  (define slatex::display-tex-line
    (lambda (line)
      (cond;((and (flush-comment-line? line)
       ;          (char=? (of line =char / 1) #\%))
       ;     (display "\\ZZZZschemecodebreak" *out*)
       ;     (newline *out*))
       (else
	(let loop ((i (if (flush-comment-line? line) 1 0)))
	  (let ((c (of line =char / i)))
	    (if (char=? c #\newline)
		(if (not (eq? (of line =tab / i) &void-tab))
		    (newline *out*))
		(begin (write-char c *out*) (loop (+ i 1))))))))))

  (define slatex::display-scm-line
    (lambda (line)
      (let loop ((i 0))
	(let ((c (of line =char / i)))
	  (cond ((char=? c #\newline)
		 (let ((tab (of line =tab / i)))
		   (cond ((eq? tab &tabbed-crg-ret)
			  (display "\\\\%" *out*)
			  (newline *out*))
			 ((eq? tab &plain-crg-ret) (newline *out*))
			 ((eq? tab &void-tab)
			  (write-char #\% *out*)
			  (newline *out*)))))
		((eq? (of line =notab / i) &begin-comment)
		 (display-tab (of line =tab / i) *out*)
		 (write-char c *out*)
		 (loop (+ i 1)))
		((eq? (of line =notab / i) &mid-comment)
		 (write-char c *out*)
		 (loop (+ i 1)))
		((eq? (of line =notab / i) &begin-string)
		 (display-tab (of line =tab / i) *out*)
		 (display "\\dt{" *out*)
		 (if (char=? c #\space)
		     (display-space (of line =space / i) *out*)
		     (display-tex-char c *out*))
		 (loop (+ i 1)))
		((eq? (of line =notab / i) &mid-string)
		 (if (char=? c #\space)
		     (display-space (of line =space / i) *out*)
		     (display-tex-char c *out*))
		 (loop (+ i 1)))
		((eq? (of line =notab / i) &end-string)
		 (if (char=? c #\space)
		     (display-space (of line =space / i) *out*)
		     (display-tex-char c *out*))
		 (write-char #\} *out*)
		 (if *in-qtd-tkn* (set! *in-qtd-tkn* #f)
		     (if *in-mac-tkn* (set! *in-mac-tkn* #f)))
		 (loop (+ i 1)))
		((eq? (of line =notab / i) &begin-math)
		 (display-tab (of line =tab / i) *out*)
		 (write-char c *out*)
		 (loop (+ i 1)))
		((eq? (of line =notab / i) &mid-math)
		 (write-char c *out*)
		 (loop (+ i 1)))
		((eq? (of line =notab / i) &end-math)
		 (write-char c *out*)
		 (if *in-qtd-tkn* (set! *in-qtd-tkn* #f)
		     (if *in-mac-tkn* (set! *in-mac-tkn* #f)))
		 (loop (+ i 1)))
		;	      ((memq (of line =notab / i) (list &mid-math &end-math))
		;	       (write-char c *out*)
		;	       (loop (+ i 1)))
		((char=? c #\space)
		 (display-tab (of line =tab / i) *out*)
		 (display-space (of line =space / i) *out*)
		 (loop (+ i 1)))
		((char=? c #\')
		 (display-tab (of line =tab / i) *out*)
		 (write-char c *out*)
		 (if (not (or *in-qtd-tkn* (> *in-bktd-qtd-exp* 0)))
		     (set! *in-qtd-tkn* #t))
		 (loop (+ i 1)))
		((char=? c #\`)
		 (display-tab (of line =tab / i) *out*)
		 (write-char c *out*)
		 (if (or (null? *bq-stack*)
			 (of (car *bq-stack*) =in-comma))
		     (set! *bq-stack*
			   (cons (let ((f (make-bq-frame)))
				   (setf (of f =in-comma) #f)
				   (setf (of f =in-bq-tkn) #t)
				   (setf (of f =in-bktd-bq-exp) 0)
				   f)
				 *bq-stack*)))
		 (loop (+ i 1)))
		((char=? c #\,)
		 (display-tab (of line =tab / i) *out*)
		 (write-char c *out*)
		 (if (not (or (null? *bq-stack*)
			      (of (car *bq-stack*) =in-comma)))
		     (set! *bq-stack*
			   (cons (let ((f (make-bq-frame)))
				   (setf (of f =in-comma) #t)
				   (setf (of f =in-bq-tkn) #t)
				   (setf (of f =in-bktd-bq-exp) 0)
				   f)
				 *bq-stack*)))
		 (if (char=? (of line =char / (+ i 1)) #\@)
		     (begin (display-tex-char #\@ *out*) (loop (+ 2 i)))
		     (loop (+ i 1))))
		((memv c '(#\( #\[))
		 (display-tab (of line =tab / i) *out*)
		 (write-char c *out*)
		 (cond (*in-qtd-tkn* (set! *in-qtd-tkn* #f)
				     (set! *in-bktd-qtd-exp* 1))
		       ((> *in-bktd-qtd-exp* 0)
			(set! *in-bktd-qtd-exp* (+ *in-bktd-qtd-exp* 1))))
		 (cond (*in-mac-tkn* (set! *in-mac-tkn* #f)
				     (set! *in-bktd-mac-exp* 1))
		       ((> *in-bktd-mac-exp* 0) ;is this possible?
			(set! *in-bktd-mac-exp* (+ *in-bktd-mac-exp* 1))))
		 (if (not (null? *bq-stack*))
		     (let ((top (car *bq-stack*)))
		       (cond ((of top =in-bq-tkn)
			      (setf (of top =in-bq-tkn) #f)
			      (setf (of top =in-bktd-bq-exp) 1))
			     ((> (of top =in-bktd-bq-exp) 0)
			      (setf (of top =in-bktd-bq-exp)
				    (+ (of top =in-bktd-bq-exp) 1))))))
		 (if (not (null? *case-stack*))
		     (let ((top (car *case-stack*)))
		       (cond ((of top =in-ctag-tkn)
			      (setf (of top =in-ctag-tkn) #f)
			      (setf (of top =in-bktd-ctag-exp) 1))
			     ((> (of top =in-bktd-ctag-exp) 0)
			      (setf (of top =in-bktd-ctag-exp)
				    (+ (of top =in-bktd-ctag-exp) 1)))
			     ((> (of top =in-case-exp) 0)
			      (setf (of top =in-case-exp)
				    (+ (of top =in-case-exp) 1))
			      (if (= (of top =in-case-exp) 2)
				  (set! *in-qtd-tkn* #t))))))
		 (loop (+ i 1)))
		((memv c '(#\) #\]))
		 (display-tab (of line =tab / i) *out*)
		 (write-char c *out*)
		 (if (> *in-bktd-qtd-exp* 0)
		     (set! *in-bktd-qtd-exp* (- *in-bktd-qtd-exp* 1)))
		 (if (> *in-bktd-mac-exp* 0)
		     (set! *in-bktd-mac-exp* (- *in-bktd-mac-exp* 1)))
		 (if (not (null? *bq-stack*))
		     (let ((top (car *bq-stack*)))
		       (if (> (of top =in-bktd-bq-exp) 0)
			   (begin
                             (setf (of top =in-bktd-bq-exp)
				   (- (of top =in-bktd-bq-exp) 1))
			     (if (= (of top =in-bktd-bq-exp) 0)
				 (set! *bq-stack* (cdr *bq-stack*)))))))
		 (let loop ()
		   (if (not (null? *case-stack*))
		       (let ((top (car *case-stack*)))
			 (cond ((> (of top =in-bktd-ctag-exp) 0)
				(setf (of top =in-bktd-ctag-exp)
				      (- (of top =in-bktd-ctag-exp) 1))
				(if (= (of top =in-bktd-ctag-exp) 0)
				    (setf (of top =in-case-exp) 1)))
			       ((> (of top =in-case-exp) 0)
				(setf (of top =in-case-exp)
				      (- (of top =in-case-exp) 1))
				(if (= (of top =in-case-exp) 0)
				    (begin
                                      (set! *case-stack* (cdr *case-stack*))
				      (loop))))))))
		 (loop (+ i 1)))
		(else (display-tab (of line =tab / i) *out*)
		      (loop (slatex::do-token line i))))))))

  (define slatex::do-token
    (let ((token-delims (list #\( #\) #\[ #\] #\space *return*
			      #\newline #\, #\;)))
      (lambda (line i)
	(let loop ((buf '()) (i i))
	  (let ((c (of line =char / i)))
	    (cond ((char=? c #\\ )
		   (loop (cons (of line =char / (+ i 1)) (cons c buf))
			 (+ i 2)))
		  ((or (memv c token-delims)
		       (memv c *math-triggerers*))
		   (slatex::output-token (list->string (reverse! buf)))
		   i)
		  ((char? c) (loop (cons (of line =char / i) buf) (+ i 1)))
		  (else (error "do-token: token contains non-char ~s?"
			       c))))))))

  (define slatex::output-token
    (lambda (token)
      (if (not (null? *case-stack*))
	  (let ((top (car *case-stack*)))
	    (if (of top =in-ctag-tkn)
		(begin
                  (setf (of top =in-ctag-tkn) #f)
		  (setf (of top =in-case-exp) 1)))))
      (if (lassoc token special-symbols (function token=?))
	  (begin
            (if *in-qtd-tkn* (set! *in-qtd-tkn* #f)
                (if *in-mac-tkn* (set! *in-mac-tkn* #f)))
	    (display (cdr (lassoc token special-symbols (function token=?)))
		     *out*))
	  (display-token
	   token
	   (cond (*in-qtd-tkn*
		  (set! *in-qtd-tkn* #f)
		  (cond ((equal? token "else") 'syntax)
			((lmember token data-tokens (function token=?)) 'data)
			((lmember token constant-tokens (function token=?))
			 'constant)
			((lmember token variable-tokens (function token=?))
			 'constant)
			((lmember token keyword-tokens (function token=?))
			 'constant)
			((prim-data-token? token) 'data)
			(else 'constant)))
		 ((> *in-bktd-qtd-exp* 0) 'constant)
		 ((and (not (null? *bq-stack*))
		       (not (of (car *bq-stack*) =in-comma))) 'constant)
		 (*in-mac-tkn* (set! *in-mac-tkn* #f)
			       (set-keyword token) 'syntax)
		 ((> *in-bktd-mac-exp* 0) (set-keyword token) 'syntax)
		 ((lmember token data-tokens (function token=?)) 'data)
		 ((lmember token constant-tokens (function token=?)) 'constant)
		 ((lmember token variable-tokens (function token=?)) 'variable)
		 ((lmember token keyword-tokens (function token=?))
		  (cond ((token=? token "quote") (set! *in-qtd-tkn* #t))
			((lmember token macro-definers (function token=?))
			 (set! *in-mac-tkn* #t))
			((lmember token case-and-ilk (function token=?))
			 (set! *case-stack*
			   (cons (let ((f (make-case-frame)))
				   (setf (of f =in-ctag-tkn) #t)
				   (setf (of f =in-bktd-ctag-exp) 0)
				   (setf (of f =in-case-exp) 0)
				   f)
				 *case-stack*))))
		  'syntax)
		 ((prim-data-token? token) 'data)
		 (else 'variable))
	   *out*))
      (if (and (not (null? *bq-stack*)) (of (car *bq-stack*) =in-bq-tkn))
	  (set! *bq-stack* (cdr *bq-stack*)))))
  )