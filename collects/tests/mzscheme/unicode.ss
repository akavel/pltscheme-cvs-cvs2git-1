
(load-relative "loadtest.ss")

(SECTION 'unicode)

;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;  UTF-8 boundary tests based on Markus Kuhn's test suite

(define basic-utf-8-tests
  '((#(#x3ba #x1f79 #x3c3 #x3bc #x3b5) complete
     (#o316 #o272 #o341 #o275 #o271 #o317 #o203 #o316 #o274 #o316 #o265))
    (#(0) complete
     (#o0))
    (#(#x00000080) complete
     (#o302 #o200))
    (#(#x00000800) complete
     (#o340 #o240 #o200))
    (#(#x00010000) complete
     (#o360 #o220 #o200 #o200))
    (#(#x00200000) complete
     (#o370 #o210 #o200 #o200 #o200))
    (#(#x04000000) complete
     (#o374 #o204 #o200 #o200 #o200 #o200))
    (#(#x0000007F) complete
     (127))
    (#(#x000007FF) complete
     (#o337 #o277))
    (#(#x001FFFFF) complete
     (#o367 #o277 #o277 #o277))
    (#(#x03FFFFFF) complete
     (#o373 #o277 #o277 #o277 #o277))
    (#(#x7FFFFFFF) complete
     (#o375 #o277 #o277 #o277 #o277 #o277))
    (#(#x0000D7FF) complete
     (#o355 #o237 #o277))
    (#(#x0000E000) complete
     (#o356 #o200 #o200))
    (#(#x0000FFFD) complete
     (#o357 #o277 #o275))
    (#(#x0010FFFF) complete
     (#o364 #o217 #o277 #o277))
    (#(#x00110000) complete
     (#o364 #o220 #o200 #o200))
    ;; Missing start byte
    (#() error
     (#o200))
    (#() error
     (#o277))
    (#() error
     (#o200 #o277))
    (#() error
     (#o200 #o277 #o200))
    (#() error
     (#o200 #o277 #o200 #o277))
    (#() error
     (#o200 #o277 #o200 #o277 #o200))
    (#() error
     (#o200 #o277 #o200 #o277 #o200 #o277))
    (#() error
     (#o200 #o277 #o200 #o277 #o200 #o277 #o200))
    (#() error
     (#o200))
    (#() error
     (#o201))
    (#() error
     (#o202))
    (#() error
     (#o203))
    (#() error
     (#o204))
    (#() error
     (#o205))
    (#() error
     (#o206))
    (#() error
     (#o207))
    (#() error
     (#o210))
    (#() error
     (#o211))
    (#() error
     (#o212))
    (#() error
     (#o213))
    (#() error
     (#o214))
    (#() error
     (#o215))
    (#() error
     (#o216))
    (#() error
     (#o217))
    (#() error
     (#o220))
    (#() error
     (#o221))
    (#() error
     (#o222))
    (#() error
     (#o223))
    (#() error
     (#o224))
    (#() error
     (#o225))
    (#() error
     (#o226))
    (#() error
     (#o227))
    (#() error
     (#o230))
    (#() error
     (#o231))
    (#() error
     (#o232))
    (#() error
     (#o233))
    (#() error
     (#o234))
    (#() error
     (#o235))
    (#() error
     (#o236))
    (#() error
     (#o237))
    (#() error
     (#o240))
    (#() error
     (#o241))
    (#() error
     (#o242))
    (#() error
     (#o243))
    (#() error
     (#o244))
    (#() error
     (#o245))
    (#() error
     (#o246))
    (#() error
     (#o247))
    (#() error
     (#o250))
    (#() error
     (#o251))
    (#() error
     (#o252))
    (#() error
     (#o253))
    (#() error
     (#o254))
    (#() error
     (#o255))
    (#() error
     (#o256))
    (#() error
     (#o257))
    (#() error
     (#o260))
    (#() error
     (#o261))
    (#() error
     (#o262))
    (#() error
     (#o263))
    (#() error
     (#o264))
    (#() error
     (#o265))
    (#() error
     (#o266))
    (#() error
     (#o267))
    (#() error
     (#o270))
    (#() error
     (#o271))
    (#() error
     (#o272))
    (#() error
     (#o273))
    (#() error
     (#o274))
    (#() error
     (#o275))
    (#() error
     (#o276))
    (#() error
     (#o277))
    ;; 2-byte seqs with no continuation
    (#(#f 32) error
     (#o300 #o40))
    (#(#f 32) error
     (#o301 #o40))
    (#(#f 32) error
     (#o302 #o40))
    (#(#f 32) error
     (#o303 #o40))
    (#(#f 32) error
     (#o304 #o40))
    (#(#f 32) error
     (#o305 #o40))
    (#(#f 32) error
     (#o306 #o40))
    (#(#f 32) error
     (#o307 #o40))
    (#(#f 32) error
     (#o310 #o40))
    (#(#f 32) error
     (#o311 #o40))
    (#(#f 32) error
     (#o312 #o40))
    (#(#f 32) error
     (#o313 #o40))
    (#(#f 32) error
     (#o314 #o40))
    (#(#f 32) error
     (#o315 #o40))
    (#(#f 32) error
     (#o316 #o40))
    (#(#f 32) error
     (#o317 #o40))
    (#(#f 32) error
     (#o320 #o40))
    (#(#f 32) error
     (#o321 #o40))
    (#(#f 32) error
     (#o322 #o40))
    (#(#f 32) error
     (#o323 #o40))
    (#(#f 32) error
     (#o324 #o40))
    (#(#f 32) error
     (#o325 #o40))
    (#(#f 32) error
     (#o326 #o40))
    (#(#f 32) error
     (#o327 #o40))
    (#(#f 32) error
     (#o330 #o40))
    (#(#f 32) error
     (#o331 #o40))
    (#(#f 32) error
     (#o332 #o40))
    (#(#f 32) error
     (#o333 #o40))
    (#(#f 32) error
     (#o334 #o40))
    (#(#f 32) error
     (#o335 #o40))
    (#(#f 32) error
     (#o336 #o40))
    (#(#f 32) error
     (#o337 #o40))
    ;; 3-byte seqs with no continuation
    (#(#f 32) error
     (#o340 #o40))
    (#(#f 32) error
     (#o341 #o40))
    (#(#f 32) error
     (#o342 #o40))
    (#(#f 32) error
     (#o343 #o40))
    (#(#f 32) error
     (#o344 #o40))
    (#(#f 32) error
     (#o345 #o40))
    (#(#f 32) error
     (#o346 #o40))
    (#(#f 32) error
     (#o347 #o40))
    (#(#f 32) error
     (#o350 #o40))
    (#(#f 32) error
     (#o351 #o40))
    (#(#f 32) error
     (#o352 #o40))
    (#(#f 32) error
     (#o353 #o40))
    (#(#f 32) error
     (#o354 #o40))
    (#(#f 32) error
     (#o355 #o40))
    (#(#f 32) error
     (#o356 #o40))
    (#(#f 32) error
     (#o357 #o40))
    ;; 3-byte seqs with partial continuation
    (#(#f 32) error
     (#o340 #o203 #o40))
    (#(#f 32) error
     (#o341 #o203 #o40))
    (#(#f 32) error
     (#o342 #o203 #o40))
    (#(#f 32) error
     (#o343 #o203 #o40))
    (#(#f 32) error
     (#o344 #o203 #o40))
    (#(#f 32) error
     (#o345 #o203 #o40))
    (#(#f 32) error
     (#o346 #o203 #o40))
    (#(#f 32) error
     (#o347 #o203 #o40))
    (#(#f 32) error
     (#o350 #o203 #o40))
    (#(#f 32) error
     (#o351 #o203 #o40))
    (#(#f 32) error
     (#o352 #o203 #o40))
    (#(#f 32) error
     (#o353 #o203 #o40))
    (#(#f 32) error
     (#o354 #o203 #o40))
    (#(#f 32) error
     (#o355 #o203 #o40))
    (#(#f 32) error
     (#o356 #o203 #o40))
    (#(#f 32) error
     (#o357 #o203 #o40))
    ;; 4-byte seq with no continuations
    (#(#f 32) error
     (#o360 #o40))
    (#(#f 32) error
     (#o361 #o40))
    (#(#f 32) error
     (#o362 #o40))
    (#(#f 32) error
     (#o363 #o40))
    (#(#f 32) error
     (#o364 #o40))
    (#(#f 32) error
     (#o365 #o40))
    (#(#f 32) error
     (#o366 #o40))
    (#(#f 32) error
     (#o367 #o40))
    ;; 4-byte seq with only 1 continuation
    (#(#f 32) error
     (#o360 #o203 #o40))
    (#(#f 32) error
     (#o361 #o203 #o40))
    (#(#f 32) error
     (#o362 #o203 #o40))
    (#(#f 32) error
     (#o363 #o203 #o40))
    (#(#f 32) error
     (#o364 #o203 #o40))
    (#(#f 32) error
     (#o365 #o203 #o40))
    (#(#f 32) error
     (#o366 #o203 #o40))
    (#(#f 32) error
     (#o367 #o203 #o40))
    ;; 4-byte seq with only 2 continuation
    (#(#f 32) error
     (#o360 #o203 #o203 #o40))
    (#(#f 32) error
     (#o361 #o203 #o203 #o40))
    (#(#f 32) error
     (#o362 #o203 #o203 #o40))
    (#(#f 32) error
     (#o363 #o203 #o203 #o40))
    (#(#f 32) error
     (#o364 #o203 #o203 #o40))
    (#(#f 32) error
     (#o365 #o203 #o203 #o40))
    (#(#f 32) error
     (#o366 #o203 #o203 #o40))
    (#(#f 32) error
     (#o367 #o203 #o203 #o40))
    ;; 5-byte seqs with no continuation
    (#(#f 32) error
     (#o370 #o40))
    (#(#f 32) error
     (#o371 #o40))
    (#(#f 32) error
     (#o372 #o40))
    (#(#f 32) error
     (#o373 #o40))
    ;; 5-byte seqs with only 1 continuation
    (#(#f 32) error
     (#o370 #o203 #o40))
    (#(#f 32) error
     (#o371 #o203 #o40))
    (#(#f 32) error
     (#o372 #o203 #o40))
    (#(#f 32) error
     (#o373 #o203 #o40))
    ;; 5-byte seqs with only 2 continuations
    (#(#f 32) error
     (#o370 #o203 #o203 #o40))
    (#(#f 32) error
     (#o371 #o203 #o203 #o40))
    (#(#f 32) error
     (#o372 #o203 #o203 #o40))
    (#(#f 32) error
     (#o373 #o203 #o203 #o40))
    ;; 5-byte seqs with only 3 continuations
    (#(#f 32) error
     (#o370 #o203 #o203 #o203 #o40))
    (#(#f 32) error
     (#o371 #o203 #o203 #o203 #o40))
    (#(#f 32) error
     (#o372 #o203 #o203 #o203 #o40))
    (#(#f 32) error
     (#o373 #o203 #o203 #o203 #o40))
    ;; 6-byte seqs with no continuation
    (#(#f 32) error
     (#o374 #o40))
    (#(#f 32) error
     (#o375 #o40))
    ;; 6-byte seqs with only 1 continuation
    (#(#f 32) error
     (#o374 #o203 #o40))
    (#(#f 32) error
     (#o375 #o203 #o40))
    ;; 6-byte seqs with only 2 continuation
    (#(#f 32) error
     (#o374 #o203 #o203 #o40))
    (#(#f 32) error
     (#o375 #o203 #o203 #o40))
    ;; 6-byte seqs with only 3 continuation
    (#(#f 32) error
     (#o374 #o203 #o203 #o203 #o40))
    (#(#f 32) error
     (#o375 #o203 #o203 #o203 #o40))
    ;; 6-byte seqs with only 4 continuation
    (#(#f 32) error
     (#o374 #o203 #o203 #o203 #o203 #o40))
    (#(#f 32) error
     (#o375 #o203 #o203 #o203 #o203 #o40))
    ;; Sequences with last continuation byte missing, eol instead of space
    (#(#f) aborts
     (#o300))
    (#(#f) aborts
     (#o340 #o200))
    (#(#f) aborts
     (#o340))
    (#(#f) aborts
     (#o360 #o200 #o200))
    (#(#f) aborts
     (#o360 #o200))
    (#(#f) aborts
     (#o360 #o200))
    (#(#f) aborts
     (#o370 #o200 #o200 #o200))
    (#(#f) aborts
     (#o370))
    (#(#f) aborts
     (#o370 #o200))
    (#(#f) aborts
     (#o370 #o200 #o200))
    (#(#f) aborts
     (#o374 #o200 #o200 #o200 #o200))
    (#(#f) aborts
     (#o374))
    (#(#f) aborts
     (#o374 #o200))
    (#(#f) aborts
     (#o374 #o200 #o200))
    (#(#f) aborts
     (#o374 #o200 #o200 #o200))
    (#(#f) aborts
     (#o337))
    (#(#f) aborts
     (#o357 #o277))
    (#(#f) aborts
     (#o367 #o277 #o277))
    (#(#f) aborts
     (#o373 #o277 #o277 #o277))
    (#(#f) aborts
     (#o375 #o277 #o277 #o277 #o277))
    ;; Concatenation of incomplete sequences
    (#(#f #f #f #f #f #f #f #f #f #f) error/aborts
     (#o300 #o340 #o200 #o360 #o200 #o200 #o370 #o200 #o200 #o200 #o374 #o200 #o200 #o200 #o200 #o337 #o357 #o277 #o367 #o277 #o277 #o373 #o277 #o277 #o277 #o375 #o277 #o277 #o277 #o277))
    ;; Impossible bytes
    (#(#f) error
     (#o376))
    (#(#f) error
     (#o377))
    (#(#f #f #f #f) error
     (#o376 #o376 #o377 #o377))
    ;; Overlong
    (#(#f) error
     (#o300 #o257))
    (#(#f) error
     (#o340 #o200 #o257))
    (#(#f) error
     (#o360 #o200 #o200 #o257))
    (#(#f) error
     (#o370 #o200 #o200 #o200 #o257))
    (#(#f) error
     (#o374 #o200 #o200 #o200 #o200 #o257))
    (#(#f) error
     (#o301 #o277))
    (#(#f) error
     (#o340 #o237 #o277))
    (#(#f) error
     (#o360 #o217 #o277 #o277))
    (#(#f) error
     (#o370 #o207 #o277 #o277 #o277))
    (#(#f) error
     (#o374 #o203 #o277 #o277 #o277 #o277))
    (#(#f) error
     (#o300 #o200))
    (#(#f) error
     (#o340 #o200 #o200))
    (#(#f) error
     (#o360 #o200 #o200 #o200))
    (#(#f) error
     (#o370 #o200 #o200 #o200 #o200))
    (#(#f) error
     (#o374 #o200 #o200 #o200 #o200 #o200))
    ;; illedgal surrogates
    (#(#f) error
     (#o355 #o240 #o200))
    (#(#f) error
     (#o355 #o255 #o277))
    (#(#f) error
     (#o355 #o256 #o200))
    (#(#f) error
     (#o355 #o257 #o277))
    (#(#f) error
     (#o355 #o260 #o200))
    (#(#f) error
     (#o355 #o276 #o200))
    (#(#f) error
     (#o355 #o277 #o277))
    (#(#f #f) error
     (#o355 #o240 #o200 #o355 #o260 #o200))
    (#(#f #f) error
     (#o355 #o240 #o200 #o355 #o277 #o277))
    (#(#f #f) error
     (#o355 #o255 #o277 #o355 #o260 #o200))
    (#(#f #f) error
     (#o355 #o255 #o277 #o355 #o277 #o277))
    (#(#f #f) error
     (#o355 #o256 #o200 #o355 #o260 #o200))
    (#(#f #f) error
     (#o355 #o256 #o200 #o355 #o277 #o277))
    (#(#f #f) error
     (#o355 #o257 #o277 #o355 #o260 #o200))
    (#(#f #f) error
     (#o355 #o257 #o277 #o355 #o277 #o277))
    ;; Other illegal code positions
    (#(#f) error
     (#o357 #o277 #o276))
    (#(#f) error
     (#o357 #o277 #o277))))

(define bytes->unicode-vector
  (case-lambda
   [(s)
    (list->vector (map char->integer (string->list (bytes->string/utf-8 s))))]
   [(s start)
    (bytes->unicode-vector (subbytes s start))]
   [(s start end)
    (bytes->unicode-vector (subbytes s start end))]))
(define (bytes-any->unicode-vector s non-v)
  (list->vector 
   (map (lambda (x) (if (= x (char->integer #\?)) non-v x))
	(map char->integer (string->list (bytes->string/utf-8 s #\?))))))
(define (unicode-vector->bytes v)
  (string->bytes/utf-8 (list->string (map integer->char (vector->list v)))))

(let ([utf-8-iconv (bytes-open-converter "UTF-8" "UTF-8")]
      [utf-8-iconv-p (bytes-open-converter "UTF-8-permissive" "UTF-8")])
  (for-each (lambda (p)
	      (let ([code-points (car p)]
		    [parse-status (cadr p)]
		    [s (apply bytes (caddr p))])
		(if (and (positive? (vector-length code-points))
			 (vector-ref code-points 0))
		    (begin
		      (test (vector-length code-points) bytes-utf-8-length s)
		      (test code-points bytes->unicode-vector s)
		      (test code-points bytes-any->unicode-vector s #f)
		      (test s unicode-vector->bytes code-points)
		      (test 0 bytes-utf-8-index s 0)
		      (test (bytes-length s) bytes-utf-8-index 
			    (bytes-append s #"x")
			    (vector-length code-points))
		      (if ((vector-length code-points) . > . 1)
			  (begin
			    (let ([post-1 (bytes-utf-8-index s 1)])
			      (test #t positive? post-1)
			      (test (list->vector (cdr (vector->list code-points)))
				    bytes->unicode-vector s post-1))
			    (let ([last-1 (bytes-utf-8-index s (sub1 (vector-length code-points)))])
			      (test #t positive? last-1)
			      (test code-points
				    list->vector (append
						  (vector->list (bytes->unicode-vector s 0 last-1))
						  (vector->list (bytes->unicode-vector s last-1))))))
			  (test #f bytes-utf-8-index s 1))
		      (test-values (list s (bytes-length s) 'complete)
				   (lambda () (bytes-convert utf-8-iconv s))))
		    (begin
		      (test code-points bytes-any->unicode-vector s #f)
		      (test (list->vector (append '(97) (vector->list code-points) '(98)))
			    bytes-any->unicode-vector (bytes-append #"a" s #"b") #f)
		      (test (list->vector (append (vector->list code-points) (vector->list code-points)))
			    bytes-any->unicode-vector (bytes-append s s) #f)
		      (test #f bytes-utf-8-length s)
		      (test 0 bytes-utf-8-index s 0)
		      (let-values ([(s2 n status) (bytes-convert utf-8-iconv s)])
			(test (case parse-status 
				[(error/aborts) 'error]
				[else parse-status ])
			      'status status))
		      (let ([convert
			     (lambda (prefix)
			       (let-values ([(s2 n status) 
					     (bytes-convert utf-8-iconv-p (bytes-append prefix s))]
					    [(pl) (bytes-length prefix)])
				 (case parse-status
				   [(error)
				    (test 'complete 'status status)
				    (test (+ (bytes-length s) pl) 'n n)
				    (test (+ (vector-length code-points) pl) bytes-length s2)
				    (test (append (bytes->list prefix)
						  (map 
						   (lambda (i) (or i (char->integer #\?)))
						   (vector->list code-points)))
					  vector->list (bytes->unicode-vector s2))]
				   [(error/aborts)
				    (test 'aborts 'status status)
				    (let ([code-points (list->vector
							(reverse
							 (cdr
							  (reverse
							   (vector->list code-points)))))])
				      (test (+ (vector-length code-points) pl) bytes-length s2)
				      (test (append (bytes->list prefix)
						    (map 
						     (lambda (i) (or i (char->integer #\?)))
						     (vector->list code-points)))
					    vector->list (bytes->unicode-vector s2)))]
				   [else
				    (test parse-status 'status status)])))])
			(convert #"")
			(convert #"so"))
		      (let-values ([(s2 n status) (bytes-convert utf-8-iconv-p (bytes-append s #"xxxxx"))])
			(test 'complete 'status status)
			(test '(#"xxxxx") regexp-match #rx#"xxxxx$" s2)
			(test (+ 5 (bytes-length s)) 'n n))))
		;; Test byte reading and port positions
		(let ([v (bytes-any->unicode-vector s #f)])
		  (let ([p (open-input-bytes s)])
		    (port-count-lines! p)
		    (read-bytes 500 p)
		    (let-values ([(l c p) (port-next-location p)])
		      (test (vector-length v) 'c c)
		      (test (add1 (vector-length v)) 'p p)))
		  (let ([p (open-input-string (format "\t~a\t" s))])
		    (port-count-lines! p)
		    (read-bytes 500 p)
		    (let-values ([(l c p) (port-next-location p)])
		      (test p 'p (add1 (+ 2 (vector-length v))))
		      (test c 'tab (+ 16
				      (- (vector-length v)
					 (bitwise-and (vector-length v) 7))))))
		  (let ([p (open-input-string (format "~a\t~a" s s))])
		    (port-count-lines! p)
		    (read-bytes 500 p)
		    (let-values ([(l c p) (port-next-location p)])
		      (test p 'p (add1 (+ 1 (* 2 (vector-length v)))))
		      (test c 'tab (+ 8
				      (- (vector-length v)
					 (bitwise-and (vector-length v) 7))
				      (vector-length v))))))
		;; Test read-string decoding
		(let ([us (apply string (map (lambda (i)
					       (if i (integer->char i) #\?))
					     (vector->list code-points)))])
		  (test us bytes->string/utf-8 s #\?)
		  (test us read-string (vector-length code-points) (open-input-bytes s))
		  (test us read-string (* 100 (vector-length code-points)) (open-input-bytes s))
		  (unless (string=? "" us)
		    (test (substring us 0 1) read-string 1 (open-input-bytes s)))
		  (let ([go (lambda (read-string)
			      (let-values ([(in out) (make-pipe)])
				(let loop ([i 0])
				  (unless (= i 10)
				    (write-bytes s out)
				    (loop (add1 i))))
				(close-output-port out)
				(test #t 'read-10
				      (let loop ([i 0])
					(if (= i 10)
					    #t
					    (let ([rs (read-string (string-length us) in)])
					      (and (or (string=? us rs)
						       (test us `(,i of 10 for ,(caddr p) with ,read-string) rs))
						   (loop (add1 i)))))))))])
		    (go read-string)
		    ;; Peek-string at front, consistent with read-string:
		    (go (lambda (n p)
			  (let ([a (peek-string n 0 p)]
				[b (read-string n p)])
			    (if (equal? a b)
				a
				(list a b)))))
		    ;; Peek-string, using offset calculated from s
		    (go (let ([pos 0])
			  (lambda (n p)
			    (begin0
			     (peek-string n pos p)
			     (set! pos (+ (bytes-length s) pos))))))
		    ;; Read-char
		    (go (lambda (n p)
			  (list->string
			   (let loop ([i 0])
			     (if (= i n)
				 null
				 (cons (read-char p) (loop (add1 i))))))))
		    ;; Peek-char, consistent with read-char
		    (go (lambda (n p)
			    (list->string
			     (let loop ([i 0])
			       (if (= i n)
				   null
				   (let ([a (peek-char p 0)]
					 [b (read-char p)])
				     (if (equal? a b)
					 (cons a (loop (add1 i)))
					 (list* #\* a b #\* (loop (add1 i))))))))))))))
	    basic-utf-8-tests))

(test '(#o302 #o251) bytes->list (unicode-vector->bytes (vector 169)))
(test '(#o304 #o250) bytes->list (unicode-vector->bytes (vector 296)))

(test '("\uA9") regexp-match #rx"." "\uA9")
(test '(#"\302") regexp-match #rx#"." #"\302\251")
(test '(#"\302") regexp-match #rx#"." "\uA9")
(test '(#"\302\251") regexp-match #rx"." #"\302\251")

(test #f regexp-match #rx"[a-z]" "\uA9")
(test '("\uA9") regexp-match #rx"\uA9" "\uA9")
(test '("\uA9") regexp-match #rx"\uA9+" "\uA9")
(test '("\uA9\uA9") regexp-match #rx"\uA9+" "\uA9\uA9")
(test '("\uA9\uA9") regexp-match #rx"\uA9+" "x\uA9\uA9y")
(test '("\uA9") regexp-match #rx"[a-z\uA9]" "\uA9mm")
(test '("\uA9mm") regexp-match #rx"[a-z\uA9]+" "\uA9mm")
(test '("\uA9") regexp-match #rx"[\uA9-\uE9]+" "xx\uA9mm")
(test '("\uE9") regexp-match #rx"[\uA9-\uE9]+" "xx\uE9mm")
(test #f regexp-match #rx"[\uA9-\uE9]+" "xx\u129")
(test #f regexp-match #rx"[\uA9-\uE9]+" "xx\uA8mm")
(test '("\uE9\uA9") regexp-match #rx"[\uA9-\u128]+" "xx\uE9\uA9mm")

(test #f regexp-match #rx"[^a-z][^a-z]" "\uA9")
(test #f regexp-match #rx"[^a-z][^a-z]" (string->bytes/utf-8 "\uA9"))
(test (list (string->bytes/utf-8 "\uA9")) regexp-match #rx#"[^a-z][^a-z]" "\uA9")
(test (list (string->bytes/utf-8 "\uA9")) regexp-match #rx#"[^a-z][^a-z]" (string->bytes/utf-8 "\uA9"))

;; Nots of patterns and ranges:
(test #f regexp-match #rx"[^a-z\uA9]" "\uA9mm")
(test '("") regexp-match #rx"[^a-d\uA9]*" "\uA9mm")
(test '("xx") regexp-match #rx"[^\uA9-\u128]+" "xx\uA9mm")
(test '("xx") regexp-match #rx"[^\uA9-\u128]+" "xx\303\251mm")
(test '("xx\u129mm") regexp-match #rx"[^\uA9-\u128]+" "xx\u129mm")
(test '("xx\uA8mm") regexp-match #rx"[^\uA9-\u128]+" "xx\uA8mm")
(test '("xx") regexp-match #rx"[^\uA9-\u128]+" "xx\uE9\uA9mm")

;; 3-char seqs
(test '("\u1F79") regexp-match #rx"\u1F79" "a\u1F79\u1F79b")
(test '("\u1F79\u1F79") regexp-match #rx"\u1F79+" "a\u1F79\u001F79b")

(test '("\u1F79\uA9\u1F79r") regexp-match #rx"[c-\u1F79]+" "a\u1F79\uA9\u1F79r")
(test '("d\u1F79\uA9\u1F79r") regexp-match #rx"[c-\u1F79]+" "d\u1F79\uA9\u1F79r")

(test '("\u2F79") regexp-match #rx"[\u1F79-\u3F79]" "\u2F79")
(test '("\u1F79") regexp-match #rx"[\u1F79-\u3F79]" "\u1F79")
(test #f regexp-match #rx"[\u1F79-\u3F79]" "\u1F39")
(test #f regexp-match #rx"[\u1F79-\u3F79]" "\u1F78")

;; Nots of 3-char seqs:
(test #f regexp-match #rx"[^\u1F79-\u3F79]" "\u2F79")
(test #f regexp-match #rx"[^\u1F79-\u3F79]" "\u1F79")
(test '("\u1F39") regexp-match #rx"[^\u1F79-\u3F79]" "\u1F39")
(test '("\u1F78") regexp-match #rx"[^\u1F79-\u3F79]" "\u1F78")


;; Regexps that shouldn't parse:
(err/rt-test (regexp "[a--b\u1F78]") exn:misc?)
(err/rt-test (regexp "[a-b-c\u1F78]") exn:misc?)

;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; Let Matthew perform some basic sanity checks for locale-sensitive
;; comparisons:
(define known-locale? (and (regexp-match "mflatt|matthewf" (path->string (find-system-path 'home-dir)))
			   (or (regexp-match "linux" (path->string (system-library-subpath)))
			       (eq? 'macosx (system-type)))))

;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; String comparison. Relies on the default locale knowing
;;  about upper A with hat (\303\202) and lower a with hat (\303\242),
;;  and also relies on a "C" locale that can't encode those
;;  two characters. It doesn't rely on a relative order of A-hat
;;  and a-hat --- only that they're the same case-insensitively.
(when known-locale?
  (let ()
    (define (stest r comp? a b)
      (test r comp? a b)
      (test r comp? (format "xx~ayy" a) (format "xx~ayy" b))
      (test r comp? (format "x\000x~ay" a) (format "x\000x~ay" b))
      (test r comp? (format "x\000~ay" a) (format "x\000~ay" b))
      (test r comp? (format "x\000~a\000y" a) (format "x\000~a\000y" b)))
    (define (go c?)
      (stest #f string=? "A" "a")
      (stest #t string-ci=? "A" "a")
      (stest #t string-locale-ci=? "A" "a")
      (stest #f string=? "\uC2" "\uE2") 
      (stest #t string-ci=? "\uC2" "\uE2")
      (stest #f string-locale=? "\uC2" "\uE2")
      (stest (if c? #f #t) string-locale-ci=? "\uC2" "\uE2")
      (stest #f string<? "\uE2" "b")
      (stest (if c? #f #t) string-locale<? "\uE2" "b")
      (stest #t string>? "\uE2" "b")
      (stest (if c? #t #f) string-locale>? "\uE2" "b")
      (stest #t string<? "b" "\uE2")
      (stest (if c? #t #f) string-locale<? "b" "\uE2")
      (stest #f string>? "b" "\uE2")
      (stest (if c? #f #t) string-locale>? "b" "\uE2")
      (test "ABC" string-locale-upcase "aBc")
      (test (if c? "\uE2" "\uC2") string-locale-upcase "\uE2")
      (test (if c? "A\uE2\0B" "A\uC2\0B") string-locale-upcase "a\uE2\0b")
      (test (if c? "A\uE2\0\uE2\0B" "A\uC2\0\uC2\0B") string-locale-upcase "a\uE2\0\uE2\0b"))
    (go #f)
    (parameterize ([current-locale "C"])
      (go #t))))

;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;  locale<->UTF-8 conversions

(test #f bytes-open-converter "UTF-8" "nowhere")
(test #f bytes-open-converter "nowhere" "UTF-8")
(when known-locale?
  (let ([c (bytes-open-converter "UTF-8" "")])
    (test (void) bytes-close-converter c)
    (test (void) bytes-close-converter c)
    (err/rt-test (bytes-convert c #"hello") exn:application:mismatch?)))

(define (bytes-encode-... from to)
  (lambda args
    (let ([c (bytes-open-converter from to)])
      (dynamic-wind
	  void
	  (lambda ()
	    (apply bytes-convert c args))
	  (lambda ()
	    (bytes-close-converter c))))))

(define bytes-encode-locale->utf-8 (bytes-encode-... "" "UTF-8"))
(define bytes-encode-utf-8->locale (bytes-encode-... "UTF-8" ""))

;; Relies on a default locale that extends Latin-1:
(when known-locale?
  (let-values ([(s l ok?) (bytes-encode-utf-8->locale #"abc")])
    (test 3 'count l)
    (test 'complete 'ok ok?))
  (let-values ([(s l ok?) (bytes-encode-utf-8->locale #"a\300\300")])
    (test 1 'count l)
    (test 'error 'ok ok?))
  (let-values ([(s l ok?) (bytes-encode-utf-8->locale #"a\302\200")])
    (test 3 'count l)
    (test 'complete 'ok ok?))
  (test-values '(#"abcd" 4 complete) (lambda () (bytes-encode-locale->utf-8 #"abcd")))
  (test-values '(#"bcd" 3 complete) (lambda () (bytes-encode-locale->utf-8 #"abcd" 1)))
  (test-values '(#"bc" 2 complete) (lambda () (bytes-encode-locale->utf-8 #"abcd" 1 3)))
  (test-values '(#"a" 1 aborts) (lambda () (bytes-encode-locale->utf-8 #"a\302\200" 0 2)))
  (test-values '(2 2 complete) (lambda () (bytes-encode-locale->utf-8 #"abcd" 1 3 (make-bytes 2))))
  (let ([s (make-bytes 10)])
    (test-values '(2 2 complete) (lambda () (bytes-encode-locale->utf-8 #"abcd" 1 3 s 4 7)))
    (test 98 bytes-ref s 4)
    (test 99 bytes-ref s 5))
  (let ([s (make-bytes 10)])
    (test-values '(1 1 continues) (lambda () (bytes-encode-locale->utf-8 #"abcd" 1 3 s 4 5)))
    (test 98 bytes-ref s 4))
  ;; The rest relies on the "C" locale:
  (parameterize ([current-locale "C"])
    (test-values '(#"abc" 3 complete) (lambda () (bytes-encode-utf-8->locale #"abc")))
    (test-values '(#"a" 1 error) (lambda () (bytes-encode-utf-8->locale #"a\300\300")))
    (test-values '(#"ab" 2 aborts) (lambda () (bytes-encode-utf-8->locale #"ab\303")))
    ;; Well-formed, but can't be converted to "C":
    (test-values '(#"a" 1 error) (lambda () (bytes-encode-utf-8->locale #"a\303\342")))))

(err/rt-test (bytes-encode-utf-8->locale 'ok))
(err/rt-test (bytes-encode-utf-8->locale "ok"))
(err/rt-test (bytes-encode-utf-8->locale #"ok" -1))
(err/rt-test (bytes-encode-utf-8->locale #"ok" 3) exn:application:mismatch?)
(err/rt-test (bytes-encode-utf-8->locale #"ok" 1 0) exn:application:mismatch?)
(err/rt-test (bytes-encode-utf-8->locale #"ok" 1 3) exn:application:mismatch?)
(err/rt-test (bytes-encode-utf-8->locale #"ok" 1 2 'nope))
(err/rt-test (bytes-encode-utf-8->locale #"ok" 1 2 #"nope"))
(let ([s (make-string 4)])
  (err/rt-test (bytes-encode-utf-8->locale #"ok" 1 3 s -1) exn:application:mismatch?)
  (err/rt-test (bytes-encode-utf-8->locale #"ok" 1 3 s 5) exn:application:mismatch?)
  (err/rt-test (bytes-encode-utf-8->locale #"ok" 1 3 s 2 1) exn:application:mismatch?)
  (err/rt-test (bytes-encode-utf-8->locale #"ok" 1 3 s 2 7) exn:application:mismatch?))

;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; The following tests are derived from SRFI-14

(define (check-all-latin-1 ? l)
  (for-each (lambda (c)
	      (test #t ? c))
	    l)
  (let loop ([i 0])
    (unless (= i 256)
      (unless (memq (integer->char i) l)
	(test #f ? (integer->char i)))
      (loop (add1 i)))))

(define (check-all-unicode ? l)
  (define (unless-in-l ? c)
    (and (? c)
	 (not (member c l))))
  (define (qtest expect f . args)
    (unless (equal? expect (apply f args))
      (apply test expect f args)))
  (for-each (lambda (c)
	      (test #t ? c))
	    l)
  (for-each (lambda (r)
	      (if (caddr r)
		  (qtest #f unless-in-l ? (integer->char (car r)))
		  (let loop ([i (car r)])
		    (qtest #f unless-in-l  ? (integer->char i))
		    (unless (= i (cadr r))
		      (loop (add1 i))))))
	    (make-known-char-range-list)))

(check-all-latin-1
 char-lower-case?
 '(#\a
   #\b
   #\c
   #\d
   #\e
   #\f
   #\g
   #\h
   #\i
   #\j
   #\k
   #\l
   #\m
   #\n
   #\o
   #\p
   #\q
   #\r
   #\s
   #\t
   #\u
   #\v
   #\w
   #\x
   #\y
   #\z
   #\u00B5
   #\u00DF
   #\u00E0
   #\u00E1
   #\u00E2
   #\u00E3
   #\u00E4
   #\u00E5
   #\u00E6
   #\u00E7
   #\u00E8
   #\u00E9
   #\u00EA
   #\u00EB
   #\u00EC
   #\u00ED
   #\u00EE
   #\u00EF
   #\u00F0
   #\u00F1
   #\u00F2
   #\u00F3
   #\u00F4
   #\u00F5
   #\u00F6
   #\u00F8
   #\u00F9
   #\u00FA
   #\u00FB
   #\u00FC
   #\u00FD
   #\u00FE
   #\u00FF))

;; No upper case in latin-1
(check-all-latin-1
 (lambda (x) (and (char-lower-case? x)
		  (or (eq? x (char-upcase x))
		      (> (char->integer (char-upcase x)) 255))))
 '(#\u00B5
   #\u00DF
   #\u00FF))

;; Latin-1 uppercase:
(check-all-latin-1
 char-upper-case?
 '(#\A
   #\B
   #\C
   #\D
   #\E
   #\F
   #\G
   #\H
   #\I
   #\J
   #\K
   #\L
   #\M
   #\N
   #\O
   #\P
   #\Q
   #\R
   #\S
   #\T
   #\U
   #\V
   #\W
   #\X
   #\Y
   #\Z
   #\u00C0
   #\u00C1
   #\u00C2
   #\u00C3
   #\u00C4
   #\u00C5
   #\u00C6
   #\u00C7
   #\u00C8
   #\u00C9
   #\u00CA
   #\u00CB
   #\u00CC
   #\u00CD
   #\u00CE
   #\u00CF
   #\u00D0
   #\u00D1
   #\u00D2
   #\u00D3
   #\u00D4
   #\u00D5
   #\u00D6
   #\u00D8
   #\u00D9
   #\u00DA
   #\u00DB
   #\u00DC
   #\u00DD
   #\u00DE))

;; Extra letters (not lower or upper) in Latin-1:
(check-all-latin-1
 (lambda (c)
   (and (char-alphabetic? c)
	(not (char-upper-case? c))
	(not (char-lower-case? c))))
 '(#\u00AA
   #\u00BA))

;; Complete titlecase list:
(check-all-unicode
 char-title-case?
 '(#\u01C5
   #\u01C8
   #\u01CB
   #\u01F2
   #\u1F88
   #\u1F89
   #\u1F8A
   #\u1F8B
   #\u1F8C
   #\u1F8D
   #\u1F8E
   #\u1F8F
   #\u1F98
   #\u1F99
   #\u1F9A
   #\u1F9B
   #\u1F9C
   #\u1F9D
   #\u1F9E
   #\u1F9F
   #\u1FA8
   #\u1FA9
   #\u1FAA
   #\u1FAB
   #\u1FAC
   #\u1FAD
   #\u1FAE
   #\u1FAF
   #\u1FBC
   #\u1FCC
   #\u1FFC))

;; Complete whitspace list:
(check-all-unicode
 char-whitespace?
 '(#\u0009
   #\u000A
   #\u000B
   #\u000C
   #\u000D
   #\u0020
   #\u00A0
   #\u1680
   #\u2000
   #\u2001
   #\u2002
   #\u2003
   #\u2004
   #\u2005
   #\u2006
   #\u2007
   #\u2008
   #\u2009
   #\u200A
   #\u200B
   #\u2028
   #\u2029
   #\u202F
   #\u3000
   ;; Post SRFI-14?
   #\u205F
   #\u180E))

;; Punctuation in Latin-1:
(check-all-latin-1
 char-punctuation?
 '(#\!
   #\"
   #\#
   #\%
   #\&
   #\'
   #\(
   #\)
   #\*
   #\,
   #\-
   #\.
   #\/
   #\:
   #\;
   #\?
   #\@
   #\[
   #\\
   #\]
   #\_
   #\{
   #\}
   #\u00A1
   #\u00AB
   ;; #\u00AD ;; Treated as a control character now?
   #\u00B7
   #\u00BB
   #\u00BF))

;; Symbols in Latin-1
(check-all-latin-1
 char-symbolic?
 '(#\$
   #\+
   #\<
   #\=
   #\>
   #\^
   #\`
   #\|
   #\~
   #\u00A2
   #\u00A3
   #\u00A4
   #\u00A5
   #\u00A6
   #\u00A7
   #\u00A8
   #\u00A9
   #\u00AC
   #\u00AE
   #\u00AF
   #\u00B0
   #\u00B1
   #\u00B4
   #\u00B6
   #\u00B8
   #\u00D7
   #\u00F7))

;; Complete blank list:
(check-all-unicode
 char-blank?
 '(#\u0009
   #\u0020
   #\u00A0
   #\u1680
   #\u2000
   #\u2001
   #\u2002
   #\u2003
   #\u2004
   #\u2005
   #\u2006
   #\u2007
   #\u2008
   #\u2009
   #\u200A
   #\u200B
   #\u202F
   #\u3000
   ;; Post SRFI-14?
   #\u205F
   #\u180E
   ))


;; Every letter, digit, punct, and symbol is graphic
(check-all-unicode
 (lambda (c)
   (and (not (char-graphic? c))
	(or (char-alphabetic? c)
	    (char-numeric? c)
	    (char-punctuation? c)
	    (char-symbolic? c))))
 null)

;; Letter, digit, punct, and symbol are distinct
(check-all-unicode
 (lambda (c)
   (> (+ (if (char-alphabetic? c) 1 0)
	 (if (char-numeric? c) 1 0)
	 (if (char-punctuation? c) 1 0)
	 (if (char-symbolic? c) 1 0))
      1))
 null)

(check-all-unicode
 char-iso-control?
 (append
  (let loop ([i 0])
    (if (= i #x20)
	null
	(cons (integer->char i) (loop (add1 i)))))
  (let loop ([i #x7F])
    (if (= i #xA0)
	null
	(cons (integer->char i) (loop (add1 i)))))))


(report-errs)