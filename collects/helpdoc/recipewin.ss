
;; A window containing the HtDP Design Recipe.  The recipe itself
;; is defined by an external file, recipe.ss, in the same directory as
;; this code. The recipe text is post-processed in an ad hoc manner to
;; convert some LaTeX annotations to MrEd styles in an editor;
;; currently supported:
;;     {\em ... \/}
;;     \scm{}
;;     \ldots
;;     ^o
;;     $ (just eliminated)
;;     \ (space)
;;     \begin{enumerate}, \item, \begin{enumerate}

(define steps (with-input-from-file 
		  ;; Same directory as this file...
		  (build-path (current-load-relative-directory) "recipe.ss")
		read))
     
;; Load the hierarchical list implementation
(require-library "hierlist.ss" "hierlist")

(define f (make-object frame% "Recipe Demo" #f 500 300))
(define l (make-object hierarchical-list% f))

;; No hilighting selections:
(send l selectable #f)

;; Some styles (style deltas, actually):
(define bold (make-object style-delta% 'change-bold))
(define emphasize (make-object style-delta% 'change-italic))
(define normal (make-object style-delta% 'change-normal))
(define scheme (make-object style-delta% 'change-family 'modern))

; text->text conversions
(define (fixup-text t)
  (if (string? t)
      (let* ([t (regexp-replace* (format "[~c~c]" #\newline #\return)
				 t " ")]
	     [t (regexp-replace* "\\\\ " t " ")]
	     [t (regexp-replace* "  " t " ")]
	     [t (regexp-replace* "[$]" t "")]
	     [t (regexp-replace* "\\\\ldots" t "...")]
	     [t (regexp-replace* " *\\\\begin{enumerate} *" t "")]
	     [t (regexp-replace* " *\\\\item *" t (string #\newline #\space #\* #\space))]
	     [t (regexp-replace* " *\\\\end{enumerate} *" t (string #\newline))])
	t)
      t))

;; text -> formatted-text conversions
(define tex-emphasize "{\\em ")
(define tex-emphasize-end "\\/}")
(define tex-scheme "\\scheme{")
(define tex-scheme-end "}")
(define tex-degree "^o")
(define (fixup-text-in-editor! e start)
  (define (fixup-style! tex tex-end style)
    (cond
     [(send e find-string tex 'forward start)
      => (lambda (pos)
	   (send e delete pos (+ pos (string-length tex)))
	   (let ([end (send e find-string tex-end 'forward pos)])
	     (when end
	       (send e delete end (+ end (string-length tex-end)))
	       (send e change-style style pos end)))
	   (fixup-style! tex tex-end style))]
     [else (void)]))
  (fixup-style! tex-emphasize tex-emphasize-end emphasize)
  (fixup-style! tex-scheme tex-scheme-end scheme)
  ;; A lot of work to draw a degree mark:
  (let ([pos (send e find-string tex-degree 'forward start)])
    (when pos
      (send e delete pos (+ pos (string-length tex-degree)))
      (let* ([s (make-object image-snip%)]
	     [bm (make-object bitmap% 6 6)]
	     [dc (make-object bitmap-dc% bm)])
	(send dc clear)
	(send dc draw-ellipse 0 0 6 6)
	(send dc set-bitmap #f)
	(send s set-bitmap bm)
	(send e insert s pos)
	(send e change-style 
	      (make-object style-delta% 'change-alignment 'top) 
	      pos (add1 pos))))))

;; Procedure to write a string or character into a list item:
(define (add-text! s text style)
  (let ([e (send s get-editor)])
    (let ([start (send e last-position)])
      (send e insert (fixup-text text))
      (let ([end (send e last-position)])
	(send e change-style style start end))
      (fixup-text-in-editor! e start))
    (send e auto-wrap #t)))

;; Put `steps' into the hierarchical list:
(for-each
 (lambda (step)
   (let ([s (send l new-list)])
     (add-text! s (car step) bold)
     (add-text! s #\newline normal)
     (add-text! s (cadr step) normal)
     (for-each
      (lambda (q&a)
	(let ([q (send s new-list)])
	  (add-text! q (car q&a) normal)
	  (let ([a (send q new-item)])
	    (add-text! a (cadr q&a) normal))))
      (cddr step))))
 steps)

(lambda () (send f show #t))
