(module gui mzscheme
  (require
   (lib "etc.ss")
   (lib "list.ss")
   (lib "class.ss")
   (all-except (lib "mred.ss" "mred") send-event)
   (all-except "frp.ss"
               #%app
               if
               lambda
               case-lambda
               define
               letrec
	       and
	       or
	       cond
	       rec)
   (rename "frp.ss" frp:define define))
  
  (define reactive-control<%>
    (interface (control<%>)
      get-event))
  
  (define (insert-in-list list position new-element)
    (let loop ((list list) (position position))
      (cond
        ((zero? position)
         (cons new-element list))
        ((null? list)
         '())
        (else
         (cons (car list) (loop (cdr list) (- position 1)))))))
  
  (define (reactive-control-mixin super)
    (class* super (reactive-control<%>)
;    (mixin (control<%>) (reactive-control<%>)
      (define event (event-receiver))
      (define/public (get-event) event)
      (define/public (extract-value)
        (error 'extract-value "abstract method"))
      (super-instantiate ()
        (callback
         (lambda (self control-event)
           (send-event event
                       (list self
                             control-event
                             (extract-value))))))))
  
  (define reactive-slider%
    (class (reactive-control-mixin slider%)
      (inherit get-value)
      (define/override (extract-value)
        (get-value))
      (super-instantiate ())))
  
  (define (make-control-behavior reactive-control)
    (hold (send reactive-control extract-value)
          ((send reactive-control get-event) . ==> . third)))
  
  ;(define (get-button-event reactive-button)
  ;  ((send reactive-button get-event) . -=> . #t))
  
  (define reactive-slider-1d% reactive-slider%)
  
  (define make-slider-1d-behavior make-control-behavior)
  
;   (define reactive-slider-2d%
;     (class (reactive-control-mixin slider-2d%)
;       (inherit get-values)
;       (define/override (extract-value)
;         (call-with-values (lambda () (get-values))
;                           cons))
;       (super-instantiate ())))
;   
;   (define (make-slider-2d-behavior reactive-slider)
;     (hold (call-with-values
;            (lambda () (send reactive-slider get-values))
;            cons)
;           ((send reactive-slider get-event) . ==> . third)))
;   
; 
 
  
  (define reactive-button%
    (class (reactive-control-mixin button%)
      (define/override (extract-value)
        'button-press)
      (super-instantiate ())))
  
  (define reactive-check-box%
    (class (reactive-control-mixin check-box%)
      (inherit get-value)
      (define/override (extract-value)
        (get-value))
      (super-instantiate ())))
  
  (define reactive-list-box%
    (class (reactive-control-mixin list-box%)
      (inherit get-selections)
      (define/override (extract-value)
        (get-selections))
      (super-instantiate ())))
  
  (define reactive-choice%
    (class (reactive-control-mixin choice%)
      (inherit get-selection)
      (define/override (extract-value)
        (get-selection))
      (super-instantiate ())))
  
  (define reactive-text-field%
    (class (reactive-control-mixin text-field%)
      (inherit get-value)
      (define/override (extract-value)
        (get-value))
      (super-instantiate ())))
  
  (define view<%>
    (interface ()
      set-value))
  
  (define reactive-view<%>
    (interface ()
      set-behavior))
  
  (define (reactive-view-mixin super)
    (class* super (reactive-view<%>)
;    (mixin (view<%>) (reactive-view<%>)
      (init-field
       (cell (new-cell undefined))
       (updater (proc->behavior
		 (lambda () (send this set-value (get-value cell)))
		 undefined
                 cell)))
      
      (super-instantiate ())
      
      (define/public (set-behavior beh)
        (set-cell! cell beh))))
  
;   (define (drive-view-thread view event)
;     (let ((view-box (make-weak-box view)))
;       (set! view (void))
;       (dynamic-disable-break
;        (lambda ()
;          (thread
;           (lambda ()
;             (let/ec k
;               (with-handlers ((exn:break? k))
;                 (run-callback event
;                               (lambda (value)
;                                 (cond
;                                   ((weak-box-value view-box)
;                                    => (lambda (view)
;                                         (send view set-value value)))
;                                   (else (k 'fick-dich-ins-knie)))))))))))))
; 

  (define reactive-gauge%
    (reactive-view-mixin
     (class* gauge% (view<%>)
       (super-instantiate ()))))
  
  (define reactive-message%
    (reactive-view-mixin
     (class* message% (view<%>)
       (inherit set-label)
       (define/public (set-value label)
         (set-label label))
       (super-instantiate ()))))
  
  (define frame (instantiate frame% () (label "GUI") (height 150) (width 200)))
  (send frame show #t)
  
  (define-struct gui-item (builder))
  
  (define (create-gui-item builder)
    (let ([C false])
      (make-gui-item
       (lambda (p)
         (cond
           [(and p C)
            (error 'create-window "item added to window twice")]
           [(and p (not C)) (set! C (builder p)) C]
           [(and (not p) C) C]
           [(and (not p) (not C))
            (error 'gui "gui-items must be added to window before use (see create-window)")])))))
  
  (define (make-button str)
    (send (instantiate reactive-button% () (label str) (parent frame)) get-event))
  
  (frp:define (make-message str~)
    (send (instantiate reactive-message% ()
            (label (if (undefined? (get-value str~))
                       ""
                       (get-value str~)))
            (parent frame)
            (stretchable-height #t)
            (stretchable-width #t))
          set-behavior
          str~))
  
  (frp:define (make-gauge rng val~)
    (send (instantiate reactive-gauge% ()
            (label "")
            (range rng)
            (parent frame)
            (stretchable-width #t))
          set-behavior
          val~))
  
  (define (make-text str)
    (make-control-behavior (instantiate reactive-text-field% () (label str) (init-value "") (parent frame))))
  
  (define (make-choice str los)
    (make-control-behavior (instantiate reactive-choice% () (choices los) (parent frame) (label str))))

  (define (make-slider str min max init)
    (make-control-behavior (instantiate reactive-slider-1d% () (label str)
                             (min-value min) (max-value max) (parent frame)
                             ;(style (list 'plain 'horizontal))
                             )))
  
  (define fresh-window
    (let ([first #t])
      (lambda ()
        (if first
            (set! first #f)
            (begin
              (set! frame (instantiate frame% () (label "GUI") (height 150) (width 200)))
              (send frame show #t))))))
  
  (provide (all-defined)))
