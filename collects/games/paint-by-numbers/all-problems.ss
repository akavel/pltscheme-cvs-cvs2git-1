(module all-problems mzscheme
  (require (lib "etc.ss")
           (lib "list.ss")
           (lib "unitsig.ss")
           (lib "include.ss")
           "problem.ss")

  (require-for-syntax (lib "etc.ss"))

  (define-signature paint-by-numbers:all-problems^ (problemss set-names))
  (define-signature paint-by-numbers:problem-set^ (problems set-name))
  (define-signature paint-by-numbers:problem^ ((struct problem (name rows cols solution))))

  (define-syntax (mk-units stx)
    (syntax-case stx ()
      [(_)
       (with-syntax ([(unit-names ...)
                      (let loop ([files
                                  (call-with-input-file
                                      (build-path (collection-path "games" "paint-by-numbers")
                                                  "problems" "directory")
                                    read)])
                        (cond
                          [(null? files) null]
                          [(or (member (car files) '("CVS" ".svn"))
                               (not (file-exists? (build-path (collection-path "games" "paint-by-numbers")
                                                              "problems" (car files)))))
                           (loop (cdr files))]
                          [else
                           (cons (car files)
                                 (loop (cdr files)))]))])
         (syntax (list (include (build-path "problems" unit-names)) ...)))]))
  
  (define units (mk-units))
  
  (define empty-unit
    (unit/sig paint-by-numbers:all-problems^
      (import [p : paint-by-numbers:problem^])
      
      (define problemss null)
      (define set-names null)))
  
  
  (define (combine-units new-unit sofar)
    (compound-unit/sig 
      (import [p : paint-by-numbers:problem^])
      (link [new : paint-by-numbers:problem-set^ (new-unit p)]
            [old : paint-by-numbers:all-problems^ (sofar p)]
            [combine : paint-by-numbers:all-problems^
                     ((unit/sig paint-by-numbers:all-problems^
                        (import [old : paint-by-numbers:all-problems^]
                                [new : paint-by-numbers:problem-set^]
                                paint-by-numbers:problem^)
                        
                        (define (expand-problem pbm)
                          (make-problem (problem-name pbm)
                                        (problem-rows pbm)
                                        (problem-cols pbm)
                                        (expand-solution (problem-solution pbm))))
  
                        (define problemss 
                          (if (null? new:problems)
                              old:problemss
                              (cons (map expand-problem new:problems) old:problemss)))
                        (define set-names
                          (if (null? new:problems)
                              old:set-names
                              (cons new:set-name old:set-names))))
                      old
                      new
                      p)])
      (export
       (open combine))))
  
  ;; expand-solution : (union #f (listof string[row])) -> 
  ;;                   (union #f (vectorof (vectorof (union 'on 'off 'unknown))))
  (define (expand-solution sol)
    (and sol (apply vector (map expand-row sol))))
  ;; expand-row : string -> (vectorof (union 'on 'off 'unknown))
  (define (expand-row str)
    (list->vector (map expand-char (string->list str))))
  ;; expand-char : char -> (union 'on 'off 'unknown)
  (define (expand-char c)
    (case c
      [(#\x) 'on]
      [(#\space) 'off]
      [(#\U) 'unknown]))

  (provide-signature-elements paint-by-numbers:all-problems^)
  
  (define-values/invoke-unit/sig paint-by-numbers:all-problems^
                                 (foldr combine-units empty-unit units)
                                 #f
                                 paint-by-numbers:problem^))
