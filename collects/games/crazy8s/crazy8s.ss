
(module crazy8s mzscheme
  (require (lib "cards.ss" "games" "cards")
	   (lib "mred.ss" "mred")
	   (lib "class.ss")
           (lib "unit.ss")
           (lib "etc.ss")
	   (lib "list.ss")
           (lib "async-channel.ss"))

  ;; Player record
  (define-struct player (r hand-r  ; region
                         hand))    ; cards
      
  ;; Initial card count
  (define opponents-count 2)
  (define init-hand-size 7)
      
  ;; Messages
  (define YOUR-NAME "You")
  (define PLAYER-X-NAME "Opponent ~a")
  (define YOUR-TURN-MESSAGE "Your turn.  (Discard an ~a or ~a, or else ~a.)")
  (define GAME-OVER-YOU-WIN "Game over - you win!")
  (define GAME-OVER "Game over - opponent wins")
  
  ;; Region layout constants
  (define MARGIN 10)
  (define SUBMARGIN 10)
  (define LABEL-H 15)
  (define PASS-W 40)
      
  (provide game-unit)
  
  (define game-unit
    (unit
      (import)
      (export)
  
  ;; Randomize
      (random-seed (modulo (current-milliseconds) 10000))
      
      ;; Set up the table
      (define t (make-table "Crazy 8s" 8 5.5))
      (send t create-status-line)
      (send t show #t)
      (send t set-double-click-action #f)
      (send t set-button-action 'left 'drag-raise/one)
      (send t set-button-action 'right 'drag/above)
      
      ;; Get table width & height
      (define w (send t table-width))
      (define h (send t table-height))
      
      ;; Set up the cards
      (define deck (shuffle-list (make-deck) 7))
      (for-each
       (lambda (card)
	 (send card user-can-flip #f))
       deck)

      ;; We'll need an 8 of each suit for substitutions later:
      (define (find-8 suit)
	(ormap (lambda (c)
		 (and (= 8 (send c get-value))
		      (eq? suit (send c get-suit))
		      (send c copy)))
	       deck))
      (define 8-hearts (find-8 'hearts))
      (define 8-spades (find-8 'spades))
      (define 8-clubs (find-8 'clubs))
      (define 8-diamonds (find-8 'diamonds))
      
      ;; Function for dealing or drawing cards
      (define (deal n)
	(let loop ([n n][d deck])
	  (if (zero? n)
	      (begin
		(set! deck d)
		null)
	      (cons (car d) (loop (sub1 n) (cdr d))))))
      
      ;; Card width & height
      (define cw (send (car deck) card-width))
      (define ch (send (car deck) card-height))
      
      (define deck-region
	(make-region (- (/ (- w cw) 2) (/ (+ cw MARGIN) 2))
		     (/ (- h ch) 2)
		     cw ch
		     #f #f))
      (define discard-region
	(make-region (+ (region-x deck-region) cw MARGIN)
		     (region-y deck-region)
		     cw ch
		     #f #f))

      (define discard-target-region
	(make-region (- (region-x discard-region) (/ MARGIN 2))
		     (- (region-y discard-region) (/ MARGIN 2))
		     (+ cw MARGIN) (+ ch MARGIN)
		     "" #f))
      
      (define SEL-WIDTH 32)
      (define SEL-HEIGHT 32)
      
      (define (make-suit-region x y label card)
	(let ([bm (make-object bitmap%
			       (build-path (collection-path "games" "crazy8s")
					   "images"
					   (format "~a.png" label))
			       'unknown/mask)])
	  (make-button-region x y
			      SEL-WIDTH SEL-HEIGHT
			      bm
			      (lambda ()
				(async-channel-put msg card)))))
      
      (define hearts-region
	(make-suit-region (+ (region-x discard-target-region) cw (* 2 MARGIN))
			  (+ (region-y discard-target-region)
			     (/ (- ch (* 2 SEL-HEIGHT) (/ MARGIN 2)) 2))
			  "heart" 8-hearts))
      (define spades-region
	(make-suit-region (+ (region-x hearts-region) SEL-WIDTH (/ MARGIN 2))
			  (region-y hearts-region)
			  "spade" 8-spades))
      (define clubs-region
	(make-suit-region (region-x hearts-region)
			  (+ (region-y hearts-region) SEL-HEIGHT (/ MARGIN 2))
			  "club" 8-clubs))
      (define diamonds-region
	(make-suit-region (region-x spades-region)
			  (region-y clubs-region)
			  "diamond" 8-diamonds))
      
      (send t add-region discard-target-region)
      
      (define pass-button
	(make-button-region (+ (region-x deck-region)
			       (/ (- cw PASS-W) 2))
			    (+ (region-y deck-region)
			       (/ (- ch (* 2 LABEL-H)) 2))
			    PASS-W (+ LABEL-H 4)
			    "Pass" (lambda ()
				     (async-channel-put msg 'pass))))

      ;; Put the cards on the table
      (send t add-cards-to-region deck deck-region)
      
      ;; Player region size
      (define pw (- (/ w opponents-count) (* opponents-count MARGIN)))
      (define ph (- (/ (- h ch) 2) (* 2 MARGIN)))
      
      ;; Define the players with their regions
      (define (make-a-player x y w h lbl)
	(let ([r (make-region x y w h lbl #f)])
	  (send t add-region r)
	  (make-player
	   r
	   (make-region (+ x SUBMARGIN) (+ y SUBMARGIN LABEL-H)
			(- w (* 2 SUBMARGIN))
			(- h (* 2 SUBMARGIN) LABEL-H)
			#f #f)
	   null)))
      
      (define players
	(cons
	 ;; You
	 (make-a-player
	  (/ MARGIN 2) (- h ph (/ MARGIN 2))
	  (- w MARGIN) ph
	  YOUR-NAME)
	 (build-list
	  opponents-count
	  (lambda (delta)
	    (make-a-player
	     (+ (* (+ pw MARGIN) delta) (/ MARGIN 2)) (/ MARGIN 2)
	     pw ph
	     (format PLAYER-X-NAME (+ 1 delta)))))))
      (define you (car players))
      (define opponents (cdr players))

      (send t add-region
	    (make-button-region (region-x (player-r you))
				(- (region-y (player-r you))
				   (+ LABEL-H 4 MARGIN))
				PASS-W (+ LABEL-H 4)
				"Clean" (lambda ()
					  (send t stack-cards (player-hand you))
					  (send t move-cards-to-region
						(player-hand you)
						(player-hand-r you)))))
      
      ;; Card setup: Deal the cards
      (for-each (lambda (player)
		  (set-player-hand! player (deal init-hand-size))
		  (send t move-cards-to-region 
			(player-hand player)
			(player-hand-r player)))
		players)

      ;; More card setup: Opponents's cards and deck initally can't be moved
      (for-each
       (lambda (card) (send card user-can-move #f))
       (append
	(apply append
	       (map player-hand opponents))
	deck))
      (for-each (lambda (c) (send c home-region (player-r you)))
		(player-hand you))
      
      ;; More card setup: Initial discard
      ;;  If it's an eight, then shuffle and try again
      (let loop ()
	(when (= 8 (send (car deck) get-value))
	  (set! deck (shuffle-list deck 1))
	  (send t stack-cards deck)
	  (loop)))
      (define discards (deal 1))
      (send t flip-cards discards)
      (send t move-cards-to-region discards discard-region)
      
      ;; More card setup: Show your cards
      (send t flip-cards (player-hand you))

      ;; Function to enable/disable moving your cards
      (define (enable-your-cards on?)
	(for-each (lambda (c) (send c user-can-move on?))
		  (player-hand you)))
      
      ;; Callbacks communicate back to the main loop via these
      (define msg (make-async-channel))
      
      (define (get-discard-card cs)
	(and (= 1 (length cs))
	     (let ([c (car cs)])
	       (and (memq c (player-hand you))
		    (or (= (send (car discards) get-value)
			   (send c get-value))
			(= (send (car discards) get-suit-id)
			   (send c get-suit-id))
			(= (send c get-value) 8))
		    c))))
      
      (define (pick-to-discard cards)
	(let ([non-8s (filter (lambda (c)
				(not (= 8 (send c get-value))))
			      cards)])
	  (if (null? non-8s)
	      (car cards)
	      (car non-8s))))
      
      (define (play-opponent p)
	(let ([suit-id (send (car discards) get-suit-id)]
	      [value (send (car discards) get-value)])
	  (let ([matches (filter (lambda (c)
				   (or (= suit-id (send c get-suit-id))
				       (= value (send c get-value))
				       (= 8 (send c get-value))))
				 (player-hand p))])
	    (if (null? matches)
		;; Draw or pass
		(if (pair? deck)
		    ;; Draw
		    (begin
		      (send t card-to-front (car deck))
		      (set-player-hand! p (append (deal 1) (player-hand p)))
		      (send t move-cards-to-region (player-hand p) (player-hand-r p))
		      (play-opponent p))
		    ;; Pass
		    (begin
		      (send t hilite-region (player-r p))
		      (send t pause 0.25)
		      (send t unhilite-region (player-r p))
		      #t))
		;; Discard
		(let ([c (pick-to-discard matches)])
		  (set-player-hand! p (remq c (player-hand p)))
		  (send t flip-card c)
		  (send t card-to-front c)
		  (send t move-cards-to-region (list c) discard-region)
		  (send t move-cards-to-region (player-hand p) (player-hand-r p))
		  (set! discards (cons c discards))
		  ;; Did we just discard an 8?
		  (when (= 8 (send (car discards) get-value))
		    (let ([counts (map (lambda (v)
					 (cons v
					       (length (filter (lambda (c)
								 (= v (send c get-value)))
							       (player-hand p)))))
				       '(1 2 3 4))])
		      (reset-8
		       (list-ref
			(list 8-clubs 8-diamonds 8-hearts 8-spades)
			(sub1 (caar (quicksort counts (lambda (a b) (> (cdr a) (cdr b))))))))))
		  ;; Return #f if this player has just won:
		  (pair? (player-hand p)))))))            
      
      (define (allow-cards on?)
	(when (pair? deck)
	  (send (car deck) user-can-move on?))
	(for-each (lambda (c)
		    (send c user-can-move on?))
		  (player-hand you))
	(when (null? deck)
	  (if on?
	      (send t add-region pass-button)
	      (send t remove-region pass-button))))
      
      (define (reset-8 got-8)
	(unless (eq? (send (car discards) get-suit)
		     (send got-8 get-suit))
	  (let ([c (send got-8 copy)])
	    (send c user-can-move #f)
	    (send t flip-card (car discards))
	    (send t add-cards-to-region (list c) discard-region)
	    (send t card-to-front c)
	    (send t remove-card (car discards))
	    (set! discards (cons c (cdr discards)))
	    (send t flip-card c))))
      
      (define (pick-suit)
	(allow-cards #f)
	(send t add-region hearts-region)
	(send t add-region spades-region)
	(send t add-region clubs-region)
	(send t add-region diamonds-region)
	(let ([got-8 (yield msg)])
	  (reset-8 got-8))
	(send t remove-region hearts-region)
	(send t remove-region spades-region)
	(send t remove-region clubs-region)
	(send t remove-region diamonds-region)
	(allow-cards #t))

      ;; Run the game loop
      (let loop ()
	(when (pair? deck)
	  (send (car deck) user-can-move #t)
	  (send (car deck) home-region deck-region))
	(when (null? deck)
	  (send t add-region pass-button))
	(set-region-interactive-callback! 
	 discard-target-region
	 (lambda (in? cs)
	   (let ([c (get-discard-card cs)])
	     (when c
	       (send c home-region (if in? #f (player-r you)))))))
	(set-region-callback! 
	 discard-target-region
	 (lambda (cs)
	   (let ([c (get-discard-card cs)])
	     (when c
	       (send c home-region #f)
	       (set! discards (cons c discards))
	       (set-player-hand! you (remq c (player-hand you)))
	       (send t card-to-front c)
	       (send t move-cards-to-region (list c) discard-region)
	       (send c user-can-move #f)
	       (async-channel-put msg 'discard)))))
	(set-region-interactive-callback! 
	 (player-r you)
	 (lambda (in? cs)
	   (send (car cs) home-region 
		 (if in? (player-r you) deck-region))))
	(set-region-callback! 
	 (player-r you)
	 (lambda (cs)
	   (let ([c (car cs)])
	     (send t flip-card c)
	     (send c home-region (player-r you))
	     (set-player-hand! you (cons c (player-hand you)))
	     (deal 1)
	     (async-channel-put msg 'draw))))
	(send t set-status-text (format YOUR-TURN-MESSAGE
					(let ([v (send (car discards) get-value)])
					  (if (= v 8)
					      "8"
					      (format "8, ~a," (case v
								 [(1) "ace"]
								 [(11) "jack"]
								 [(12) "queen"]
								 [(13) "king"]
								 [else v]))))
					(case (send (car discards) get-suit)
					  [(hearts) "heart"]
					  [(spades) "spade"]
					  [(diamonds) "diamond"]
					  [(clubs) "club"])
					(if (null? deck)
					    "pass"
					    "draw")))
	(let ([what (yield msg)])
	  ;; Played a crazy 8?
	  (when (and (eq? what 'discard)
		     (= 8 (send (car discards) get-value)))
	    (pick-suit))
	  ;; Done... but what did we do?
	  (case what
	    [(draw)
	     ;; Go again
	     (loop)]
	    [(discard pass)
	     ;; Hide pass button...
	     (when (null? deck)
	       (send t remove-region pass-button))
	     ;; ... and run opponents
	     (send t set-status-text "Opponent's turn...")
	     (unless (null? (player-hand you))
	       (let oloop ([l opponents])
		 (cond
		  [(null? l) (loop)]
		  [else (when (play-opponent (car l))
			  (oloop (cdr l)))])))])))
      
      (allow-cards #f)

      (send t set-status-text (if (null? (player-hand you))
				  GAME-OVER-YOU-WIN
				  GAME-OVER)))))
