#cs
(module World-native-methods mzscheme
  (require (lib "draw.ss" "htdp")
           (lib "posn.ss" "lang")
           (lib "class.ss")
	   (lib "String.ss" "profj" "libs" "java" "lang"))
  ;(require "Posn.ss")
  
  (define-syntax (define/provide stx)
    (syntax-case stx ()
      [(_ id val)
       (identifier? #'id)
       #'(begin
	   (define id val)
	   (provide id))]
      [(_ (id . formals) . rest)
       #'(begin
	   (define (id . formals) . rest)
	   (provide id))]))
  
  (define Posn-x-get (dynamic-require '(lib "Posn.ss" "htdch" "draw") 'Posn-x-get))
  (define Posn-y-get (dynamic-require '(lib "Posn.ss" "htdch" "draw") 'Posn-y-get))
  
  (define (build-posn posnO)
    (make-posn (Posn-x-get posnO) (Posn-y-get posnO)))
  (define (color->symbol colorO)
    (string->symbol (to-lower-case (send colorO my-name))))
  
  (define (to-lower-case s)
    (letrec ((lower 
              (lambda (s)
                (cond
                  ((null? s) s)
                  (else (cons (char-downcase (car s))
                              (lower (cdr s))))))))
      (list->string (lower (string->list s)))))
  
  (define/provide (start-int-int-native this x y)
     (start x y))
  
  (define/provide (stop-native this) (stop))
  
  (define/provide (drawCircle-draw.Posn-int-draw.Color-native this posn r c)
     (draw-circle (build-posn posn) r (color->symbol c)))
  
  (define/provide (drawDisk-draw.Posn-int-draw.Color-native this posn r c)
     (draw-solid-disk (build-posn posn) r (color->symbol c)))
  
  (define/provide (drawRect-draw.Posn-int-int-draw.Color-native this posn w h c)
     (draw-solid-rect (build-posn posn) w h (color->symbol c)))

  (define/provide (drawLine-draw.Posn-draw.Posn-draw.Color-native this p0 p1 c)
     (draw-solid-line (build-posn p0) (build-posn p1) (color->symbol c)))

  (define/provide (drawString-draw.Posn-java.lang.String-native this p s)
     (draw-solid-string (build-posn p) (send s get-mzscheme-string)))

  (define/provide (clearCircle-draw.Posn-int-draw.Color-native this p r c)
     (clear-circle (build-posn p) r (color->symbol c)))

  (define/provide (clearDisk-draw.Posn-int-draw.Color-native this p r c)
     (clear-solid-disk (build-posn p) r (color->symbol c)))

  (define/provide (clearRect-draw.Posn-int-int-draw.Color-native this p w h c)
     (clear-solid-rect (build-posn p) w h (color->symbol c)))

  (define/provide (clearLine-draw.Posn-draw.Posn-draw.Color-native this p0 p1 c)
     (clear-solid-line (build-posn p0) (build-posn p1) (color->symbol c)))

  (define/provide (sleepForAWhile-int-native this s)
    (sleep-for-a-while s))
  
  (define/provide (bigBang-double-native this i)
    (big-bang i this)
    (on-tick-event
      (lambda (world)
	(set! last-world world)
;	(printf ".~n")
	(send world onTick)))
    (on-key-event
      (lambda (ke world)
	(set! last-world world)
;	(printf "key: ~s~n" ke)
	(send world onKeyEvent-java.lang.String
	  (make-java-string (keyevent->string ke)))))
    #t)

  ;; (union Char Symbol) -> String
  (define (keyevent->string ke)
    (if (char? ke) (string ke) (symbol->string ke)))
  
  (define/provide (onTick-native this)
     this)

  (define/provide (onKeyEvent-java.lang.String-native this ke)
     this)

  (define last-world #f)
  
  (define/provide (endOfTime-native this)
    (set! last-world (end-of-time))
    #t)

  (define/provide (endOfWorld-native this)
    (set! last-world (end-of-time))
    last-world)

  (define/provide (lastWorld-native this)
    (if last-world last-world this))
)
