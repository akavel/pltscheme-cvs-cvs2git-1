(define-lex-abbrev)
(define-lex-abbrev a)
(define-lex-abbrev (a b) v)
(define-lex-abbrev 1 1)
(define-lex-abbrevs ())

(define-lex-trans)
(define-lex-trans (1 b) 1)

(lexer)
(lexer ("a" "b" "c"))
(lexer ())
(lexer (""))

(lexer (a 1))
(lexer ((a) 1))
(let ((a 1))
  (lexer ((a) 1)))
(let-syntax ((a 1))
  (lexer ((a) 1)))
(let ()
  (define-lex-trans a 1)
  (let ()
    (lexer ((a) 1))))

(lexer (1 1))
(lexer ((repetition) 1))
(lexer ((repetition #\1 #\1 "3") 1))
(lexer ((repetition 1 #\1 "3") 1))
(lexer ((repetition 1 0 "3") 1))
(lexer ((complement) 1))
(lexer ((char-range) 1))
(lexer ((char-range #\9 #\0) 1))
(lexer ((char-complement) 1))
(lexer ((char-complement (concatenation "1" "2")) 1))