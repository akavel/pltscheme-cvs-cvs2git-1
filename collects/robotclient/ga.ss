(module ga mzscheme
  (require (lib "list.ss" "mzlib")
	   (lib "process.ss" "mzlib")
	   "heuristics.ss"
	   "client.ss")
	  
  (define GREATEST-POSSIBLE-INTEGER 10000)
  (define LOWEST-POSSIBLE-INTEGER -10000)

  (define capacity-amount 10000)
  (define print-every-n-generations 1)
  (define run-n-generations 1000)
  (define output-port (current-output-port))
  (define boards '((3 "board1" "packages1" 100)
                   (3 "board2" "packages1" 100)
                   ;(5 "board3" "packages2")
                   (4 "board4" "packages3" 1000)
                   (4 "board5" "packages4" 300)
		   (6 "board6" "packages5" 4000)))
  (define initial-set
    `((-10 -100 -20 -200 -1000 -500 100 2000 200 10 500 10 1 -10000 -10000 
	   100 -10 75 50 25 1000 750 500 250 5 200 2000 0.6 1 0.7 0.1 0.75 0.2 
	   1 -1)
      (10 10 10 100 -1000 50 10 2000 10 50 2000 50 1 -10000 -10000 100 -1 
	  50 25 10 900 500 250 100 1 150 1800 0.8 1 0.9 0.5 1 0.6 3 -0.5)
      (-100 -1000 -500 -500 -3000 -1000 500 5000 200 -10 0 -10 1 -10000 
	    -10000 200 -50 150 100 50 1500 1000 500 300 8 250 2500 1 1 1 0 1 1 
	    5 -0.75)))
  
  (define parameter-list
    (list wall-threat-value 
          water-threat-value
          blank-threat-value
          wall-danger-value
          water-danger-value
          blank-danger-value
          wall-escape-value
          water-escape-value
          blank-escape-value
          wall-push-value
          water-push-value
          blank-push-value
          blank-value
          wall-value
          water-value
          home-value
          next-water-value
          one-home-value
          two-home-value
          three-home-value
          destination-value
          one-destination-value
          two-destination-value
          three-destination-value
          dvw-value
          pickup-value
          dropoff-value
          wall-escape-bid
          water-escape-bid
          blank-escape-bid
          wall-push-bid
          water-push-bid
          blank-push-bid
          max-bid-const
          step-weight))
  
  ;; set-parameter-values! : gene-seq -> void
  (define (set-parameter-values! sequence)
    (for-each (lambda (param value)
                (param value))
              parameter-list sequence))
  
  ;; mutation-factor : -> number
  ;; Randomly determines how much to multiply the random mutation
  ;; value by for a particular gene
  (define (mutation-factor)
    (let ([num (random 100)])
      (cond
        [(< num 75) 0]
        [(< num 85) 1]
        [(< num 95) 10]
        [else 100])))

  ;; roundify : number -> number
  (define (roundify num)
    (exact->inexact (/ (round (* num 10000)) 10000)))
  
  ;; breed : gene-seq gene-seq -> gene-seq
  (define (breed gs1 gs2)
    (let ([base (map (lambda (x y) (roundify (/ (+ x y) 2))) gs1 gs2)])
      (map (lambda (x)
             (let* ([mutation (* (- (random 21) 10) (mutation-factor))]
                    [newval (+ x mutation)])
               (if (or (>= newval GREATEST-POSSIBLE-INTEGER)
                       (<= newval LOWEST-POSSIBLE-INTEGER))
                   x newval)))
           base)))
  
  ;; n-copies : T -> list-of-T
  (define (n-copies n x)
    (cond
      [(= 0 n) '()]
      [else (cons x (n-copies (sub1 n) x))]))
  
  ;; spawn-next-generation : list-of-gene-seqs -> list-of-gene-seqs
  (define (spawn-next-generation bestls)
    (let ([base-set (apply append (map (lambda (x) (n-copies 2 x)) bestls))])
      (let loop ((ls base-set) (acc '()))
        (cond
          [(null? (cdr ls)) (append bestls acc)]
          [else (loop (cdr ls)
                      (append (map (lambda (x) (breed (car x) (cdr x)))
                                   (map (lambda (x) (cons (car ls) x)) 
					(cdr ls)))
                              acc))]))))

  ;; startup-player : gene-seq -> number
  (define (startup-player gene-seq)
    (set-parameter-values! gene-seq)
    (start-client #f #f "localhost" 4000))
  
  ;; play-board : vector(num) string string num vector(gene-seq) num -> void
  (define (play-board scoreboard board packages fuel player-vec players)
    (let ([board-file (string-append "boards/" board)]
          [packages-file (string-append "boards/" packages)])
      (sleep 3)
      (apply (lambda (stdout stdin procid stderr utility)
	       (sleep 3)
	       (let loop ((ls players) (threads '()))
		 (cond
		  [(null? ls) (for-each thread-wait threads)
		              (utility 'kill)
			      (close-input-port stderr)
		              (close-input-port stdout)
			      (close-output-port stdin)]
		  [else (let ([genes (vector-ref player-vec (car ls))])
			  (loop (cdr ls)
				(cons (thread
				       (lambda ()
					 (let ([score (startup-player genes)])
					   (printf "Adding score of ~a to ~a~n"
						   score (car ls))
					   (vector-set! scoreboard (car ls)
					     (+ (vector-ref scoreboard (car ls))
						score)))))
				      threads)))])))
	     (process (format "/home/cs/grad/wick/plt/collects/robotclient/Simulator -p 4000 -n ~a -m ~a -k ~a -f ~a -c ~a~n"
			      (length players) board-file packages-file
			      fuel capacity-amount)))))

  ;; generate-results : list-of-gene-seqs -> list-of-gene-seqs
  (define (generate-results ls)
    (let ([scoreboard (make-vector (length ls) 0)]
          [players (list->vector ls)]
          [num-players (length ls)])
      (let board-loop ((board-ls boards))
        (cond
          [(null? board-ls) (map cons (vector->list scoreboard)
                                 (vector->list players))]
          [else (let tourny-loop ([i 0])
                  (cond
                    [(= i num-players) (board-loop (cdr board-ls))]
                    [else (let ([need-players (caar board-ls)]
                                [board-file (cadar board-ls)]
                                [packages-file (caddar board-ls)]
				[fuel (car (cdddar board-ls))])
                            (let other-players ([player i] [acc '()])
                              (cond
                                [(= (length acc) need-players)
                                 (play-board scoreboard board-file
                                             packages-file fuel players acc)
                                 (tourny-loop (add1 i))]
                                [else (other-players
                                       (modulo (add1 player) num-players)
                                       (cons player acc))])))]))]))))
  
  ;; chop-list! : num list -> void
  (define (chop-list! n ls)
    (let loop ((workls ls) (n (sub1 n)))
      (cond
        ((= n 0) (set-cdr! workls '()))
        (else (loop (cdr workls) (sub1 n))))))
  
  ;; run-generation : list-of-gene-seqs -> list-of-gene-seqs
  (define (run-generation ls print)
    (let* ([results (generate-results ls)]
           [results (mergesort results (lambda (x y) (> (car x) (car y))))])
      (chop-list! 3 results)
      (when (output-port? print)
        (fprintf print "(Score ~a) ~a~n" (caar results) (cdar results))
        (fprintf print "(Score ~a) ~a~n" (caadr results) (cdadr results))
        (fprintf print "(Score ~a) ~a~n" (caaddr results) (cdaddr results))
        (flush-output print))
      (spawn-next-generation (map cdr results))))
  
  ;; run-genetic-algorithm : list-of-gene-seqs output-port num num -> void
  (define (run-genetic-algorithm initial-set output-port modnum endat)
    (let loop ((i 0) (group (spawn-next-generation initial-set)))
      (cond
        [(= i endat)
         (fprintf output-port "--------------FINAL---------------------~n")
         (run-generation group output-port)]
        [(= (modulo i modnum) 0)
         (fprintf output-port "----------Generation ~a------------------~n" i)
         (loop (add1 i) (run-generation group output-port))]
        [else (loop (add1 i) (run-generation group #f))]))
    (void))
  
  (run-genetic-algorithm initial-set output-port print-every-n-generations
			 run-n-generations)
  
)

(require ga)