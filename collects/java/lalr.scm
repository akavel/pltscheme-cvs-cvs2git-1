;;; ------------------------------------------------------------------------
;;; This is the lalr(1) generator constructed by Dominique Boucher.  I
;;; have being using it on several projects, and I have made
;;; corrections to the code. Mario Latendresse
;;;
;;; Bug fixes
;;;
;;; 17 May 2000
;;;
;;;   Several vectors were created by make-vector but with no
;;;   specification on the second argument. The code assumes that this
;;;   second argument is #f. On some systems (e.g. mzscheme) this is
;;;   not the initial value by default. Therefore, all of these
;;;   make-vector have been modified to specify the value #f.
;;; 
;;; ------------------------------------------------------------------------


;;; -------------------------------------------------------------------------
;;; lalr.scm -- Un compilateur de compilateur efficace pour Scheme
;;; -------------------------------------------------------------------------
;;; Le pr�sent fichier contient un g�n�rateur d'analyseurs syntaxiques 
;;; LALR(1) �crits en Scheme, pour Scheme. � la diff�rence d'autres 
;;; g�n�rateurs �crits en Scheme, celui-ci utilise une technique tr�s
;;; rapide pour calculer les ensembles de pr�vision. L'algorithme utilis�
;;; est le m�me que celui contenu dans Bison (GNU yacc) et est d�crit dans
;;; l'article suivant:
;;;
;;; "Efficient Computation of LALR(1) Look-Ahead Set", F. DeRemer et
;;; T. Pennello, TOPLAS, vol. 4, no. 4, october 1982.
;;;
;;; L'implication imm�diate est que le programme n'est pas �crit dans 
;;; un style purement fonctionnel. Les effets de bord sont nombreux,
;;; la rapidit� d'ex�cution ayant �t� le but recherch�.
;;;
;;; Le programme a �t� test� sur diff�rent syst�mes Scheme:
;;;  - scm4e1 sur DEC Alpha, Linux, DOS, Sun, etc.
;;;  - Gambit v2.2 (interpr�t� et compil�)
;;;  - MIT-Scheme 7.2.0 (microcode 11.127, runtime 14.160) (interp. et comp.)
;;; -------------------------------------------------------------------------
;;; L'UTILISATION DU PROGRAMME
;;;
;;; Pour g�n�rer un analyseur syntaxique pour une grammaire donn�e, il
;;; faut d'abord �crire cette derni�re en Scheme. La prochaine section
;;; d�crit la syntaxe des grammaires. Supposons donc qu'une telle
;;; grammaire ait �t� d�finie par l'expression suivante :
;;;
;;;    (define ma-grammaire { grammaire })
;;;
;;; il suffit d'�valuer
;;;
;;;    (gen-lalr1 ma-grammaire "fichier" [pr�fixe])
;;;
;;; pour g�n�rer l'analyseur. "fichier" est le nom du fichier dans lequel
;;; l'analyseur sera �crit. Le dernier argument, qui est optionnel, 
;;; permet d'ajouter un pr�fixe au nom de l'analyseur afin de faire
;;; coexister plusieurs analyseurs dans la m�me application. Ce pr�fixe
;;; doit �tre un symbole Scheme, sinon il sera ignor�.
;;;
;;; Pour ex�cuter l'analyseur syntaxique, il faut d'abord charger le
;;; `driver' d'analyse LR:
;;;
;;;      (load "lr-dvr.scm")
;;;
;;; Pour d�marrer l'analyse, il suffit d'�valuer:
;;;
;;;    ([pr�fixe-]parse lexer errorp)
;;;
;;; o� `lexer' est le nom de l'analyseur lexical qui fournira les 
;;; symboles terminaux reconnus � l'analyseur syntaxique. Chaque 
;;; terminal doit �tre une paire (token . lval) o� `token' est
;;; le num�ro du terminal et `lval' est un attribut pour ce terminal.
;;; `errorp' est une fonction d'erreur d�finie par l'usager (la 
;;; proc�dure standard `error' peut �tre utilis�e par d�faut).
;;;
;;; Voici quelques notes importantes concernant l'analyseur lexical
;;; et la fonction d'erreur:
;;;
;;;   - lorsque l'analyseur lexical veut signifier la fin de fichier,
;;;     il doit retourner '(0) � chaque fois qu'il est appel�.
;;;
;;;   - le num�ro d'un terminal est d�termin� par sa position dans 
;;;     la grammaire. Le premier terminal vaut 1, le deuxi�me 2, etc.
;;;     (Voir l'exemple qui suit).
;;;
;;;   - Le fichier g�n�r�, contenant les tables d'analyse, contient
;;;     des d�clarations pour les terminaux. Si la grammaire contient
;;;     un terminal `foo', alors le fichier contiendra une d�claration
;;;        (define [pr�fixe-]foo-tok n)
;;;     o� $n$ est la position du terminal dans la grammaire. Ceci
;;;     permet de ne pas avoir � se soucier des num�ros associ�s aux
;;;     terminaux lors de l'�criture de l'analyseur lexical. 
;;;     
;;;   - La fonction d'erreur doit accepter au moins deux arguments, 
;;;     un message d'erreur et le terminal qui a provoqu� l'erreur.
;;;     (la proc�dure `error' accepte un nombre variable d'arguments,
;;;     donc elle en accepte au moins deux).
;;;
;;; -------------------------------------------------------------------------
;;; LE FORMAT DE LA GRAMMAIRE
;;;
;;; La grammaire est sp�cifi�e en donnant d'abord la liste des terminaux 
;;; et ensuite la liste des d�finitions des cat�gories (non-terminaux).
;;; Chaque d�finition de cat�gorie est une liste dont le premier �l�ment
;;; est le nom de la cat�gorie et les autres �l�ments sont les parties
;;; droites des r�gles associ�es � la cat�gorie (des listes de symboles). 
;;; De plus, une action s�mantique peut �tre ajout�e � chaque partie
;;; droite d'une r�gle.
;;;
;;; Consid�rez la grammaire suivante:
;;;
;;;   e --> e '+' t  | t
;;;   t --> t '*' f  | f
;;;   f --> ID
;;;
;;; o� '+', '*' et ID sont des terminaux
;;;
;;; Cette grammaire, �crite en Scheme, ressemblerait � ceci (avec les
;;; actions s�mantiques ajout�es):
;;;
;;; (define ma-grammaire
;;;   '( 
;;;     ; Les terminaux d'abord
;;;     ADD 
;;;     MULT 
;;;     ID
;;;     ; Les productions viennent ensuite
;;;     (e (e ADD t)  : (+ $1 $3)
;;;        (t)        : $1
;;;        )
;;;     (t (t MULT f) : (* $1 $3)
;;;        (f)        : $1
;;;        )
;;;     (f (ID)       : $1)
;;;    ))
;;;
;;; Dans les actions s�mantiques, les symbole $<n> r�f�re � l'attribut
;;; synth�tis� du ni�me symbole de la production. La valeur de l'attribut
;;; associ� au symbole � gauche de la production est le r�sultat de 
;;; l'�valuation de l'action s�mantique. L'action par d�faut retourne
;;; un vecteur dont le premier champ est un symbole identifiant la 
;;; production (dans l'exemple: e-1, e-2, t-1, t-2 ou f-1) et dont les 
;;; autres champs contiennent les attributs des symboles de la production.
;;; (Donc, par d�faut, l'analyseur construira un arbre d'analyse.)
;;; 
;;; Si on �value:
;;;
;;;    (gen-lalr1 ma-grammaire "foo.scm" 'mon) 
;;;
;;; alors le fichier "foo.scm" contiendra la d�finition d'un analyseur
;;; dont le nom sera `mon-parser'.
;;; 
;;; NOTE SUR LA R�SOLUTION DES CONFLITS
;;; 
;;; Le conflits dans la grammaire sont r�solus de mani�re conventionnelle:
;;; un conflit Shift/Reduce est r�solu en un 'shift' et un conflit 
;;; Reduce/Reduce est r�solu en faisant une r�duction par la r�gle
;;; apparaissant en premier dans la grammaire.
;;; -------------------------------------------------------------------------
;;; Dominique Boucher (July 28, 1994)
;;; -------------------------------------------------------------------------

(define BITS-PER-WORD #f)
(define logical-or #f)

;;; ---------- SYSTEM DEPENDENT SECTION -----------------

;; -------- SCM
'(begin
  (defmacro def-macro (args body)
    `(defmacro ,(car args) ,(cdr args) ,body))
  
  (def-macro (BITS-PER-WORD) 24)
  (def-macro (logical-or x . y) `(logior ,x ,@y))
  )

;; -------- MIT-Scheme 
'(begin
  (declare (usual-integrations))
  
  (define-macro (def-macro form . body)
    `(DEFINE-MACRO ,form (LET () ,@body)))
    
  (def-macro (BITS-PER-WORD) 24)
  (def-macro (logical-or x . y) `(fix:or ,x ,@y))
  )

;; -------- Gambit
;(begin
; (declare 
;  (standard-bindings)
;  (fixnum)
;  (block)
;  (not safe)
;  )
; (set! BITS-PER-WORD 28)
; (set! logical-or (lambda (x y) (##fixnum.logior x y)))
; )

;; -------- MzScheme
(begin
 (set! BITS-PER-WORD 28)
 (set! logical-or (lambda (x y) (bitwise-ior x y)))
 )

;;; ---------- END OF SYSTEM DEPENDENT SECTION ------------

;; - Macros pour la gestion des vecteurs de bits

(define (set-bit v b)
  (let ((x (quotient b BITS-PER-WORD))
	(y (expt 2 (remainder b BITS-PER-WORD))))
    (vector-set! v x (logical-or (vector-ref v x) y))))

(define (bit-union v1 v2 n)
  (do ((i 0 (+ i 1)))
      ((= i n))
    (vector-set! v1 i (logical-or (vector-ref v1 i) 
				  (vector-ref v2 i)))))

;; - Macro pour les structures de donnees

(define (new-core)              (make-vector 4 0))
(define (set-core-number! c n)  (vector-set! c 0 n))
(define (set-core-acc-sym! c s) (vector-set! c 1 s))
(define (set-core-nitems! c n)  (vector-set! c 2 n))
(define (set-core-items! c i)   (vector-set! c 3 i))
(define (core-number c)         (vector-ref c 0))
(define (core-acc-sym c)        (vector-ref c 1))
(define (core-nitems c)         (vector-ref c 2))
(define (core-items c)          (vector-ref c 3))

(define (new-shift)              (make-vector 3 0))
(define (set-shift-number! c x)  (vector-set! c 0 x))
(define (set-shift-nshifts! c x) (vector-set! c 1 x))
(define (set-shift-shifts! c x)  (vector-set! c 2 x))
(define (shift-number s)         (vector-ref s 0))
(define (shift-nshifts s)        (vector-ref s 1))
(define (shift-shifts s)         (vector-ref s 2))

(define (new-red)                (make-vector 3 0))
(define (set-red-number! c x)    (vector-set! c 0 x))
(define (set-red-nreds! c x)     (vector-set! c 1 x))
(define (set-red-rules! c x)     (vector-set! c 2 x))
(define (red-number c)           (vector-ref c 0))
(define (red-nreds c)            (vector-ref c 1))
(define (red-rules c)            (vector-ref c 2))



(define (new-set nelem)
  (make-vector nelem 0))


(define (vector-map f v)
  (let ((vm-n (- (vector-length v) 1)))
    (let loop ((vm-low 0) (vm-high vm-n))
      (if (= vm-low vm-high)
	  (vector-set! v vm-low (f (vector-ref v vm-low) vm-low))
	  (let ((vm-middle (quotient (+ vm-low vm-high) 2)))
	    (loop vm-low vm-middle)
	    (loop (+ vm-middle 1) vm-high))))))


;; - Constantes
(define STATE-TABLE-SIZE 1009)


;; - Tableaux 
(define rrhs         #f)
(define rlhs         #f)
(define ritem        #f)
(define nullable     #f)
(define derives      #f)
(define fderives     #f)
(define firsts       #f)
(define kernel-base  #f)
(define kernel-end   #f)
(define shift-symbol #f)
(define shift-set    #f)
(define red-set      #f)
(define state-table  #f)
(define acces-symbol #f)
(define reduction-table #f)
(define shift-table  #f)
(define consistent   #f)
(define lookaheads   #f)
(define LA           #f)
(define LAruleno     #f)
(define lookback     #f)
(define goto-map     #f)
(define from-state   #f)
(define to-state     #f)
(define includes     #f)
(define F            #f)
(define action-table #f)

;; - Variables
(define nitems          #f)
(define nrules          #f)
(define nvars           #f)
(define nterms          #f)
(define nsyms           #f)
(define nstates         #f)
(define first-state     #f)
(define last-state      #f)
(define final-state     #f)
(define first-shift     #f)
(define last-shift      #f)
(define first-reduction #f)
(define last-reduction  #f)
(define nshifts         #f)
(define maxrhs          #f)
(define ngotos          #f)
(define token-set-size  #f)


(define (gen-lalr1 gram output-file . opt)
  (initialize-all)
  (rewrite-grammar 
   gram
   (lambda (terms vars gram gram/actions)
     (set! nterms (length terms))
     (set! nvars  (length vars))
     (set! nsyms  (+ nterms nvars))
     (let ((no-of-rules (length gram/actions))
	   (no-of-items (let loop ((l gram/actions) (count 0))
			  (if (null? l) 
			      count
			      (loop (cdr l) (+ count (length (caar l))))))))
       (pack-grammar no-of-rules no-of-items gram)
       (set-derives)
       (set-nullable)
       (generate-states)
       (lalr)
       (build-tables)
       (compact-action-table)
       (let* ((parser-name (if (and (pair? opt) (symbol? (car opt))) (car opt) #f))
	      (prefix      (if parser-name 
			       (string-append
				(symbol->string parser-name)
				":")
			       ""))
	      (parser-prefix (if parser-name
				  (string-append (symbol->string parser-name) "-")
				 "")))
	 (with-output-to-file output-file 
	   (lambda ()
	     (display "; *** Token Definitions ***")
	     (newline)
	     (output-token-defs terms prefix)
	     (display "; *** Action Table ***")
	     (newline)
	     (output-action-table prefix)
	     (display "; *** Goto Table ***")
	     (newline)
	     (output-goto-table prefix)
	     (display "; *** Reduction Table ***")
	     (newline)
	     (output-reduction-table gram/actions prefix)
	     (display "; *** Parser Definition ***")
	     (newline)
	     (output-parser-def parser-prefix prefix)
	     )
	   'replace
	   ))))))


(define (initialize-all)
  (set! rrhs         #f)
  (set! rlhs         #f)
  (set! ritem        #f)
  (set! nullable     #f)
  (set! derives      #f)
  (set! fderives     #f)
  (set! firsts       #f)
  (set! kernel-base  #f)
  (set! kernel-end   #f)
  (set! shift-symbol #f)
  (set! shift-set    #f)
  (set! red-set      #f)
  (set! state-table  (make-vector STATE-TABLE-SIZE '()))
  (set! acces-symbol #f)
  (set! reduction-table #f)
  (set! shift-table  #f)
  (set! consistent   #f)
  (set! lookaheads   #f)
  (set! LA           #f)
  (set! LAruleno     #f)
  (set! lookback     #f)
  (set! goto-map     #f)
  (set! from-state   #f)
  (set! to-state     #f)
  (set! includes     #f)
  (set! F            #f)
  (set! action-table #f)
  (set! nstates         #f)
  (set! first-state     #f)
  (set! last-state      #f)
  (set! final-state     #f)
  (set! first-shift     #f)
  (set! last-shift      #f)
  (set! first-reduction #f)
  (set! last-reduction  #f)
  (set! nshifts         #f)
  (set! maxrhs          #f)
  (set! ngotos          #f)
  (set! token-set-size  #f))


(define (pack-grammar no-of-rules no-of-items gram)
  (set! nrules (+  no-of-rules 1))
  (set! nitems no-of-items)
  (set! rlhs (make-vector nrules #f))
  (set! rrhs (make-vector nrules #f))
  (set! ritem (make-vector (+ 1 nitems) #f))

  (let loop ((p gram) (item-no 0) (rule-no 1))
	(if (not (null? p))
	(let ((nt (caar p)))
	  (let loop2 ((prods (cdar p)) (it-no2 item-no) (rl-no2 rule-no))
		(if (null? prods)
		(loop (cdr p) it-no2 rl-no2)
		(begin
		  (vector-set! rlhs rl-no2 nt)
		  (vector-set! rrhs rl-no2 it-no2)
		  (let loop3 ((rhs (car prods)) (it-no3 it-no2))
			(if (null? rhs)
			(begin
			  (vector-set! ritem it-no3 (- rl-no2))
			  (loop2 (cdr prods) (+ it-no3 1) (+ rl-no2 1)))
			(begin
			  (vector-set! ritem it-no3 (car rhs))
			  (loop3 (cdr rhs) (+ it-no3 1))))))))))))


;; Fonction set-derives
;; --------------------
(define (set-derives)
  (define delts (make-vector (+ nrules 1) 0))
  (define dset  (make-vector nvars -1))

  (let loop ((i 1) (j 0))		; i = 0
    (if (< i nrules)
	(let ((lhs (vector-ref rlhs i)))
	  (if (>= lhs 0)
	      (begin
		(vector-set! delts j (cons i (vector-ref dset lhs)))
		(vector-set! dset lhs j)
		(loop (+ i 1) (+ j 1)))
	      (loop (+ i 1) j)))))
  
  (set! derives (make-vector nvars 0))
  
  (let loop ((i 0))
    (if (< i nvars)
	(let ((q (let loop2 ((j (vector-ref dset i)) (s '()))
		   (if (< j 0)
		       s
		       (let ((x (vector-ref delts j)))
			 (loop2 (cdr x) (cons (car x) s)))))))
	  (vector-set! derives i q)
	  (loop (+ i 1))))))



(define (set-nullable)
  (set! nullable (make-vector nvars #f))
  (let ((squeue (make-vector nvars #f))
	(rcount (make-vector (+ nrules 1) 0))
	(rsets  (make-vector nvars #f))
	(relts  (make-vector (+ nitems nvars 1) #f)))
    (let loop ((r 0) (s2 0) (p 0))
      (let ((*r (vector-ref ritem r)))
	(if *r
	    (if (< *r 0)
		(let ((symbol (vector-ref rlhs (- *r))))
		  (if (and (>= symbol 0)
			   (not (vector-ref nullable symbol)))
		      (begin
			(vector-set! nullable symbol #t)
			(vector-set! squeue s2 symbol)
			(loop (+ r 1) (+ s2 1) p))))
		(let loop2 ((r1 r) (any-tokens #f))
		  (let* ((symbol (vector-ref ritem r1)))
		    (if (> symbol 0)
			(loop2 (+ r1 1) (or any-tokens (>= symbol nvars)))
			(if (not any-tokens)
			    (let ((ruleno (- symbol)))
			      (let loop3 ((r2 r) (p2 p))
				(let ((symbol (vector-ref ritem r2)))
				  (if (> symbol 0)
				      (begin
					(vector-set! rcount ruleno
						     (+ (vector-ref rcount ruleno) 1))
					(vector-set! relts p2
						     (cons (vector-ref rsets symbol)
							   ruleno))
					(vector-set! rsets symbol p2)
					(loop3 (+ r2 1) (+ p2 1)))
				      (loop (+ r2 1) s2 p2)))))
			    (loop (+ r1 1) s2 p))))))
	    (let loop ((s1 0) (s3 s2))
	      (if (< s1 s3)
		  (let loop2 ((p (vector-ref rsets (vector-ref squeue s1))) (s4 s3))
		    (if p 
			(let* ((x (vector-ref relts p))
			       (ruleno (cdr x))
			       (y (- (vector-ref rcount ruleno) 1)))
			  (vector-set! rcount ruleno y)
			  (if (= y 0)
			      (let ((symbol (vector-ref rlhs ruleno)))
				(if (and (>= symbol 0)
					 (not (vector-ref nullable symbol)))
				    (begin
				      (vector-set! nullable symbol #t)
				      (vector-set! squeue s4 symbol)
				      (loop2 (car x) (+ s4 1)))
				    (loop2 (car x) s4)))
			      (loop2 (car x) s4))))
		    (loop (+ s1 1) s4)))))))))
		  


; Fonction set-firsts qui calcule un tableau de taille
; nvars et qui donne, pour chaque non-terminal X, une liste des
; non-terminaux pouvant apparaitre au debut d'une derivation a
; partir de X.

(define (set-firsts)
  (set! firsts (make-vector nvars '()))
  
  ;; -- initialization
  (let loop ((i 0))
    (if (< i nvars)
	(let loop2 ((sp (vector-ref derives i)))
	  (if (null? sp)
	      (loop (+ i 1))
	      (let ((sym (vector-ref ritem (vector-ref rrhs (car sp)))))
		(if (< -1 sym nvars)
		    (vector-set! firsts i (sinsert sym (vector-ref firsts i))))
		(loop2 (cdr sp)))))))

  ;; -- reflexive and transitive closure
  (let loop ((continue #t))
    (if continue
	(let loop2 ((i 0) (cont #f))
	  (if (>= i nvars)
	      (loop cont)
	      (let* ((x (vector-ref firsts i))
		     (y (let loop3 ((l x) (z x))
			  (if (null? l)
			      z
			      (loop3 (cdr l)
				     (sunion (vector-ref firsts (car l)) z))))))
		(if (equal? x y)
		    (loop2 (+ i 1) cont)
		    (begin
		      (vector-set! firsts i y)
		      (loop2 (+ i 1) #t))))))))
  
  (let loop ((i 0))
    (if (< i nvars)
	(begin
	  (vector-set! firsts i (sinsert i (vector-ref firsts i)))
	  (loop (+ i 1))))))

; Fonction set-fderives qui calcule un tableau de taille
; nvars et qui donne, pour chaque non-terminal, une liste des regles pouvant
; etre derivees a partir de ce non-terminal. (se sert de firsts)

(define (set-fderives)
  (set! fderives (make-vector nvars #f))

  (set-firsts)

  (let loop ((i 0))
    (if (< i nvars)
	(let ((x (let loop2 ((l (vector-ref firsts i)) (fd '()))
		   (if (null? l) 
		       fd
		       (loop2 (cdr l) 
			      (sunion (vector-ref derives (car l)) fd))))))
	  (vector-set! fderives i x)
	  (loop (+ i 1))))))

; Fonction calculant la fermeture d'un ensemble d'items LR0
; ou core est une liste d'items

(define (closure core)
  ;; Initialization
  (define ruleset (make-vector nrules #f))

  (let loop ((csp core))
    (if (not (null? csp))
	(let ((sym (vector-ref ritem (car csp))))
	  (if (< -1 sym nvars)
	      (let loop2 ((dsp (vector-ref fderives sym)))
		(if (not (null? dsp))
		    (begin
		      (vector-set! ruleset (car dsp) #t)
		      (loop2 (cdr dsp))))))
	  (loop (cdr csp)))))

  (let loop ((ruleno 1) (csp core) (itemsetv '())) ; ruleno = 0
    (if (< ruleno nrules)
	(if (vector-ref ruleset ruleno)
	    (let ((itemno (vector-ref rrhs ruleno)))
	      (let loop2 ((c csp) (itemsetv2 itemsetv))
		(if (and (pair? c)
			 (< (car c) itemno))
		    (loop2 (cdr c) (cons (car c) itemsetv2))
		    (loop (+ ruleno 1) c (cons itemno itemsetv2)))))
	    (loop (+ ruleno 1) csp itemsetv))
	(let loop2 ((c csp) (itemsetv2 itemsetv))
	  (if (pair? c)
	      (loop2 (cdr c) (cons (car c) itemsetv2))
	      (reverse itemsetv2))))))



(define (allocate-item-sets)
  (set! kernel-base (make-vector nsyms 0))
  (set! kernel-end  (make-vector nsyms #f)))


(define (allocate-storage)
  (allocate-item-sets)
  (set! red-set (make-vector (+ nrules 1) 0)))

;; --


(define (initialize-states)
  (let ((p (new-core)))
    (set-core-number! p 0)
    (set-core-acc-sym! p #f)
    (set-core-nitems! p 1)
    (set-core-items! p '(0))

    (set! first-state (list p))
    (set! last-state first-state)
    (set! nstates 1)))



(define (generate-states)
  (allocate-storage)
  (set-fderives)
  (initialize-states)
  (let loop ((this-state first-state))
    (if (pair? this-state)
	(let* ((x (car this-state))
	       (is (closure (core-items x))))
	  (save-reductions x is)
	  (new-itemsets is)
	  (append-states)
	  (if (> nshifts 0)
	      (save-shifts x))
	  (loop (cdr this-state))))))

;; Fonction calculant les symboles sur lesquels il faut "shifter" 
;; et regroupe les items en fonction de ces symboles

(define (new-itemsets itemset)
  ;; - Initialization
  (set! shift-symbol '())
  (let loop ((i 0))
    (if (< i nsyms)
	(begin
	  (vector-set! kernel-end i '())
	  (loop (+ i 1)))))

  (let loop ((isp itemset))
    (if (pair? isp)
	(let* ((i (car isp))
	       (sym (vector-ref ritem i)))
	  (if (>= sym 0)
	      (begin
		(set! shift-symbol (sinsert sym shift-symbol))
		(let ((x (vector-ref kernel-end sym)))
		  (if (null? x)
		      (begin
			(vector-set! kernel-base sym (cons (+ i 1) x))
			(vector-set! kernel-end sym (vector-ref kernel-base sym)))
		      (begin
			(set-cdr! x (list (+ i 1)))
			(vector-set! kernel-end sym (cdr x)))))))
	  (loop (cdr isp)))))

  (set! nshifts (length shift-symbol)))



(define (get-state sym)
  (let* ((isp  (vector-ref kernel-base sym))
	 (n    (length isp))
	 (key  (let loop ((isp1 isp) (k 0))
		 (if (null? isp1)
		     (modulo k STATE-TABLE-SIZE)
		     (loop (cdr isp1) (+ k (car isp1))))))
	 (sp   (vector-ref state-table key)))
    (if (null? sp)
	(let ((x (new-state sym)))
	  (vector-set! state-table key (list x))
	  (core-number x))
	(let loop ((sp1 sp))
	  (if (and (= n (core-nitems (car sp1)))
		   (let loop2 ((i1 isp) (t (core-items (car sp1)))) 
		     (if (and (pair? i1) 
			      (= (car i1)
				 (car t)))
			 (loop2 (cdr i1) (cdr t))
			 (null? i1))))
	      (core-number (car sp1))
	      (if (null? (cdr sp1))
		  (let ((x (new-state sym)))
		    (set-cdr! sp1 (list x))
		    (core-number x))
		  (loop (cdr sp1))))))))


(define (new-state sym)
  (let* ((isp  (vector-ref kernel-base sym))
	 (n    (length isp))
	 (p    (new-core)))
    (set-core-number! p nstates)
    (set-core-acc-sym! p sym)
    (if (= sym nvars) (set! final-state nstates))
    (set-core-nitems! p n)
    (set-core-items! p isp)
    (set-cdr! last-state (list p))
    (set! last-state (cdr last-state))
    (set! nstates (+ nstates 1))
    p))


;; --

(define (append-states)
  (set! shift-set
	(let loop ((l shift-symbol))
	  (if (null? l)
	      '()
	      (cons (get-state (car l)) (loop (cdr l)))))))

;; --

(define (save-shifts core)
  (let ((p (new-shift)))
	(set-shift-number! p (core-number core))
	(set-shift-nshifts! p nshifts)
	(set-shift-shifts! p shift-set)
	(if last-shift
	(begin
	  (set-cdr! last-shift (list p))
	  (set! last-shift (cdr last-shift)))
	(begin
	  (set! first-shift (list p))
	  (set! last-shift first-shift)))))

(define (save-reductions core itemset)
  (let ((rs (let loop ((l itemset))
	      (if (null? l)
		  '()
		  (let ((item (vector-ref ritem (car l))))
		    (if (< item 0)
			(cons (- item) (loop (cdr l)))
			(loop (cdr l))))))))
    (if (pair? rs)
	(let ((p (new-red)))
	  (set-red-number! p (core-number core))
	  (set-red-nreds!  p (length rs))
	  (set-red-rules!  p rs)
	  (if last-reduction
	      (begin
		(set-cdr! last-reduction (list p))
		(set! last-reduction (cdr last-reduction)))
	      (begin
		(set! first-reduction (list p))
		(set! last-reduction first-reduction)))))))


;; --

(define (lalr)
  (set! token-set-size (+ 1 (quotient nterms BITS-PER-WORD)))
  (set-accessing-symbol)
  (set-shift-table)
  (set-reduction-table)
  (set-max-rhs)
  (initialize-LA) 
  (set-goto-map)
  (initialize-F)
  (build-relations)
  (digraph includes)
  (compute-lookaheads)
  )

(define (set-accessing-symbol)
  (set! acces-symbol (make-vector nstates #f))
  (let loop ((l first-state))
    (if (pair? l)
	(let ((x (car l)))
	  (vector-set! acces-symbol (core-number x) (core-acc-sym x))
	  (loop (cdr l))))))

(define (set-shift-table)
  (set! shift-table (make-vector nstates #f))
  (let loop ((l first-shift))
    (if (pair? l)
	(let ((x (car l)))
	  (vector-set! shift-table (shift-number x) x)
	  (loop (cdr l))))))

(define (set-reduction-table)
  (set! reduction-table (make-vector nstates #f))
  (let loop ((l first-reduction))
    (if (pair? l)
	(let ((x (car l)))
	  (vector-set! reduction-table (red-number x) x)
	  (loop (cdr l))))))

(define (set-max-rhs)
  (let loop ((p 0) (curmax 0) (length 0))
    (let ((x (vector-ref ritem p)))
      (if x
	  (if (>= x 0)
	      (loop (+ p 1) curmax (+ length 1))
	      (loop (+ p 1) (max curmax length) 0))
	  (set! maxrhs curmax)))))

(define (initialize-LA)
  (define (last l)
    (if (null? (cdr l))
	(car l)
	(last (cdr l))))

  (set! consistent (make-vector nstates #f))
  (set! lookaheads (make-vector (+ nstates 1) #f))

  (let loop ((count 0) (i 0))
    (if (< i nstates)
	(begin
	  (vector-set! lookaheads i count)
	  (let ((rp (vector-ref reduction-table i))
		(sp (vector-ref shift-table i)))
	    (if (and rp
		     (or (> (red-nreds rp) 1)
			 (and sp
			      (not
			       (< (vector-ref acces-symbol
					      (last (shift-shifts sp)))
				  nvars)))))
		(loop (+ count (red-nreds rp)) (+ i 1))
		(begin
		  (vector-set! consistent i #t)
		  (loop count (+ i 1))))))

	(begin
	  (vector-set! lookaheads nstates count)
	  (let ((c (max count 1)))
	    (set! LA (make-vector c #f))
	    (do ((j 0 (+ j 1))) ((= j c)) (vector-set! LA j (new-set token-set-size)))
	    (set! LAruleno (make-vector c -1))
	    (set! lookback (make-vector c #f)))
	  (let loop ((i 0) (np 0))
	    (if (< i nstates)
		(if (vector-ref consistent i)
		    (loop (+ i 1) np)
		    (let ((rp (vector-ref reduction-table i)))
		      (if rp
			  (let loop2 ((j (red-rules rp)) (np2 np))
			    (if (null? j)
				(loop (+ i 1) np2)
				(begin
				  (vector-set! LAruleno np2 (car j))
				  (loop2 (cdr j) (+ np2 1)))))
			  (loop (+ i 1) np))))))))))


(define (set-goto-map)
  (set! goto-map (make-vector (+ nvars 1) 0))
  (let ((temp-map (make-vector (+ nvars 1) 0)))
    (let loop ((ng 0) (sp first-shift))
      (if (pair? sp)
	  (let loop2 ((i (reverse (shift-shifts (car sp)))) (ng2 ng))
	    (if (pair? i)
		(let ((symbol (vector-ref acces-symbol (car i))))
		  (if (< symbol nvars)
		      (begin
			(vector-set! goto-map symbol 
				     (+ 1 (vector-ref goto-map symbol)))
			(loop2 (cdr i) (+ ng2 1)))
		      (loop2 (cdr i) ng2)))
		(loop ng2 (cdr sp))))

	  (let loop ((k 0) (i 0))
	    (if (< i nvars)
		(begin
		  (vector-set! temp-map i k)
		  (loop (+ k (vector-ref goto-map i)) (+ i 1)))

		(begin
		  (do ((i 0 (+ i 1)))
		      ((>= i nvars))
		    (vector-set! goto-map i (vector-ref temp-map i)))

		  (set! ngotos ng)
		  (vector-set! goto-map nvars ngotos)
		  (vector-set! temp-map nvars ngotos)
		  (set! from-state (make-vector ngotos #f))
		  (set! to-state (make-vector ngotos #f))
		  
		  (do ((sp first-shift (cdr sp)))
		      ((null? sp))
		    (let* ((x (car sp))
			   (state1 (shift-number x)))
		      (do ((i (shift-shifts x) (cdr i)))
			  ((null? i))
			(let* ((state2 (car i))
			       (symbol (vector-ref acces-symbol state2)))
			  (if (< symbol nvars)
			      (let ((k (vector-ref temp-map symbol)))
				(vector-set! temp-map symbol (+ k 1))
				(vector-set! from-state k state1)
				(vector-set! to-state k state2))))))))))))))


(define (map-goto state symbol)
  (let loop ((low (vector-ref goto-map symbol))
	     (high (- (vector-ref goto-map (+ symbol 1)) 1)))
    (if (> low high)
	(begin
	  (display (list "Error in map-goto" state symbol)) (newline)
	  0)
	(let* ((middle (quotient (+ low high) 2))
	       (s (vector-ref from-state middle)))
	  (cond
	   ((= s state)
	    middle)
	   ((< s state)
	    (loop (+ middle 1) high))
	   (else
	    (loop low (- middle 1))))))))


(define (initialize-F)
  (set! F (make-vector ngotos #f))
  (do ((i 0 (+ i 1))) ((= i ngotos)) (vector-set! F i (new-set token-set-size)))

  (let ((reads (make-vector ngotos #f)))

    (let loop ((i 0) (rowp 0))
      (if (< i ngotos)
	  (let* ((rowf (vector-ref F rowp))
		 (stateno (vector-ref to-state i))
		 (sp (vector-ref shift-table stateno)))
	    (if sp
		(let loop2 ((j (shift-shifts sp)) (edges '()))
		  (if (pair? j)
		      (let ((symbol (vector-ref acces-symbol (car j))))
			(if (< symbol nvars)
			    (if (vector-ref nullable symbol)
				(loop2 (cdr j) (cons (map-goto stateno symbol) 
						     edges))
				(loop2 (cdr j) edges))
			    (begin
			      (set-bit rowf (- symbol nvars))
			      (loop2 (cdr j) edges))))
		      (if (pair? edges)
			  (vector-set! reads i (reverse edges))))))
	      (loop (+ i 1) (+ rowp 1)))))
    (digraph reads)))

(define (add-lookback-edge stateno ruleno gotono)
  (let ((k (vector-ref lookaheads (+ stateno 1))))
    (let loop ((found #f) (i (vector-ref lookaheads stateno)))
      (if (and (not found) (< i k))
	  (if (= (vector-ref LAruleno i) ruleno)
	      (loop #t i)
	      (loop found (+ i 1)))

	  (if (not found)
	      (begin (display "Error in add-lookback-edge : ")
		     (display (list stateno ruleno gotono)) (newline))
	      (vector-set! lookback i
			   (cons gotono (vector-ref lookback i))))))))


(define (transpose r-arg n)
  (let ((new-end (make-vector n #f))
	(new-R  (make-vector n #f)))
    (do ((i 0 (+ i 1))) 
	((= i n))
      (let ((x (list 'bidon)))
	(vector-set! new-R i x)
	(vector-set! new-end i x)))
    (do ((i 0 (+ i 1)))
	((= i n))
      (let ((sp (vector-ref r-arg i)))
	(if (pair? sp)
	    (let loop ((sp2 sp))
	      (if (pair? sp2)
		  (let* ((x (car sp2))
			 (y (vector-ref new-end x)))
		    (set-cdr! y (cons i (cdr y)))
		    (vector-set! new-end x (cdr y))
		    (loop (cdr sp2))))))))
    (do ((i 0 (+ i 1)))
	((= i n))
      (vector-set! new-R i (cdr (vector-ref new-R i))))
    
    new-R))



(define (build-relations)

  (define (get-state stateno symbol)
    (let loop ((j (shift-shifts (vector-ref shift-table stateno)))
	       (stno stateno))
      (if (null? j)
	  stno
	  (let ((st2 (car j)))
	    (if (= (vector-ref acces-symbol st2) symbol)
		st2
		(loop (cdr j) st2))))))

  (set! includes (make-vector ngotos #f))
  (do ((i 0 (+ i 1)))
      ((= i ngotos))
    (let ((state1 (vector-ref from-state i))
	  (symbol1 (vector-ref acces-symbol (vector-ref to-state i))))
      (let loop ((rulep (vector-ref derives symbol1))
		 (edges '()))
	(if (pair? rulep)
	    (let ((*rulep (car rulep)))
	      (let loop2 ((rp (vector-ref rrhs *rulep))
			  (stateno state1)
			  (states (list state1)))
		(let ((*rp (vector-ref ritem rp)))
		  (if (> *rp 0)
		      (let ((st (get-state stateno *rp)))
			(loop2 (+ rp 1) st (cons st states)))
		      (begin

			(if (not (vector-ref consistent stateno))
			    (add-lookback-edge stateno *rulep i))
			
			(let loop2 ((done #f) 
				    (stp (cdr states))
				    (rp2 (- rp 1))
				    (edgp edges))
			  (if (not done)
			      (let ((*rp (vector-ref ritem rp2)))
				(if (< -1 *rp nvars)
				  (loop2 (not (vector-ref nullable *rp))
					 (cdr stp)
					 (- rp2 1)
					 (cons (map-goto (car stp) *rp) edgp))
				  (loop2 #t stp rp2 edgp)))

			      (loop (cdr rulep) edgp))))))))
	    (vector-set! includes i edges)))))
  (set! includes (transpose includes ngotos)))
			
(define (compute-lookaheads)
  (let ((n (vector-ref lookaheads nstates)))
    (let loop ((i 0))
      (if (< i n)
	  (let loop2 ((sp (vector-ref lookback i)))
	    (if (pair? sp)
		(let ((LA-i (vector-ref LA i))
		      (F-j  (vector-ref F (car sp))))
		  (bit-union LA-i F-j token-set-size)
		  (loop2 (cdr sp)))
		(loop (+ i 1))))))))

(define (digraph relation)
 
  (define infinity (+ ngotos 2))
  (define INDEX (make-vector (+ ngotos 1) 0))
  (define VERTICES (make-vector (+ ngotos 1) 0))
  (define top 0)
  (define R relation)

  (define (traverse i)
    (set! top (+ 1 top))
    (vector-set! VERTICES top i)
    (let ((height top))
      (vector-set! INDEX i height)
      (let ((rp (vector-ref R i)))
	(if (pair? rp)
	    (let loop ((rp2 rp))
	      (if (pair? rp2)
		  (let ((j (car rp2)))
		    (if (= 0 (vector-ref INDEX j))
			(traverse j))
		    (if (> (vector-ref INDEX i) 
			   (vector-ref INDEX j))
			(vector-set! INDEX i (vector-ref INDEX j)))
		    (let ((F-i (vector-ref F i))
			  (F-j (vector-ref F j)))
		      (bit-union F-i F-j token-set-size)
		      )
		    (loop (cdr rp2))))))
	(if (= (vector-ref INDEX i) height)
	    (let loop ()
	      (let ((j (vector-ref VERTICES top)))
		(set! top (- top 1))
		(vector-set! INDEX j infinity)
		(if (not (= i j))
		    (begin
		      (bit-union (vector-ref F i) 
				 (vector-ref F j)
				 token-set-size)
		      (loop)))))))))


  (let loop ((i 0))
    (if (< i ngotos)
	(begin
	  (if (and (= 0 (vector-ref INDEX i))
		   (pair? (vector-ref R i)))
	      (traverse i))
	  (loop (+ i 1))))))


;; --

(define (build-tables)
  (define (add-action St Sym Act)
    (let* ((x (vector-ref action-table St))
	   (y (assv Sym x)))
      (if y
	  (if (not (= Act (cdr y)))
	      ;; -- there is a conflict 
	      (begin
		(if (and (<= (cdr y) 0)
			 (<= Act 0))
		    (begin
		      (display "%% Reduce/Reduce conflict ")
		      (display "(reduce ") (display (- Act))
		      (display ", reduce ") (display (- (cdr y)))
		      (display ") on ") (display Sym)
		      (display " in state ") (display St)
		      (newline)
		      (set-cdr! y (max (cdr y) Act)))
		    (begin
		      (display "%% Shift/Reduce conflict ")
		      (display "(shift ") (display Act)
		      (display ", reduce ") (display (- (cdr y)))
		      (display ") on ") (display Sym)
		      (display " in state ") (display St)
		      (newline)
		      (set-cdr! y Act)))))
	  (vector-set! action-table St
		       (cons (cons Sym Act) x)))))
	
  (set! action-table (make-vector nstates '()))

  (do ((i 0 (+ i 1)))  ; i = state
      ((= i nstates))
    (let ((red (vector-ref reduction-table i)))
      (if (and red (>= (red-nreds red) 1))
	  (if (and (= (red-nreds red) 1) (vector-ref consistent i))
	      (add-action i 'default (- (car (red-rules red))))
	      (let ((k (vector-ref lookaheads (+ i 1))))
		(let loop ((j (vector-ref lookaheads i)))
		  (if (< j k)
		      (let ((rule (- (vector-ref LAruleno j)))
			    (lav  (vector-ref LA j)))
			(let loop2 ((token 0) (x (vector-ref lav 0)) (y 1) (z 0))
			  (if (< token nterms)
			      (begin
				(let ((in-la-set? (modulo x 2)))
				  (if (= in-la-set? 1)
				      (add-action i token rule)))
				(if (= y BITS-PER-WORD)
				    (loop2 (+ token 1) 
					   (vector-ref lav (+ z 1))
					   1
					   (+ z 1))
				    (loop2 (+ token 1) (quotient x 2) (+ y 1) z)))))
			(loop (+ j 1)))))))))

    (let ((shiftp (vector-ref shift-table i)))
      (if shiftp
	  (let loop ((k (shift-shifts shiftp)))
	    (if (pair? k)
		(let* ((state (car k))
		       (symbol (vector-ref acces-symbol state)))
		  (if (>= symbol nvars)
		      (add-action i (- symbol nvars) state))
		  (loop (cdr k))))))))

  (add-action final-state 0 'accept))

(define (compact-action-table)
  (define (most-common-action acts)
    (let ((accums '()))
      (let loop ((l acts))
	(if (pair? l)
	    (let* ((x (cdar l))
		   (y (assv x accums)))
	      (if (and (number? x) (< x 0))
		  (if y
		      (set-cdr! y (+ 1 (cdr y)))
		      (set! accums (cons `(,x . 1) accums))))
	      (loop (cdr l)))))

      (let loop ((l accums) (max 0) (sym #f))
	(if (null? l)
	    sym
	    (let ((x (car l)))
	      (if (> (cdr x) max)
		  (loop (cdr l) (cdr x) (car x))
		  (loop (cdr l) max sym)))))))

  (do ((i 0 (+ i 1)))
      ((= i nstates))
    (let ((acts (vector-ref action-table i)))
      (if (vector? (vector-ref reduction-table i))
	  (let ((act (most-common-action acts)))
	    (vector-set! action-table i
			 (cons `(default . ,(if act act 'error))
			       (filter (lambda (x) 
					 (not (eq? (cdr x) act)))
				       acts))))
	  (vector-set! action-table i 
		       (cons `(default . *error*) acts))))))


(define (output-action-table prefix)
  (display "(define ") (display prefix) (display "action-table") (newline)
  (display "  '#(") (newline)
  (do ((i 0 (+ i 1)))
      ((= i nstates))
    (display "     ")
    (write (vector-ref action-table i))
    (newline))
  (display "    ))") (newline)
  (newline))

(define (output-goto-table prefix)
  (display "(define ") (display prefix) (display "goto-table") (newline)
  (display "  '#(") (newline)
  (do ((i 0 (+ i 1)))
      ((= i nstates))
    (display "     ") 
    (let ((shifts (vector-ref shift-table i)))
      (if shifts
	  (begin
	    (display "(")
	    (let loop ((l (shift-shifts shifts)))
	      (if (null? l)
		  (display ")")
		  (let* ((state (car l))
			 (symbol (vector-ref acces-symbol state)))
		    (if (< symbol nvars)
			(display `(,symbol . ,state)))
		    (loop (cdr l))))))
	  (display '())))
    (newline))
  (display "    ))") (newline)
  (newline))

(define (output-reduction-table gram/actions prefix)
  (display "(define ") (display prefix) (display "reduction-table") (newline)
  (display "  (vector") (newline)
  (display "    '()") (newline)
  (for-each
   (lambda (p)
     (let ((act (cdr p)))
       (display "    (lambda (stack sp goto-table)") (newline)
       (let* ((nt (caar p)) (rhs (cdar p)) (n (length rhs)))
	 (display "      (let* (")
	 (if act
	     (let loop ((i 1) (l rhs))
	       (if (not (null? l))
		   (let ((rest (cdr l)))
		     (if (> i 1) (begin (newline) (display "             ")))
		     (display "($") (display (+ (- n i) 1)) (display " ")
		     (display "(vector-ref stack (- sp ")
		     (display (- (* i 2) 1))
		     (display ")))")
		     (loop (+ i 1) rest)))))
	 (display ")")
	 (newline)
	 (display "          ")
	 (if (= nt 0)
	     (display "(accept $1)")
	     (begin
	       (display "(push stack (- sp ")
	       (display (* 2 n))
	       (display ") ")
	       (display nt)
	       (display " goto-table ")
	       (write (cdr p))
	       (display ")")))
	 (display "))") (newline))))
   gram/actions)
  (display "  ))") (newline)
  (newline))

(define (output-parser-def parser-prefix prefix)
  (display "(define ") (display parser-prefix) (display "parser") (newline)
  (display "   (make-parser") (newline)
  (display "    ") (display prefix) (display "action-table") (newline)
  (display "    ") (display prefix) (display "goto-table") (newline)
  (display "    ") (display prefix) (display "reduction-table") (newline)
  (display "    ") (display prefix) (display "token-defs))") (newline))
  

(define (output-token-defs terms prefix)
  (let loop ((i 0) (l terms))
    (if (pair? l)
	(let ((x (car l)))
	  (display "(define ") (display prefix)
	  (write x) (display "-tok")
	  (display #\tab)
	  (display i)
	  (display ")")
	  (newline)
	  (loop (+ i 1) (cdr l)))))
  (newline)
  (display "(define ") (display prefix) (display "token-defs") (newline)
  (display "  (list ") (newline)
  (let loop ((i 0) (l terms))
    (if (pair? l)
	(begin
	  (display "   (cons ")
	  (display i)
	  (display " \"") (display (car l)) (display "\")")
	  (newline)
	  (loop (+ i 1) (cdr l)))))
  (display "  ))") (newline)
  (newline))

;; --

(define (rewrite-grammar grammar proc) 

  (define eoi '*EOI*)

  (if (not (pair? grammar))
      (error "Grammar definition must be a non-empty list")
      (let loop1 ((lst grammar) (rev-terms '()))
	(if (and (pair? lst) (not (pair? (car lst)))) ; definition d'un terminal?
	    (let ((term (car lst)))
	      (cond ((not (valid-terminal? term))
		     (error "Invalid terminal:" term))
		    ((member term rev-terms)
		     (error "Terminal previously defined:" term))
		    (else
		     (loop1 (cdr lst) (cons term rev-terms)))))
	    (let loop2 ((lst lst) (rev-nonterm-defs '()))
	      (if (pair? lst)
		  (let ((def (car lst)))
		    (if (not (pair? def))
			(error "Nonterminal definition must be a non-empty list")
			(let ((nonterm (car def)))
			  (cond ((not (valid-nonterminal? nonterm))
				 (error "Invalid nonterminal:" nonterm))
				((or (member nonterm rev-terms)
				     (assoc nonterm rev-nonterm-defs))
				 (error "Nonterminal previously defined:" nonterm))
				(else
				 (loop2 (cdr lst)
					(cons def rev-nonterm-defs)))))))
		  (let* ((terms (cons eoi (reverse rev-terms)))
			 (nonterm-defs (reverse rev-nonterm-defs))
			 (nonterms (cons '*start* (map car nonterm-defs))))
		    (if (= (length nonterms) 1)
			(error "Grammar must contain at least one nonterminal")
			(let ((compiled-nonterminals
			       (map (lambda (nonterm-def)
				      (rewrite-nonterm-def nonterm-def
							   terms
							   nonterms))
				    (cons `(*start* (,(cadr nonterms) ,eoi) : $1)
					  nonterm-defs))))
			  (proc terms
				nonterms
				(map (lambda (x) (cons (caaar x) (map cdar x)))
				     compiled-nonterminals)
				(apply append compiled-nonterminals)))))))))))


(define (rewrite-nonterm-def nonterm-def terms nonterms)

  (define No-NT (length nonterms))

  (define (encode x) 
    (let ((PosInNT (pos-in-list x nonterms)))
      (if PosInNT
	  PosInNT
	  (let ((PosInT (pos-in-list x terms)))
	    (if PosInT
		(+ No-NT PosInT)
		(error "undefined symbol : " x terms))))))

  (if (not (pair? (cdr nonterm-def)))
      (error "At least one production needed for nonterminal" nonterm)
      (let ((name (symbol->string (car nonterm-def))))
	(let loop1 ((lst (cdr nonterm-def))
		    (i 1)
		    (rev-productions-and-actions '()))
	  (if (not (pair? lst))
	      (reverse rev-productions-and-actions)
	      (let* ((rhs (car lst))
		     (rest (cdr lst))
		     (prod (map encode (cons (car nonterm-def) rhs))))
		(for-each (lambda (x)
			    (if (not (or (member x terms) (member x nonterms)))
				(error "Invalid terminal or nonterminal" x)))
			  rhs)
		(if (and (pair? rest)
			 (eq? (car rest) ':)
			 (pair? (cdr rest)))
		    (loop1 (cddr rest)
			   (+ i 1)
			   (cons (cons prod (cadr rest)) 
				 rev-productions-and-actions))
		    (let* ((rhs-length (length rhs))
			   (action
			    (cons 'VECTOR
				 (cons (list 'QUOTE (string->symbol
						     (string-append
						      name
						      "-"
						      (number->string i))))
				       (let loop-j ((j 1))
					 (if (> j rhs-length)
					     '()
					     (cons (string->symbol
						    (string-append
						     "$"
						     (number->string j)))
						   (loop-j (+ j 1)))))))))
		      (loop1 rest
			     (+ i 1)
			     (cons (cons prod action) 
				   rev-productions-and-actions))))))))))

(define (valid-nonterminal? x)
  (symbol? x))

(define (valid-terminal? x)
  (symbol? x))              ; DB 

;;; --------------------------------------------
;;; Utilities

(define (pos-in-list x lst)
  (let loop ((lst lst) (i 0))
    (cond ((not (pair? lst))    #f)
	  ((equal? (car lst) x) i)
	  (else                 (loop (cdr lst) (+ i 1))))))

(define (sunion lst1 lst2)		; union of sorted lists
  (let loop ((L1 lst1)
	     (L2 lst2))
    (cond ((null? L1)    L2)
	  ((null? L2)    L1)
	  (else 
	   (let ((x (car L1)) (y (car L2)))
	     (cond
	      ((> x y)
	       (cons y (loop L1 (cdr L2))))
	      ((< x y)
	       (cons x (loop (cdr L1) L2)))
	      (else
	       (loop (cdr L1) L2))
	      ))))))

(define (sinsert elem lst)
  (let loop ((l1 lst))
    (if (null? l1) 
	(cons elem l1)
	(let ((x (car l1)))
	  (cond ((< elem x)
		 (cons elem l1))
		((> elem x)
		 (cons x (loop (cdr l1))))
		(else 
		 l1))))))

(define (filter p lst)
  (let loop ((l lst))
    (if (null? l)
	'()
	(let ((x (car l)) (y (cdr l)))
	(if (p x)
	    (cons x (loop y))
	    (loop y))))))

;------------------------------------------------------------------------------


