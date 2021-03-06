(module icon mzscheme
  (require (lib "mred.ss" "mred")
	   (lib "class.ss"))

  (provide make-package-colors
           make-package-icons
           make-unknown-package-color
           make-robot-colors
           make-robot-icons
           icon-snip%)
  
  (define transparent-brush (make-object brush% "black" 'transparent))

  (define transparent-pen (make-object pen% "black" 1 'transparent))
  (define black-pen (make-object pen% "black" 1 'solid))
  (define black-brush (make-object brush% "black" 'solid))
  
  (define (adjust c adj)
    (make-object color% (adj (send c red)) (adj (send c green)) (adj (send c blue))))
  
  (define (lighter c)
    (adjust c (lambda (i) (- 255 (floor (* 3/4 (- 255 i)))))))
      
  (define (darker c)
    (adjust c (lambda (i) (floor (* 3/4 i)))))
  
  (define (rel-color rel-weight)
    (make-object color% 
      (inexact->exact (floor (* rel-weight 255)))
      0
      (inexact->exact (floor (* (- 1.0 rel-weight) 255)))))

  (define (mk-rel-weights num-bins)
    (let ([n (sub1 num-bins)])
      (let loop ([i 0])
        (if (= i n)
            '(1)
            (cons (/ i n) (loop (add1 i)))))))
  
  (define (make-package-colors num-bins)
    (map rel-color (mk-rel-weights num-bins)))
  
  (define (make-unknown-package-color) (make-object color% 0 255 0))
  
  (define (mk-package color size)
    (define half-size (add1 (quotient size 2)))
    (define pack-bm (make-object bitmap% half-size half-size))
    (define empty-bm (make-object bitmap% half-size half-size))
    (define pack-mask (make-object bitmap% half-size half-size #t))
    (define dc (make-object bitmap-dc% pack-bm))

    (define top (list (make-object point% (* size 1/4) 0)
                      (make-object point% (* size 1/2) (* size 1/8))
                      (make-object point% (* size 1/4) (* size 1/4))
                      (make-object point% 0 (* size 1/8))))
    (define right (list (make-object point% (* size 1/2) (* size 1/8))
                        (make-object point% (* size 1/4) (* size 1/4))
                        (make-object point% (* size 1/4) (* size 1/2))
                        (make-object point% (* size 1/2) (* size 3/8))))
    (define left (list (make-object point% 0 (* size 1/8))
                       (make-object point% (* size 1/4) (* size 1/4))
                       (make-object point% (* size 1/4) (* size 1/2))
                       (make-object point% 0 (* size 3/8))))
    
    (define dx 0)
    (define dy 0)
    
    (send dc set-pen transparent-pen)
    
    (send dc clear)
    (send dc set-brush (send the-brush-list find-or-create-brush color 'solid))
    (send dc draw-polygon left dx dy)
    (send dc set-brush (send the-brush-list find-or-create-brush (darker color) 'solid))
    (send dc draw-polygon right dx dy)
    (send dc set-brush (send the-brush-list find-or-create-brush (lighter color) 'solid))
    (send dc draw-polygon top dx dy)
    
    (send dc set-bitmap empty-bm)
    (send dc clear)
    (send dc set-brush (send the-brush-list find-or-create-brush color 'solid))
    (send dc draw-polygon left dx dy)
    (send dc set-brush (send the-brush-list find-or-create-brush (darker color) 'solid))
    (send dc draw-polygon right dx dy)
    (send dc set-pen (send the-pen-list find-or-create-pen (lighter color) 1 'solid))
    (send dc set-brush transparent-brush)
    (send dc draw-polygon top dx dy)
    (send dc set-pen transparent-pen)
    
    (send dc set-bitmap pack-mask)
    (send dc clear)
    (send dc set-brush black-brush)
    (send dc draw-polygon top dx dy)
    (send dc draw-polygon left dx dy)
    (send dc draw-polygon right dx dy)
    
    (send dc set-bitmap #f)
    (cons (cons pack-bm pack-mask)
          (cons empty-bm pack-mask)))
  
  (define (make-package-icons size colors)
    (map (lambda (color)
           (mk-package color size))
         colors))

  (define (make-robot-colors n)
    (let loop ([r 255][g 0][b 0][rr 255][gg 255][bb 0][rrr 255][ggg 128][bbb 128][i 0])
      (if (= i n)
          null
          (cons
           (make-object color% r g b)
           (if (zero? (modulo (add1 i) 3))
               (if (zero? (modulo (add1 i) 9))
                   (loop rr gg bb rrr ggg bbb (quotient g 2) g r (add1 i))
                   (loop rr gg bb rrr ggg bbb (quotient g 2) b r (add1 i)))
               (loop g b r rr gg bb rrr ggg bbb (add1 i)))))))

  (define pi (atan 0 -1))
  
  (define (flip-bitmap dc bm mono?)
    (send dc set-bitmap bm)
    (let ([w (send bm get-width)]
          [h (send bm get-height)]
          [c (make-object color%)])
      (define flipped-bm (make-object bitmap% w h mono?))
      (define flipped-dc (make-object bitmap-dc% flipped-bm))
      (send flipped-dc clear)
      (let iloop ([i 0])
        (unless (= i w)
          (let jloop ([j 0])
            (if (= j h)
                (iloop (add1 i))
                (begin
                  (send dc get-pixel i j c)
                  (send flipped-dc set-pixel i (- h j 1) c)
                  (jloop (add1 j)))))))
      (send flipped-dc set-bitmap #f)
      flipped-bm))
  
  (define (mk-robot color size)
    (define half-size (quotient size 2))
    (define 3/4-size (quotient (* 3 size) 4))
    (define robot-bm (make-object bitmap% half-size 3/4-size))
    (define robot-mask (make-object bitmap% half-size 3/4-size #t))
    (define dc (make-object bitmap-dc% robot-bm))
    (define (draw-once color?)
      (send dc draw-ellipse
            (* 1/10 size) (* 1/10 size)
            (* 2/5 size) (* 4/10 size))
      (send dc draw-rectangle
            (* 1/10 size) (* 3/10 size)
            (* 2/5 size) (* 6/10 size))
      (when color?
        (send dc set-brush (send the-brush-list find-or-create-brush (lighter color) 'solid))
        (send dc draw-rectangle
              (* 1/10 size) (* 3/10 size)
              (* 1/10 size) (* 6/10 size))
        (send dc set-brush (send the-brush-list find-or-create-brush (darker color) 'solid))
        (send dc draw-rectangle
              (- half-size (floor (* 1/10 size))) (* 3/10 size)
              (* 1/10 size) (* 6/10 size)))
        
      (send dc set-pen black-pen)
      (send dc draw-line (- half-size (floor (* 2/10 size))) (* 1/5 size) 
            (- half-size (floor (* 1/10 size))) (* 1/10 size))
      (send dc draw-line (* 2/10 size) (* 1/5 size) (* 1/10 size) (* 1/10 size)))
      

    (send dc set-pen transparent-pen)
    
    (send dc clear)
    (send dc set-brush (send the-brush-list find-or-create-brush color 'solid))
    (draw-once #t)
    
    (send dc set-bitmap robot-mask)
    (send dc clear)
    (send dc set-brush black-brush)
    (draw-once #f)
    
    (send dc set-bitmap #f)
    (cons (cons robot-bm robot-mask)
          (cons (flip-bitmap dc robot-bm #f)
                (flip-bitmap dc robot-mask #t))))

  (define (make-robot-icons size colors)
    (map (lambda (color)
           (mk-robot color size))
         colors))
  
  
  (define icon-snip%
    (class snip%
      (inherit get-admin set-count get-style)
      (define w 0)
      (define h 0)
      (define text-h 0)
      (init-field bm)
      (init-field label)
      (public*
        [set-label (lambda (b lbl)
                     (unless (and (eq? bm b) (string=? label lbl))
                       (set! bm b)
                       (set! label lbl)
                       (set! text-h #f)
                       (send (get-admin) resized this #t)))])
      (override*
        [size-cache-invalid (lambda () (set! text-h #f))]
	[get-extent  ; called by an editor to get the snip's size
	 (lambda (dc x y wbox hbox descentbox spacebox lspacebox rspacebox)
           (unless text-h
             (let-values ([(tw th d a) (send dc get-text-extent label (send (get-style) get-font))])
               (set! text-h th)
               (set! h (max text-h (send bm get-height)))
               (set! w (+ tw (send bm get-width) 1))))
	   (when hbox
	     (set-box! hbox h))
	   (when wbox
	     (set-box! wbox w))
	   (when descentbox
	     (set-box! descentbox 0))
	   (when spacebox
	     (set-box! spacebox 0))
	   (when rspacebox
	     (set-box! rspacebox 0))
	   (when lspacebox
	     (set-box! lspacebox 0)))]
	[draw  ; called by an editor to draw the snip
	 (lambda (dc x y . other)
           (when bm
             (send dc draw-bitmap bm x (+ y (/ (- h (send bm get-height)) 2)))
             (send dc draw-text
                   label
                   (+ x (send bm get-width) 1)
                   (+ y (/ (- h (or text-h 0)) 2)))))]
	[write  ; marshals the snip to a text stream
	 (lambda (stream)
	   (send stream << w)
	   (send stream << h))]
	[resize  ; called by a pasetboard editor to resize the snip
	 (lambda (w-in h-in)
	   (set! w w-in)
	   (set! h h-in)
	   ;; send resize notification to the editor containing the snip
	   (let ([admin (get-admin)])
	     (when admin
	       (send admin resized this #t)))
	   #t)])
      (super-instantiate ())
      (set-count 1))))
