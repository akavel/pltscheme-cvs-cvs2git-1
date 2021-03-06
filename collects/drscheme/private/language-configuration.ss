
(module language-configuration mzscheme
  (require (lib "unitsig.ss")
           (lib "hierlist.ss" "hierlist")
           (lib "class.ss")
	   (lib "contract.ss")
           "drsig.ss"
           (lib "string-constant.ss" "string-constants")
	   (lib "mred.ss" "mred")
           (lib "framework.ss" "framework")
           (lib "list.ss")
           (lib "etc.ss")
           (lib "file.ss")
           (lib "pconvert.ss")
           (lib "getinfo.ss" "setup")
           (lib "toplevel.ss" "syntax"))
  
  (define original-output (current-output-port))
  (define (printfo . args) (apply fprintf original-output args))
  
  (provide language-configuration@)
  
  (define language-configuration@
    (unit/sig drscheme:language-configuration/internal^
      (import [drscheme:unit : drscheme:unit^]
              [drscheme:rep : drscheme:rep^]
              [drscheme:teachpack : drscheme:teachpack^]
              [drscheme:init : drscheme:init^]
              [drscheme:language : drscheme:language^]
              [drscheme:app : drscheme:app^]
              [drscheme:tools : drscheme:tools^]
              [drscheme:help-desk : drscheme:help-desk^])
      
      ;; settings-preferences-symbol : symbol
      ;; the preferences key for the language settings.
      ;; depends on the version number, so people can use multiple versions
      ;; of drscheme and maintain separate language settings for each
      ;; of them.
      (define settings-preferences-symbol 
        (string->symbol (format "drscheme:~a-settings" (version:version))))
      
      ;; get-settings-preferences-symbol : -> symbol
      (define (get-settings-preferences-symbol) settings-preferences-symbol)

      ;; default-language-position : (listof string)
      ;; if a language is registered with this position, it is
      ;; considered the default language
      (define default-language-position
        (list (string-constant teaching-languages)
              (string-constant how-to-design-programs)
              (string-constant beginning-student)))

      ;; languages : (listof (instanceof language<%>))
      ;; all of the languages supported in DrScheme
      (define languages null)

      ;; add-language : (instanceof language%) -> void
      ;; only allows addition on phase2
      ;; effect: updates `languages'
      (define add-language
        (opt-lambda (language [front? #f])
          
          (drscheme:tools:only-in-phase 'drscheme:language:add-language 'phase2)
          (for-each
           (λ (i<%>)
             (unless (is-a? language i<%>)
               (error 'drscheme:language:add-language "expected language ~e to implement ~e, forgot to use drscheme:language:get-default-mixin ?" language i<%>)))
           (drscheme:language:get-language-extensions))
          
          (set! languages 
                (if front? 
                    (cons language languages)
                    (append languages (list language))))))
      
      ;; get-languages : -> (listof languages)
      (define (get-languages) 
        (drscheme:tools:only-in-phase
         'drscheme:language-configuration:get-languages
         'init-complete)
        languages)
      
      ;; get-default-language-settings : -> language-settings
      ;; uses `default-language-position' to find the default language.
      ;; if that language is not available, just takes the first language.
      ;; if there are no languages defined yet, signal an error -- drscheme is in trouble.
      (define (get-default-language-settings)
	(when (null? languages)
	  (error 'get-default-language-settings "no languages registered!"))
	(let ([lang (or (ormap (λ (x)
				 (and (equal? (send x get-language-position)
					      default-language-position)
				      x))
			       (get-languages))
			(first (get-languages)))])
	  (make-language-settings lang (send lang default-settings))))

      ;; type language-settings = (make-language-settings (instanceof language<%>) settings)
      (define-struct language-settings (language settings))
       
 
;                                                                                                      
;                                                                                                      
;                                                                                                      
;   ;                                                                ;   ;          ;                  
;   ;                                                                ;              ;                  
;   ;                                                                ;              ;                  
;   ;   ;;;    ; ;;     ;; ;   ;   ;   ;;;     ;; ;    ;;;        ;; ;   ;   ;;;    ;    ;;;     ;; ;  
;   ;  ;   ;   ;;  ;   ;  ;;   ;   ;  ;   ;   ;  ;;   ;   ;      ;  ;;   ;  ;   ;   ;   ;   ;   ;  ;;  
;   ;      ;   ;   ;  ;    ;   ;   ;      ;  ;    ;  ;    ;     ;    ;   ;      ;   ;  ;     ; ;    ;  
;   ;   ;;;;   ;   ;  ;    ;   ;   ;   ;;;;  ;    ;  ;;;;;;     ;    ;   ;   ;;;;   ;  ;     ; ;    ;  
;   ;  ;   ;   ;   ;  ;    ;   ;   ;  ;   ;  ;    ;  ;          ;    ;   ;  ;   ;   ;  ;     ; ;    ;  
;   ;  ;   ;   ;   ;   ;  ;;   ;  ;;  ;   ;   ;  ;;   ;          ;  ;;   ;  ;   ;   ;   ;   ;   ;  ;;  
;   ;   ;;;;;  ;   ;    ;; ;    ;; ;   ;;;;;   ;; ;    ;;;;       ;; ;   ;   ;;;;;  ;    ;;;     ;; ;  
;                          ;                      ;                                                 ;  
;                     ;    ;                 ;    ;                                            ;    ;  
;                      ;;;;                   ;;;;                                              ;;;;   

      
      ;; language-dialog : (boolean language-setting -> language-setting)
      ;;                   (boolean language-setting (union #f (instanceof top-level-window%))
      ;;                    -> language-setting)
      ;; allows the user to configure their language. The input language-setting is used
      ;; as the defaults in the dialog and the output language setting is the user's choice
      ;; todo: when button is clicked, ensure language is selected
      (define language-dialog
        (opt-lambda (show-welcome? language-settings-to-show [parent #f] [manuals? #f])
          (define ret-dialog%
            (class dialog%
              (define/override (on-subwindow-char receiver evt)
                (case (send evt get-key-code)
                  [(escape) (cancel-callback)]
                  [(#\return numpad-enter) (ok-callback)]
                  [else (super on-subwindow-char receiver evt)]))
              (super-instantiate ())))
          
	  (define dialog (instantiate ret-dialog% ()
                           (label (if show-welcome?
                                    (string-constant welcome-to-drscheme)
                                    (string-constant language-dialog-title)))
                           (parent parent)
                           (style '(resize-border))))
          (define welcome-before-panel (instantiate horizontal-panel% ()
                                         (parent dialog)
                                         (stretchable-height #f)))
          (define language-dialog-meat-panel (make-object vertical-panel% dialog))

          (define welcome-after-panel (instantiate vertical-panel% () 
                                        (parent dialog)
                                        (stretchable-height #f)))
          
          (define button-panel (instantiate horizontal-panel% ()
                                 (parent dialog)
                                 (stretchable-height #f)))
          
          (define-values (get-selected-language get-selected-language-settings)
            (fill-language-dialog language-dialog-meat-panel 
                                  button-panel 
                                  language-settings-to-show 
                                  #f
                                  manuals?))
          
          ;; cancelled? : boolean
          ;; flag that indicates if the dialog was cancelled.
          (define cancelled? #t)
          
	  ;; ok-callback : -> void
          (define (ok-callback)
            (cond
              [(get-selected-language)
               (set! cancelled? #f)
               (send dialog show #f)]
              [else
               (message-box (string-constant drscheme)
                            (string-constant please-select-a-language))]))
          
          ;; cancel-callback : -> void
	  (define (cancel-callback)
            (send dialog show #f))

          (define show-details-label (string-constant show-details-button-label))
          (define hide-details-label (string-constant hide-details-button-label))
          
          (define button-gap (make-object horizontal-panel% button-panel))
	  (define-values (ok-button cancel-button)
            (gui-utils:ok/cancel-buttons
             button-panel
             (λ (x y) (ok-callback))
             (λ (x y) (cancel-callback))))
          (define grow-box-spacer (make-object grow-box-spacer-pane% button-panel))
          
          (when show-welcome?
            (add-welcome dialog welcome-before-panel welcome-after-panel))

          (send dialog stretchable-width #f)
          (send dialog stretchable-height #t)

          (unless parent
            (send dialog center 'both))
          (send dialog show #t)
          (if cancelled?
              language-settings-to-show
              (make-language-settings
               (get-selected-language)
               (get-selected-language-settings)))))
      
      ;; fill-language-dialog :    (vertical-panel panel language-setting -> language-setting)
      ;;                           (union dialog #f)
      ;;                        -> (-> (union #f language<%>)) (-> settings[corresponding to fst thnk result])
      ;; allows the user to configure their language. The input language-setting is used
      ;; as the defaults in the dialog and the output language setting is the user's choice
      ;; if re-center is a dialog, when the show details button is clicked, the dialog is recenterd.
      (define fill-language-dialog
        (opt-lambda (parent show-details-parent language-settings-to-show [re-center #f] [manuals? #f])

	  (define language-to-show (language-settings-language language-settings-to-show))
	  (define settings-to-show (language-settings-settings language-settings-to-show))
          
          ;; hier-list items that implement this interface correspond to
	  ;; actual language selections
	  (define hieritem-language<%>
	    (interface (hierarchical-list-item<%>)
	      selected))

	  (define selectable-hierlist%
            (class hierarchical-list%
              (init parent)

              (inherit get-selected)
              (define/override (on-char evt)
                (let ([code (send evt get-key-code)])
                  (cond
                    [(eq? code 'up) (select-next sub1 (λ (v) (- (vector-length v) 1)))]
                    [(eq? code 'down) (select-next add1 (λ (v) 0))]
                    [else (super on-char evt)])))
              
              (inherit get-items)
              
              ;; select-next : (int -> int) (vector -> int)
              ;; finds the next leaf after the selected child,
              ;; using `inc' and `start' to control the direction of the traversal.
              (define/private (select-next inc start)
                (let ([fst-selected (get-selected)])
                  (when fst-selected
                    (let loop ([item fst-selected])
		      (when item
			(let* ([parent (send item get-parent)]
			       [siblings (list->vector 
                                          (if parent
                                              (send parent get-items)
                                              (get-items)))])
			  (let sibling-loop ([index (inc (find-index item siblings))])
			    (cond
			      [(and (<= 0 index)
				    (< index (vector-length siblings)))
			       (let ([sibling (vector-ref siblings index)])
				 (cond
				   [(find-first-leaf sibling inc start)
				    =>
				    (λ (child)
				      (send fst-selected select #f)
				      (send child select #t)
				      (open-parents child)
                                      (make-visible child))]
				   [else (sibling-loop (inc index))]))]
			      [else (loop parent)]))))))))
              
              ;; find-first-leaf : item (int -> int) (vec -> int)
              ;; finds the first child, using `inc' and `start' to control
              ;; the traversal over the children.
              (define/private (find-first-leaf item inc start)
                (let loop ([item item])
                  (cond
                    [(is-a? item hierarchical-list-compound-item<%>)
                     (let ([children (list->vector (send item get-items))])
                       (let child-loop ([i (start children)])
                         (cond
                           [(and (<= 0 i) (< i (vector-length children)))
                            (or (loop (vector-ref children i))
                                (child-loop (inc i)))]
                           [else #f])))]
                    [(send item get-allow-selection?)
                     item]
                    [else #f])))

              ;; find-index : tst (vectorof tst) -> int
              ;; returns the index of `item' in `vec'
              (define/private (find-index item vec)
                (let loop ([i 0])
                  (cond
                    [(< i (vector-length vec))
                     (if (eq? (vector-ref vec i) item)
                         i
                         (loop (+ i 1)))]
                    [else (error 'find-index "didn't find ~e in ~e" item vec)])))

              ;; open-parents : item -> void
              ;; selects the item and opens all of its parents.
              (define/private (open-parents item)
                (let loop ([item (send item get-parent)])
                  (when item
                    (send item open)
                    (loop (send item get-parent)))))

              (define/private (make-visible item)
                (let loop ([item item])
                  (let ([parent (send item get-parent)])
                    (if parent
                      (loop parent)
                      (send item scroll-to))))
                (send item scroll-to))

              (define/override (on-select i)
                (cond
		  [(and i (is-a? i hieritem-language<%>))
		   (send i selected)]
                  [else
		   (nothing-selected)]))
              (define/override (on-click i)
                (when (and i (is-a? i hierarchical-list-compound-item<%>))
                  (send i toggle-open/closed)))
              (super-instantiate (parent))))

          (define outermost-panel (make-object horizontal-panel% parent))
          (define languages-hier-list (make-object selectable-hierlist% outermost-panel))
          (define details-outer-panel (make-object vertical-panel% outermost-panel))
          (define details/manual-parent-panel (make-object vertical-panel% details-outer-panel))
	  (define details-panel (make-object panel:single% details/manual-parent-panel))
          (define manual-ordering-panel (new vertical-panel% (parent details/manual-parent-panel)))
          
          (define manual-ordering-text (new panel-background-text%))
          (define manual-ordering-canvas (new panel-background-editor-canvas% 
                                              (parent manual-ordering-panel)
                                              (editor manual-ordering-text)
                                              (style '(no-hscroll))
                                              (min-width 300)))
          
          (define one-line-summary-message (instantiate message% ()
                                             (parent parent)
                                             (label "")
                                             (stretchable-width #t)))
          
	  (define no-details-panel (make-object vertical-panel% details-panel))

          (define languages-table (make-hash-table))
          (define languages (get-languages))

	  ;; selected-language : (union (instanceof language<%>) #f)
	  ;; invariant: selected-language and get/set-selected-language-settings
	  ;;            match the user's selection in the languages-hier-list.
	  ;;            or #f if the user is not selecting a language.
	  (define selected-language #f)
	  ;; get/set-selected-language-settings (union #f (-> settings))
	  (define get/set-selected-language-settings #f)

	  ;; language-mixin : (implements language<%>) (implements area-container<%>) get/set ->
          ;;                  ((implements hierlist<%>) -> (implements hierlist<%>))
	  ;; a mixin that responds to language selections and updates the details-panel
	  (define (language-mixin language language-details-panel get/set-settings)
            (λ (%)
              (class* % (hieritem-language<%>)
                (init-rest args)
                (public selected)
                (define (selected)
                  (send details-panel active-child language-details-panel)
                  (send one-line-summary-message set-label (send language get-one-line-summary))
                  (send revert-to-defaults-button enable #t)
                  (update-manual-ordering-text language)
                  (set! get/set-selected-language-settings get/set-settings)
		  (set! selected-language language))
                (apply super-make-object args))))

	  ;; nothing-selected : -> void
	  ;; updates the GUI and selected-language and get/set-selected-language-settings
	  ;; for when no language is selected.
	  (define (nothing-selected)
            (send revert-to-defaults-button enable #f)
	    (send details-panel active-child no-details-panel)
            (send one-line-summary-message set-label "")
	    (set! get/set-selected-language-settings #f)
	    (set! selected-language #f))

          ;; update-manual-ordering-text : -> void
          ;; updates the manual ordering text with the order the manuals are searched for this language
          (define (update-manual-ordering-text language)
            (send manual-ordering-text begin-edit-sequence)
            (send manual-ordering-text lock #f)
            (send manual-ordering-text erase)
            (send manual-ordering-text insert (string-constant plt:hd:manual-search-ordering))
            (send manual-ordering-text insert "\n\n")
            (send manual-ordering-text change-style 
                  (make-object style-delta% 'change-bold)
                  0
                  (- (send manual-ordering-text last-position) 2))
            (let ([docs (drscheme:help-desk:get-docs)]
                  [manual-name-style-delta
                   (make-object style-delta%)])
              (let-values ([(ordered-bytes doc.txt?)
                            (send language order-manuals (map path->bytes (map car docs)))])
                (let loop ([ordered (map bytes->path ordered-bytes)]
                           [n 1])
                  (cond
                    [(null? ordered) 
                     (when doc.txt?
                       (insert-single "doc.txt files" n))]
                    [else 
                     (let* ([ent (car ordered)]
                            [full-name (assoc ent docs)])
                       (cond
                         [full-name
                          (insert-single (cdr full-name) n)
                          (loop (cdr ordered) (+ n 1))]
                         [else
                          (loop (cdr ordered) n)]))]))))
            (send manual-ordering-text change-style
                  (make-object style-delta% 'change-family 'system)
                  0
                  (send manual-ordering-text last-position))
            (send manual-ordering-text set-position 0)
            (send manual-ordering-text lock #t)
            (send manual-ordering-text end-edit-sequence))

          (define manual-number-style-delta (make-object style-delta%))
          (define stupid-internal-define-syntax1
            (send manual-number-style-delta set-delta-foreground "darkblue"))
          
          (define (insert-single name n)
            (let ([n-sp (send manual-ordering-text last-position)])
              (send manual-ordering-text insert (number->string n))
              (let ([n-ep (send manual-ordering-text last-position)])
                (send manual-ordering-text insert ") ")
                (send manual-ordering-text insert name)
                (send manual-ordering-text insert "\n")
                
                (send manual-ordering-text change-style
                      manual-number-style-delta
                      n-sp
                      (+ n-ep 1)))))
          
          ;; add-language-to-dialog : (instanceof language<%>) -> void
	  ;; adds the language to the dialog
	  ;; opens all of the turn-down tags
          ;; when `language' matches language-to-show, update the settings
          ;;   panel to match language-to-show, otherwise set to defaults.
          (define (add-language-to-dialog language)
            (let ([positions (send language get-language-position)]
                  [numbers (send language get-language-numbers)])
              (unless (and (list? positions)
                           (list? numbers)
                           (pair? positions)
                           (pair? numbers)
                           (andmap number? numbers)
                           (andmap string? positions)
                           (= (length positions) (length numbers))
                           ((length numbers) . >= . 2))
                (error 'drscheme:language
                       "languages position and numbers must be lists of strings and numbers, respectively, must have the same length,  and must each contain at least two elements, got: ~e ~e"
                       positions numbers))
              
              #|
              
              inline the first level of the tree into just items in the hierlist
              keep track of the starting (see call to sort method below) by
              adding a second field to the second level of the tree that indicates
              what the sorting number is for its level above (in the second-number mixin)
              
              |#
              
              (let add-sub-language ([ht languages-table]
                                     [hier-list languages-hier-list]
                                     [positions positions]
                                     [numbers numbers]
                                     [first? #t]
                                     [second-number #f]) ;; only non-#f during the second iteration in which case it is the first iterations number
                (cond
                  [(null? (cdr positions))
                   (let-values ([(language-details-panel get/set-settings)
                                 (make-details-panel language)])
                     (let* ([position (car positions)]
                            [number (car numbers)]
                            [mixin (compose
                                    number-mixin
                                    (language-mixin language language-details-panel get/set-settings))]
                            [item
                             (send hier-list new-item
                                   (if second-number
                                       (compose second-number-mixin mixin)
                                       mixin))]
                            [text (send item get-editor)]
                            [delta (send language get-style-delta)])
                       (send item set-number number)
                       (when second-number
                         (send item set-second-number second-number))
                       (send text insert position)
                       (when delta
                         (cond
                           [(list? delta)
                            (for-each (λ (x)
                                        (send text change-style 
                                              (car x)
                                              (cadr x)
                                              (caddr x)))
                                      delta)]
                           [(is-a? delta style-delta%)
                            (send text change-style 
                                  (send language get-style-delta)
                                  0
                                  (send text last-position))])))
                     (cond
                       [(equal? (send language-to-show get-language-position)
                                (send language get-language-position))
                        (get/set-settings settings-to-show)]
                       [else
                        (get/set-settings (send language default-settings))]))]
                  [else (let* ([position (car positions)]
                               [number (car numbers)]
                               [sub-ht/sub-hier-list
                                (hash-table-get
                                 ht
                                 (string->symbol position)
                                 (λ ()
                                   (if first?
                                       (let* ([item (send hier-list new-item number-mixin)]
                                              [x (list (make-hash-table) hier-list item)])
                                         (hash-table-put! ht (string->symbol position) x)
                                         (send item set-number number)
                                         (send item set-allow-selection #f)
                                         (let* ([editor (send item get-editor)]
                                                [pos (send editor last-position)])
                                           (send editor insert "\n")
                                           (send editor insert position)
                                           (send editor change-style small-size-delta pos (+ pos 1))
                                           (send editor change-style section-style-delta 
                                                 (+ pos 1) (send editor last-position)))
                                         x)
                                       (let* ([new-list (send hier-list new-list 
                                                              (if second-number
                                                                  (compose second-number-mixin number-mixin)
                                                                  number-mixin))]
                                              [x (list (make-hash-table) new-list #f)])
                                         (send new-list set-number number)
                                         (when second-number
                                           (send new-list set-second-number second-number))
                                         (send new-list set-allow-selection #f)
                                         (send new-list open)
                                         (send (send new-list get-editor) insert position)
                                         (hash-table-put! ht (string->symbol position) x)
                                         x))))])
                          (cond
                            [first? 
                             (unless (= number (send (caddr sub-ht/sub-hier-list) get-number))
                               (error 'add-language "language ~s; expected number for ~e to be ~e, got ~e"
                                      (send language get-language-name)
                                      position
                                      (send (caddr sub-ht/sub-hier-list) get-number)
                                      number))]
                            [else
                             (unless (= number (send (cadr sub-ht/sub-hier-list) get-number))
                               (error 'add-language "language ~s; expected number for ~e to be ~e, got ~e"
                                      (send language get-language-name)
                                      position
                                      (send (cadr sub-ht/sub-hier-list) get-number)
                                      number))])
                          (add-sub-language (car sub-ht/sub-hier-list)
                                            (cadr sub-ht/sub-hier-list)
                                            (cdr positions)
                                            (cdr numbers)
                                            #f
                                            (if first? number #f)))]))))

          (define number<%>
            (interface ()
              get-number
              set-number))
          
          (define second-number<%>
            (interface ()
              get-second-number
              set-second-number))
          
          ;; number-mixin : (extends object%) -> (extends object%)
          ;; adds the get/set-number methods to this class
          (define (number-mixin %)
            (class* % (number<%>)
              (field (number 0))
              (define/public (get-number) number)
              (define/public (set-number _number) (set! number _number))
              (super-instantiate ())))

          ;; second-number-mixin : (extends object%) -> (extends object%)
          ;; adds the get/set-number methods to this class
          (define (second-number-mixin %)
            (class* % (second-number<%>)
              (field (second-number 0))
              (define/public (get-second-number) second-number)
              (define/public (set-second-number _second-number) (set! second-number _second-number))
              (super-instantiate ())))
              
          ;; make-details-panel : ((instanceof language<%>) -> (values panel (case-> (-> settings) (settings -> void))))
	  ;; adds a details panel for `language', using
	  ;; the language's default settings, unless this is
	  ;; the to-show language.
	  (define (make-details-panel language)
	    (let ([panel (instantiate vertical-panel% ()
                           (parent details-panel)
                           (stretchable-width #f)
                           (stretchable-height #f))])
	      (values
	       panel
	       (send language config-panel panel))))

	  ;; close-all-languages : -> void
	  ;; closes all of the tabs in the language hier-list.
	  (define (close-all-languages)
	    (define (close-children list)
	      (for-each close-this-one (send list get-items)))
	    (define (close-this-one item)
	      (cond
		[(is-a? item hierarchical-list-compound-item<%>)
		 (send item close)
		 (close-children item)]
		[else (void)]))
	    (close-children languages-hier-list))

	  ;; open-current-language : -> void
	  ;; opens the tabs that lead to the current language
	  ;; and selects the current language
	  (define (open-current-language)
	    (let loop ([hi languages-hier-list]
                       
                       ;; skip the first position, since it is flattened into the dialog
		       [first-pos (cadr (send language-to-show get-language-position))]
		       [position (cddr (send language-to-show get-language-position))])
		 (let ([child
			;; know that this `car' is okay by construction of the dialog
			(car 
			 (filter (λ (x)
				   (equal? (send (send x get-editor) get-text)
					   first-pos))
				 (send hi get-items)))])
		   (cond
		     [(null? position)
		      (send child select #t)]
		     [else
		      (send child open)
		      (loop child (car position) (cdr position))]))))

          ;; docs-callback : -> void
          (define (docs-callback)
            (void))
          
          ;; details-shown? : boolean
          ;; indicates if the details are currently visible in the dialog
          (define details-shown? (not (send language-to-show default-settings? settings-to-show)))

          ;; details-callback : -> void
          ;; flips the details-shown? flag and resets the GUI
          (define (details-callback)
            (set! details-shown? (not details-shown?))
            (when re-center
              (send re-center begin-container-sequence))
            (update-show/hide-details)
            (when re-center
              (send re-center center 'both)
              (send re-center end-container-sequence)))
          
          ;; show/hide-details : -> void
          ;; udpates the GUI based on the details-shown? flag
          (define (update-show/hide-details)
            (send details-button set-label 
                  (if details-shown? hide-details-label show-details-label))
            (send parent begin-container-sequence)
            (send revert-to-defaults-outer-panel change-children
                  (λ (l)
                    (if details-shown? (list revert-to-defaults-button) null)))
            (send details-outer-panel change-children
                  (λ (l)
                    (if details-shown? (list details/manual-parent-panel) null)))
            (send parent end-container-sequence))

          ;; revert-to-defaults-callback : -> void
          (define (revert-to-defaults-callback)
            (when selected-language
              (get/set-selected-language-settings 
               (send selected-language default-settings))))

          (define show-details-label (string-constant show-details-button-label))
          (define hide-details-label (string-constant hide-details-button-label))
          (define details-button (make-object button% 
                                   (if (show-details-label . system-font-space->= . hide-details-label)
                                       show-details-label
                                       hide-details-label)
                                   show-details-parent
                                   (λ (x y)
                                     (details-callback))))
          
          (define revert-to-defaults-outer-panel (make-object horizontal-panel% show-details-parent))
          (define revert-to-defaults-button (make-object button% 
                                              (string-constant revert-to-language-defaults)
                                              revert-to-defaults-outer-panel
                                              (λ (_1 _2)
                                                (revert-to-defaults-callback))))

          (send revert-to-defaults-outer-panel stretchable-width #f)
          (send revert-to-defaults-outer-panel stretchable-height #f)
          (send outermost-panel set-alignment 'center 'center)

          (update-show/hide-details)

          (for-each add-language-to-dialog languages)
          (send languages-hier-list sort 
                (λ (x y)
                  (cond
                    [(and (x . is-a? . second-number<%>)
                          (y . is-a? . second-number<%>))
                     (cond
                       [(= (send x get-second-number)
                           (send y get-second-number))
                        (< (send x get-number) (send y get-number))]
                       [else
                        (< (send x get-second-number)
                           (send y get-second-number))])]
                    [(and (x . is-a? . number<%>)
                          (y . is-a? . second-number<%>))
                     (cond
                       [(= (send x get-number)
                           (send y get-second-number))
                        #t]
                       [else
                        (< (send x get-number)
                           (send y get-second-number))])]
                    [(and (x . is-a? . second-number<%>)
                          (y . is-a? . number<%>))
                     (cond
                       [(= (send x get-second-number)
                           (send y get-number))
                        #f]
                       [else (< (send x get-second-number)
                                (send y get-number))])]
                    [(and (x . is-a? . number<%>)
                          (y . is-a? . number<%>))
                     (< (send x get-number) (send y get-number))]
                    [else #f])))

          ;; remove the newline at the front of the first inlined category
          (send (send (car (send languages-hier-list get-items)) get-editor) delete 0 1)
          
          (cond
            [manuals? 
             (unless details-shown?
               (details-callback))
             (send show-details-parent change-children
                   (λ (l)
                     (remq revert-to-defaults-outer-panel
                           (remq details-button l))))
             (send details/manual-parent-panel change-children 
                   (λ (l)
                     (list manual-ordering-panel)))]
            [else 
             (send details-outer-panel stretchable-width #f)
             (send details/manual-parent-panel change-children 
                   (λ (l)
                     (list details-panel)))])
          
          (send manual-ordering-text hide-caret #t)
          (send manual-ordering-text auto-wrap #t)
          (send manual-ordering-text lock #t)
          
	  (send languages-hier-list stretchable-width #t)
	  (send languages-hier-list stretchable-height #t)
	  (send parent reflow-container)
          (close-all-languages)
	  (open-current-language)
          (send languages-hier-list min-client-width (text-width (send languages-hier-list get-editor)))
	  (send languages-hier-list min-client-height (text-height (send languages-hier-list get-editor)))
          (when get/set-selected-language-settings
            (get/set-selected-language-settings settings-to-show))
          (send languages-hier-list focus)
          (values
           (λ () selected-language)
           (λ () 
             (and get/set-selected-language-settings
                  (get/set-selected-language-settings))))))

      (define panel-background-editor-canvas% 
        (class editor-canvas%
          (inherit get-dc get-client-size)
          (define/override (on-paint)
            (let-values ([(cw ch) (get-client-size)])
              (let* ([dc (get-dc)]
                     [old-pen (send dc get-pen)]
                     [old-brush (send dc get-brush)])
                (send dc set-brush (send the-brush-list find-or-create-brush (get-panel-background) 'panel))
                (send dc set-pen (send the-pen-list find-or-create-pen "black" 1 'transparent))
                (send dc draw-rectangle 0 0 cw ch)
                (send dc set-pen old-pen)
                (send dc set-brush old-brush)))
            (super on-paint))
          (super-new)))
      
      (define panel-background-text% 
        (class text%
          (define/override (on-paint before? dc left top right bottom dx dy draw-caret)
            (when before?
              (let ([old-pen (send dc get-pen)]
                    [old-brush (send dc get-brush)])
                (send dc set-brush (send the-brush-list find-or-create-brush (get-panel-background) 'panel))
                (send dc set-pen (send the-pen-list find-or-create-pen "black" 1 'transparent))
                (send dc draw-rectangle (+ dx left) (+ dy top) (- right left) (- bottom top))
                (send dc set-pen old-pen)
                (send dc set-brush old-brush)))
            (super on-paint before? dc left top right bottom dx dy draw-caret))
          (super-new)))
      
      (define section-style-delta (make-object style-delta% 'change-bold))
      (send section-style-delta set-delta-foreground "medium blue")
      (define small-size-delta (make-object style-delta% 'change-size 9))
      
      (define (add-welcome dialog welcome-before-panel welcome-after-panel)
        (let* ([outer-pb%
                (class pasteboard%
                  (define/override (can-interactive-move? evt)
                    #f)
                  (super-instantiate ()))]
               [outer-pb (make-object outer-pb%)]
               [bitmap 
                (make-object bitmap%
                  (build-path (collection-path "icons") 
                              "plt-small-shield.gif"))]
               [image-snip
                (make-object image-snip% 
                  (build-path (collection-path "icons") 
                              "plt-small-shield.gif"))]
               [before-text (make-object text%)]
               [before-snip (make-object editor-snip% before-text #f)]
               [before-ec%
                (class editor-canvas% 
                  (inherit get-client-size)
                  (define/private (update-size)
                    (let-values ([(cw ch) (get-client-size)])
                      (unless (or (zero? cw)
                                  (zero? ch))
                        (let ([image-l-box (box 0)]
                              [image-r-box (box 0)])
                          (send before-text get-snip-location image-snip image-l-box #f #f)
                          (send before-text get-snip-location image-snip image-r-box #f #t)
                          (let* ([image-w (send bitmap get-width)]
                                 [before-snip-space (- cw image-w)]
                                 [before-snip-w (- before-snip-space
                                                   5 5 ;; space before and after inside snip 
                                                   2   ;; space at end of outer editor
                                                   1   ;; space at beginning of outer editor
                                                   1   ;; space between image and snip
                                                   -5  ;; unknown space
                                                   )])
                            (send before-text set-max-width (max 0 before-snip-w)))))))
                  (define/override (on-superwindow-show shown?)
                    (update-size)
                    (super on-superwindow-show shown?))
                  (define/override (on-size w h)
                    (update-size)
                    (super on-size w h))
                  (super-instantiate ()))]
               [before-ec (instantiate before-ec% ()
                            (parent welcome-before-panel)
                            (editor outer-pb)
                            (stretchable-height #f)
                            (style '(no-vscroll no-hscroll)))]
               [first-line-style-delta (make-object style-delta% 'change-bold)])
          (send first-line-style-delta set-delta-foreground (make-object color% 150 0 150))
          (send before-ec min-width 550)

          (let-values ([(cw ch) (send before-ec get-client-size)]
                       [(w h) (send before-ec get-size)])
            (send before-ec min-height 
                  (+ (send bitmap get-height) 
                     8  ;; pasteboards apparently want some space here....
                     (- h ch))))

          (send outer-pb insert image-snip)
          (send outer-pb insert before-snip)
          (send outer-pb move image-snip 0 0)
          (send outer-pb move before-snip (send bitmap get-width) 0)
          (send outer-pb set-selection-visible #f)
          (send outer-pb lock #t)

          ;(send before-snip set-align-top-line #t)
          (send before-text insert 
                (format (string-constant welcome-to-drscheme-version/language)
                        (version:version)
                        (this-language)))
          (send before-text insert #\newline)
          (send before-text insert (string-constant introduction-to-language-dialog))
          (send before-text change-style 
                first-line-style-delta
                0
                (send before-text paragraph-end-position 0))
          (send before-text auto-wrap #t)

          (send before-text lock #t)
          (send before-text hide-caret #t)

          (for-each (λ (native-lang-string language)
                      (unless (equal? (this-language) language)
                        (instantiate button% ()
                          (label native-lang-string)
                          (parent welcome-after-panel)
                          (stretchable-width #t)
                          (callback (λ (x1 x2) (drscheme:app:switch-language-to dialog language))))))
                    (string-constants is-this-your-native-language)
                    (all-languages))))

      ;; system-font-space->= : string string -> boolean
      ;; determines which string is wider, when drawn in the system font
      (define (x . system-font-space->= . y)
        (let ([bdc (make-object bitmap-dc%)])
          (send bdc set-bitmap (make-object bitmap% 1 1 #t))
          (send bdc set-font (send the-font-list find-or-create-font
                                   12 'system 'normal 'normal))
          (let-values ([(wx _1 _2 _3) (send bdc get-text-extent x)]
                       [(wy _4 _5 _6) (send bdc get-text-extent y)])
            (wx . >= . wy))))

      ;; text-width : (isntanceof text%) -> exact-integer
      ;; calculates the width of widest line in the
      ;; editor. This only makes sense if auto-wrap
      ;; is turned off. Otherwise, you could just use
      ;; the admin's width.
      (define (text-width text)
	(let loop ([n (+ (send text last-line) 1)]
		   [current-max-width 0])
	  (cond
	    [(zero? n)
	     (+
	      10 ;; this should be some magic small constant (hopefully less than 10 on all platforms)
	      (floor (inexact->exact current-max-width)))]
	    [else (let* ([line-number (- n 1)]
			 [box (box 0.0)]
			 [eol-pos (send text line-end-position line-number)]
			 [eol-snip (send text find-snip eol-pos 'before)])
		    (when eol-snip
		      (send text get-snip-location eol-snip box #f #t))
		    (loop (- n 1)
			  (max current-max-width (unbox box))))])))
      
      ;; text-height : (is-a?/c text% -> exact-integer
      (define (text-height text)
        (let ([y-box (box 0)])
          (send text position-location 
                (send text last-position)
                #f
                y-box
                #f
                #f
                #t)
          (+ 10 ;; upper bound on some platform specific space I don't know how to get.
             (floor (inexact->exact (unbox y-box))))))

      (define teachpack-directory 
        (let ([lib-dir (build-path 
                        (collection-path "mzlib")
                        'up 'up "teachpack")])
          (if (directory-exists? lib-dir)
              lib-dir
              #f)))

      
                                                                      
                            ;;                          ;;            
  ;                          ;                           ;            
  ;                          ;                           ;            
 ;;;;;   ;;;   ;;;;    ;;;   ; ;;  ; ;;;   ;;;;    ;;;   ;  ;;   ;;;  
  ;     ;   ;      ;  ;   ;  ;;  ;  ;   ;      ;  ;   ;  ; ;    ;   ; 
  ;     ;;;;;   ;;;;  ;      ;   ;  ;   ;   ;;;;  ;      ;;      ;;;  
  ;     ;      ;   ;  ;      ;   ;  ;   ;  ;   ;  ;      ; ;        ; 
  ;   ; ;   ;  ;   ;  ;   ;  ;   ;  ;   ;  ;   ;  ;   ;  ;  ;   ;   ; 
   ;;;   ;;;    ;;; ;  ;;;  ;;; ;;; ;;;;    ;;; ;  ;;;  ;;   ;;  ;;;  
                                    ;                                 
                                    ;                                 
                                   ;;;                                

      
      ;; add-new-teachpack : (instanceof frame%) -> boolean
      ;; querys the user for the name of a teachpack and adds it to the
      ;; current teachpacks. Uses the argument as the parent to the dialog
      ;; the result indicates if the teachpacks changed #t if they did and #f if not
      (define (add-new-teachpack frame)
        (let ([lib-file
               (begin
                 (parameterize ([finder:dialog-parent-parameter frame])
                   (finder:get-file 
                    teachpack-directory
                    (string-constant select-a-teachpack)
                    ".*\\.(ss|scm)$")))])
          (if lib-file
              (let* ([interactions-text (send frame get-interactions-text)]
                     [tp-cache (send interactions-text get-user-teachpack-cache)]
                     [tp-filenames (drscheme:teachpack:teachpack-cache-filenames tp-cache)]
                     [new-item (normalize-path lib-file)])
                (cond
                  [(member (normal-case-path new-item) (map normal-case-path tp-filenames))
                   (message-box (string-constant drscheme-teachpack-message-title)
                                (format (string-constant already-added-teachpack)
                                        new-item)
                                frame)]
                  [else
                   (let ([new-teachpacks 
                          (drscheme:teachpack:new-teachpack-cache
                           (append tp-filenames (list new-item)))])
                     (send interactions-text set-user-teachpack-cache new-teachpacks)
                     (preferences:set 'drscheme:teachpacks new-teachpacks))])
                (set! teachpack-directory (path-only lib-file))
                #t)
              #f)))
      
      ;; clear-all-teachpacks : -> boolean
      ;; clears all of the teachpack settings
      ;; the result indicates if the teachpacks changed #t if they did and #f if not
      (define (clear-all-teachpacks)
        (let ([old (preferences:get 'drscheme:teachpacks)])
          (cond
            [(null? (drscheme:teachpack:teachpack-cache-filenames old))
             #f]
            [else
             (drscheme:teachpack:set-teachpack-cache-filenames! old null)
             (preferences:set 'drscheme:teachpacks old)
             #t])))

   ;             ;;;                             
                ;                                
                ;                                
 ;;;   ; ;;;   ;;;;;   ;;;           ;;;    ;;;  
   ;    ;;  ;   ;     ;   ;         ;   ;  ;   ; 
   ;    ;   ;   ;     ;   ;          ;;;    ;;;  
   ;    ;   ;   ;     ;   ;             ;      ; 
   ;    ;   ;   ;     ;   ;         ;   ;  ;   ; 
 ;;;;; ;;;  ;; ;;;;    ;;;    ;      ;;;    ;;;  
                                                 
                                                 
                                                 

                                                               
 ;;;                                                           
   ;                                                           
   ;                                                           
   ;    ;;;;  ; ;;;    ;;; ;;;  ;;  ;;;;    ;;; ;  ;;;    ;;;  
   ;        ;  ;;  ;  ;   ;  ;   ;      ;  ;   ;  ;   ;  ;   ; 
   ;     ;;;;  ;   ;  ;   ;  ;   ;   ;;;;  ;   ;  ;;;;;   ;;;  
   ;    ;   ;  ;   ;  ;   ;  ;   ;  ;   ;  ;   ;  ;          ; 
   ;    ;   ;  ;   ;  ;   ;  ;   ;  ;   ;  ;   ;  ;   ;  ;   ; 
 ;;;;;;  ;;; ;;;;  ;;  ;;;;   ;;; ;  ;;; ;  ;;;;   ;;;    ;;;  
                          ;                    ;               
                          ;                    ;               
                       ;;;                  ;;;                

      
      (define (add-info-specified-languages)
        (for-each add-info-specified-language (find-relevant-directories '(drscheme-language-positions))))
      
      (define (add-info-specified-language directory)
        (let ([info-proc (get-info/full directory)])
          (when info-proc
            (let* ([lang-positions (info-proc 'drscheme-language-positions (λ () null))]
                   [lang-modules (info-proc 'drscheme-language-modules (λ () null))]
                   [numberss (info-proc 'drscheme-language-numbers 
                                        (λ ()
                                          (map (λ (lang-position)
                                                 (map (λ (x) 0) lang-position))
                                               lang-positions)))]
                   [summaries (info-proc 'drscheme-language-one-line-summaries 
                                         (λ ()
                                           (map (λ (lang-position) "")
                                                lang-positions)))]
                   [urls      (info-proc 'drscheme-language-urls 
                                         (λ ()
                                           (map (λ (lang-position) "")
                                                lang-positions)))]
                   [reader-specs
                    (info-proc 'drscheme-language-readers
                               (λ ()
                                 (map (λ (lang-position) #f)
                                      lang-positions)))])
              (cond
                [(and (list? lang-positions)
                      (andmap (λ (lang-position numbers)
                                (and (list? lang-position)
                                     (pair? lang-position)
                                     (andmap string? lang-position)
                                     (list? numbers)
                                     (andmap number? numbers)
                                     (= (length numbers)
                                        (length lang-position))))
                              lang-positions
                              numberss)
                      (list? lang-modules)
                      (andmap (λ (x)
                                (and (list? x)
                                     (andmap string? x)))
                              lang-modules)
                      (list? summaries)
                      (andmap string? summaries)

                      (list? urls)
                      (andmap string? urls)
                      
                      (list? reader-specs)
                      (andmap (λ (x)
				;; approximation (no good test, really)
				;; since it depends on the value of a mz
				;; parameter to interpret the module spec
                                (or (eq? x #f) (symbol? x) (pair? x)))
                              reader-specs)
                      
                      (= (length lang-positions)
                         (length lang-modules)
                         (length summaries)
                         (length urls)
                         (length reader-specs)))
                 (for-each
                  (λ (lang-module lang-position lang-numbers one-line-summary url reader-spec)
                    (let ([%
                           ((drscheme:language:get-default-mixin)
                            (drscheme:language:module-based-language->language-mixin
                             (drscheme:language:simple-module-based-language->module-based-language-mixin
                              drscheme:language:simple-module-based-language%)))]
                          [reader
                           (if reader-spec
                               (with-handlers ([exn:fail?
                                                (λ (x)
                                                  (message-box (string-constant drscheme)
                                                               (if (exn? x)
                                                                   (exn-message x)
                                                                   (format "uncaught exception: ~s" x)))
                                                  read-syntax/namespace-introduce)])
                                 (contract
                                  (opt-> (any/c)
                                         (port? (list/c (and/c number? integer? exact? (>=/c 0))
                                                        (and/c number? integer? exact? (>=/c 0))
                                                        (and/c number? integer? exact? (>=/c 0))))
                                         (union syntax? eof-object?))
                                  (dynamic-require reader-spec 'read-syntax)
                                  (string->symbol (format "~s" lang-position))
                                  'drscheme))
                               read-syntax/namespace-introduce)])
                      (add-language (instantiate % ()
                                      (module `(lib ,@lang-module))
                                      (language-position lang-position)
                                      (language-numbers lang-numbers)
                                      (one-line-summary one-line-summary)
                                      (language-url url)
                                      (reader reader)))))
                  lang-modules
                  lang-positions
                  numberss
                  summaries
                  urls
                  reader-specs)]
                [else
                 (message-box
		  (string-constant drscheme)
		  (format
		   "The drscheme-language-position, drscheme-language-modules, drscheme-language-numbers, and drscheme-language-readers specifications aren't correct. Expected (listof (cons string (listof string))), (listof (listof string)), (listof (listof number)), (listof string), (listof string), and (listof module-spec) respectively, where the lengths of the outer lists are the same. Got ~e, ~e, ~e, ~e, ~e, and ~e"
		  lang-positions
		  lang-modules
		  numberss
		  summaries
                  urls
		  reader-specs))])))))

      (define read-syntax/namespace-introduce
        (opt-lambda (source-name-v [input-port (current-input-port)])
          (let ([v (read-syntax source-name-v input-port)])
            (if (syntax? v)
                (namespace-syntax-introduce v)
                v))))
      

                                                        
;;               ;    ;;;                    ;          
 ;                      ;     ;                         
 ;                      ;     ;                         
 ;;;;  ;;  ;;  ;;;      ;    ;;;;;         ;;;   ; ;;;  
 ;   ;  ;   ;    ;      ;     ;              ;    ;;  ; 
 ;   ;  ;   ;    ;      ;     ;              ;    ;   ; 
 ;   ;  ;   ;    ;      ;     ;              ;    ;   ; 
 ;   ;  ;   ;    ;      ;     ;   ;          ;    ;   ; 
; ;;;    ;;; ; ;;;;;  ;;;;;;   ;;;         ;;;;; ;;;  ;;
                                                        
                                                        
                                                        

                                                               
 ;;;                                                           
   ;                                                           
   ;                                                           
   ;    ;;;;  ; ;;;    ;;; ;;;  ;;  ;;;;    ;;; ;  ;;;    ;;;  
   ;        ;  ;;  ;  ;   ;  ;   ;      ;  ;   ;  ;   ;  ;   ; 
   ;     ;;;;  ;   ;  ;   ;  ;   ;   ;;;;  ;   ;  ;;;;;   ;;;  
   ;    ;   ;  ;   ;  ;   ;  ;   ;  ;   ;  ;   ;  ;          ; 
   ;    ;   ;  ;   ;  ;   ;  ;   ;  ;   ;  ;   ;  ;   ;  ;   ; 
 ;;;;;;  ;;; ;;;;  ;;  ;;;;   ;;; ;  ;;; ;  ;;;;   ;;;    ;;;  
                          ;                    ;               
                          ;                    ;               
                       ;;;                  ;;;                


      
      ;; add-expand-to-front-end : mixin
      ;; overrides front-end to make the language a language that expands its arguments
      (define (add-expand-to-front-end %)
        (class %
          (define/override (front-end/complete-program input settings teachpack-cache)
            (wrap-front-end (super front-end/complete-program input settings teachpack-cache)))
          (define/override (front-end/interaction input settings teachpack-cache)
            (wrap-front-end (super front-end/interaction input settings teachpack-cache)))
          (define/private (wrap-front-end thnk)
            (λ ()
              (let ([res (thnk)])
                (cond
                  [(syntax? res) (with-syntax ([res res]
                                               [expand-syntax-top-level-with-compile-time-evals
                                                expand-syntax-top-level-with-compile-time-evals])
                                   #'(expand-syntax-top-level-with-compile-time-evals
                                      (quote-syntax res)))]
                  [(eof-object? res) res]
                  [else `(expand ',res)]))))
          (super-instantiate ())))

      (define (r5rs-mixin %)
        (class %
          (define/override (on-execute setting run-in-user-thread)
            (super on-execute setting run-in-user-thread)
            (run-in-user-thread
             (λ ()
               (read-square-bracket-as-paren #f)
               (read-curly-brace-as-paren #f)
               (print-vector-length #f))))
          (define/override (default-settings) 
            (drscheme:language:make-simple-settings #f 'write 'mixed-fraction-e #f #t 'debug))
          (define/override (order-manuals x)
            (values 
             (list #"r5rs" #"drscheme" #"tour" #"help")
             #f))
          (super-new)))
      
      ;; add-built-in-languages : -> void
      (define (add-built-in-languages)
        (let* ([extras-mixin
                (λ (mred-launcher? one-line-summary)
                  (λ (%)
                    (class %
                      (define/override (get-one-line-summary) one-line-summary)
                      (define/override (use-namespace-require/copy?) #t)
                      
                      (inherit get-module get-transformer-module get-init-code)
                      (define/override (create-executable setting parent program-filename teachpacks)
                        (let ([executable-fn
			       (drscheme:language:put-executable
				parent
				program-filename
				mred-launcher?
				#t
				(if mred-launcher?
				    (string-constant save-a-mred-launcher)
				    (string-constant save-a-mzscheme-launcher)))])
                          (when executable-fn
                            (drscheme:language:create-module-based-launcher
                             program-filename
                             executable-fn
                             (get-module)
                             (get-transformer-module)
                             (get-init-code setting teachpacks)
                             mred-launcher?
                             (use-namespace-require/copy?)))))
                      (super-instantiate ()))))]
               [make-simple
                (λ (module position numbers mred-launcher? one-line-summary extra-mixin)
                  (let ([%
                         (extra-mixin
                          ((extras-mixin mred-launcher? one-line-summary)
                           ((drscheme:language:get-default-mixin)
                            (drscheme:language:module-based-language->language-mixin
                             (drscheme:language:simple-module-based-language->module-based-language-mixin
                              drscheme:language:simple-module-based-language%)))))])
                    (instantiate % ()
                      (module module)
                      (language-position position)
                      (language-numbers numbers))))])
          (add-language
           (make-simple '(lib "plt-mzscheme.ss" "lang") 
                        (list (string-constant professional-languages)
                              (string-constant plt)
                              (string-constant mzscheme-w/debug))
                        (list -1000 -10 1)
                        #f
                        (string-constant mzscheme-one-line-summary)
                        (λ (x) x)))
          (add-language
           (make-simple '(lib "plt-mred.ss" "lang")
                        (list (string-constant professional-languages)
                              (string-constant plt)
                              (string-constant mred-w/debug))
                        (list -1000 -10 2)
                        #t
                        (string-constant mred-one-line-summary)
                        (λ (x) x)))
          (add-language
           (make-simple '(lib "plt-pretty-big.ss" "lang")
                        (list (string-constant professional-languages)
                              (string-constant plt)
                              (string-constant pretty-big-scheme))
                        (list -1000 -10 3)
                        #t
                        (string-constant pretty-big-scheme-one-line-summary)
                        (λ (x) x)))
          (add-language
           (make-simple '(lib "plt-mzscheme.ss" "lang")
                        (list (string-constant professional-languages)
                              (string-constant plt)
                              (string-constant expander))
                        (list -1000 -10 4)
                        #t
                        (string-constant expander-one-line-summary)
                        add-expand-to-front-end))
          (add-language
           (make-simple '(lib "r5rs.ss" "lang")
                        (list (string-constant professional-languages)
                              (string-constant r5rs-lang-name))
                        (list -1000 -1000)
                        #f
                        (string-constant r5rs-one-line-summary)
                        r5rs-mixin)))))))
