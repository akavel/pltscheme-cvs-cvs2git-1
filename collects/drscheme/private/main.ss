
(module main mzscheme
  (require (lib "string-constant.ss" "string-constants")
           (lib "unitsig.ss")
           "drsig.ss"
	   (lib "mred.ss" "mred")
           (lib "framework.ss" "framework")
           (lib "unitsig.ss")
           (lib "class.ss")
           (prefix pretty-print: (lib "pretty.ss"))
           (prefix print-convert: (lib "pconvert.ss"))
	   (lib "include.ss")
           (lib "list.ss")
           (lib "file.ss")
           (lib "external.ss" "browser")
           (lib "plt-installer.ss" "setup"))
  
  (provide main@)
  (define argv (current-command-line-arguments))
  
  (define main@
    (unit/sig ()
      (import [drscheme:app : drscheme:app^]
              [drscheme:unit : drscheme:unit^]
              [drscheme:get/extend : drscheme:get/extend^]
              [drscheme:language-configuration : drscheme:language-configuration/internal^]
	      [drscheme:language : drscheme:language^]
              [drscheme:teachpack : drscheme:teachpack^]
              [drscheme:module-language : drscheme:module-language^]
              [drscheme:tools : drscheme:tools^]
              [drscheme:debug : drscheme:debug^]
              [drscheme:frame : drscheme:frame^]
              [drscheme:font : drscheme:font^]
              [drscheme:modes : drscheme:modes^])

      (application-file-handler
       (let ([default (application-file-handler)])
         (λ (name)
           (if (null? (get-top-level-windows))
               (handler:edit-file name)
               (default name)))))
      
      (application-quit-handler
       (let ([default (application-quit-handler)])
         (λ ()
           (if (null? (get-top-level-windows))
               (when (exit:user-oks-exit)
                 (exit:exit))
               (default)))))
      
      (application-about-handler
       (λ ()
         (drscheme:app:about-drscheme)))

      (drscheme:modes:add-initial-modes)
      
      (namespace-set-variable-value! 'help-desk:frame-mixin drscheme:frame:basics-mixin)
      
      (finder:default-filters (list* '("Scheme (.scm)" "*.scm")
                                     '("Scheme (.ss)" "*.ss")
                                     (finder:default-filters)))
      (application:current-app-name (string-constant drscheme))
      
      (preferences:set-default 'drscheme:show-interactions-on-execute #t boolean?)
      (preferences:set-default 'drscheme:open-in-tabs #f boolean?)
      (preferences:set-default 'drscheme:toolbar-shown #t boolean?)
      (preferences:set-default 'drscheme:user-defined-keybindings '() (λ (x) (and (list? x) (andmap path? x))))
      (preferences:set-un/marshall 
       'drscheme:user-defined-keybindings
       (λ (in) (map path->bytes in))
       (λ (ex) (if (and (list? ex)
                             (andmap bytes? ex))
                        (map bytes->path ex)
                        '())))
      
      (let ([number-between-zero-and-one?
             (λ (x) (and (number? x) (<= 0 x 1)))])
        (preferences:set-default 'drscheme:unit-window-size-percentage 
                                 1/2 
                                 number-between-zero-and-one?)
        (preferences:set-default 'drscheme:module-browser-size-percentage
                                 1/5
                                 number-between-zero-and-one?))
      (preferences:set-default 'drscheme:module-browser:name-length 1 
                               (λ (x) (memq x '(0 1 2))))
      
      (let ([frame-width 600]
            [frame-height 650]
            [window-trimming-upper-bound-width 20]
            [window-trimming-upper-bound-height 50])
        (let-values ([(w h) (get-display-size)])
          (set! frame-width (min frame-width (- w window-trimming-upper-bound-width)))
          (set! frame-height (min frame-height (- h window-trimming-upper-bound-height))))
        (preferences:set-default 'drscheme:unit-window-width frame-width number?)
        (preferences:set-default 'drscheme:unit-window-height frame-height number?))
      
      (preferences:set-default 'drscheme:backtrace-window-width 400 number?)
      (preferences:set-default 'drscheme:backtrace-window-height 300 number?)
      (preferences:set-default 'drscheme:backtrace-window-x 0 number?)
      (preferences:set-default 'drscheme:backtrace-window-y 0 number?)

      (preferences:set-default 'drscheme:profile-how-to-count 'time
                               (λ (x)
                                 (memq x '(time count))))
      (preferences:set-default 'drscheme:profile:low-color
                               (make-object color% 150 255 150)
                               (λ (x) (is-a? x color%)))
      (preferences:set-default 'drscheme:profile:high-color
                               (make-object color% 255 150 150)
                               (λ (x) (is-a? x color%)))
      (preferences:set-default 'drscheme:profile:scale
                               'linear
                               (λ (x) (memq x '(sqrt linear square))))

      (preferences:set-default 'drscheme:test-coverage-ask-about-clearing? #t boolean?)

      ;; size is in editor positions
      (preferences:set-default 'drscheme:repl-buffer-size 
                               '(#t . 1000)
                               (λ (x)
                                 (and (pair? x)
                                      (boolean? (car x))
                                      (integer? (cdr x))
                                      (<= 1 (cdr x) 10000))))
      
      (let ([marshall-color 
             (λ (c)
               (list (send c red) (send c green) (send c blue)))]
            [unmarshall-color
             (λ (l)
               (if (and (list? l) 
                        (= 3 (length l))
                        (andmap (λ (x) (and number? (<= 0 x 255)))
                                l))
                   (make-object color% (car l) (cadr l) (caddr l))
                   (make-object color% 0 0 0)))])
        (preferences:set-un/marshall 
         'drscheme:profile:low-color
         marshall-color
         unmarshall-color)
        (preferences:set-un/marshall 
         'drscheme:profile:high-color
         marshall-color
         unmarshall-color))
      
      (preferences:set-default 
       'drscheme:keybindings-window-size
       (cons 200 400)
       (λ (x) (and (pair? x)
                        (number? (car x))
                        (number? (cdr x)))))
      
      (preferences:set-default
       'drscheme:execute-warning-once
       #f
       (λ (x)
         (or (eq? x #t)
             (not x))))
      
      (preferences:set-default
       'drscheme:teachpacks
       (drscheme:teachpack:new-teachpack-cache) 
       drscheme:teachpack:teachpack-cache?)
      (preferences:set-un/marshall
       'drscheme:teachpacks
       drscheme:teachpack:marshall-teachpack-cache
       drscheme:teachpack:unmarshall-teachpack-cache)
      
      (drscheme:font:setup-preferences)
      (color-prefs:add-background-preferences-panel)
      (scheme:add-preferences-panel)
      (scheme:add-coloring-preferences-panel)
      (preferences:add-editor-checkbox-panel)
      (preferences:add-warnings-checkbox-panel)
      (preferences:add-scheme-checkbox-panel)
      
      (let ([make-check-box
             (λ (pref-sym string parent)
               (let ([q (make-object check-box%
                          string
                          parent
                          (λ (checkbox evt)
                            (preferences:set 
                             pref-sym 
                             (send checkbox get-value))))])
                 (preferences:add-callback pref-sym (λ (p v) (send q set-value v)))
                 (send q set-value (preferences:get pref-sym))))])
        (preferences:add-to-editor-checkbox-panel
         (λ (editor-panel)
           (make-check-box 'drscheme:open-in-tabs 
                           (string-constant open-files-in-tabs)
                           editor-panel)
           (make-check-box 'drscheme:show-interactions-on-execute 
                           (string-constant show-interactions-on-execute)
                           editor-panel)

           ;; come back to this one.
           #;
           (letrec ([hp (new horizontal-panel% 
                             (parent editor-panel)
                             (alignment '(left top))
                             (stretchable-height #f))]
                    [cb (new check-box%
                             (label (string-constant limit-interactions-size))
                             (parent hp)
                             (callback (λ (cb v) (cb-callback))))]
                    [sl (new slider% 
                             (label #f)
                             (parent hp)
                             (min-value 1)
                             (max-value 10000)
                             (callback
                              (λ (sl _) (sl-callback))))]
                    [cb-callback
                     (λ ()
                       (preferences:set 'drscheme:repl-buffer-size
                                        (cons (send cb get-value)
                                              (cdr (preferences:get 'drscheme:repl-buffer-size)))))]
                    [sl-callback
                     (λ ()
                       (preferences:set 'drscheme:repl-buffer-size
                                        (cons (car (preferences:get 'drscheme:repl-buffer-size))
                                              (send sl get-value))))]
                    [update-controls
                     (λ (v)
                       (let ([on? (car v)])
                         (send sl enable on?)
                         (send cb set-value on?)
                         (send sl set-value (cdr v))))])
             (preferences:add-callback 'drscheme:repl-buffer-size (λ (p v) (update-controls v)))
             (update-controls (preferences:get 'drscheme:repl-buffer-size)))))
        
        (preferences:add-to-warnings-checkbox-panel
         (λ (warnings-panel)
           (make-check-box 'drscheme:execute-warning-once 
                           (string-constant only-warn-once)
                           warnings-panel)
           (make-check-box 'drscheme:test-coverage-ask-about-clearing?
                           (string-constant test-coverage-ask?)
                           warnings-panel))))
      (drscheme:debug:add-prefs-panel)
      (install-help-browser-preference-panel)

      (handler:current-create-new-window
       (let ([drscheme-current-create-new-window
	      (λ (filename)
		(drscheme:unit:open-drscheme-window filename))])
	 drscheme-current-create-new-window))

      ;; add a handler to open .plt files.
      (handler:insert-format-handler 
       "PLT Files"
       (λ (filename)
         (and (equal? "plt" (filename-extension filename))
              (gui-utils:get-choice 
               (format (string-constant install-plt-file) filename)
               (string-constant install-plt-file/yes)
               (string-constant install-plt-file/no))))
       (λ (filename)
         (run-installer filename)
	 #f))
      
      (drscheme:tools:load/invoke-all-tools
       (λ ()
         (void))
       (λ ()
         (drscheme:language-configuration:add-built-in-languages)
         (drscheme:module-language:add-module-language)
         (drscheme:language-configuration:add-info-specified-languages)))
      
      ;; no more extension after this point
      (drscheme:get/extend:get-interactions-canvas)
      (drscheme:get/extend:get-definitions-canvas)
      (drscheme:get/extend:get-unit-frame)
      (drscheme:get/extend:get-interactions-text)
      (drscheme:get/extend:get-definitions-text)
      (drscheme:language-configuration:get-languages)
      
      ;; this default can only be set *after* the
      ;; languages have all be registered by tools
      (preferences:set-default
       drscheme:language-configuration:settings-preferences-symbol
       (drscheme:language-configuration:get-default-language-settings)
       drscheme:language-configuration:language-settings?)
      
      ;; if the unmarshaller returns #f, that will fail the
      ;; test for this preference, reverting back to the default.
      ;; In that case, the default is specified in the pref.ss file
      ;; of the default collection and may not be the default
      ;; specified below.
      (preferences:set-un/marshall
       drscheme:language-configuration:settings-preferences-symbol
       (λ (x)
	 (let ([lang (drscheme:language-configuration:language-settings-language x)]
	       [settings (drscheme:language-configuration:language-settings-settings x)])
	   (list (send lang get-language-position)
		 (send lang marshall-settings settings))))
       (λ (x)
	 (and (list? x)
	      (= 2 (length x))
	      (let* ([lang-position (first x)]
		     [marshalled-settings (second x)]
		     [lang (ormap
			    (λ (x)
			      (and (equal? lang-position
					   (send x get-language-position))
				   x))
			    (drscheme:language-configuration:get-languages))])
		(and lang
		     (let ([settings (send lang unmarshall-settings marshalled-settings)])
		       (drscheme:language-configuration:make-language-settings
			lang
			(or settings (send lang default-settings)))))))))
      
      (let ([drs-handler-recent-items-super%
             (class (drscheme:frame:basics-mixin
                     (frame:standard-menus-mixin
                      frame:basic%))
               (define/override (edit-menu:between-select-all-and-find menu)
                 (void))
               (super-new))])
      (handler:set-recent-items-frame-superclass drs-handler-recent-items-super%))
      
      ;;
      ;; Show expanded language dialog when version changes
      ;; 
      (preferences:set-default 'drscheme:last-version #f (λ (x) (or (string? x) (not x))))
      (preferences:set-default 'drscheme:last-language #f (λ (x) (or (symbol? x) (not x))))

      (cond
        [(current-eventspace-has-menu-root?)
         (drscheme:frame:create-root-menubar)
         (preferences:set 'framework:exit-when-no-frames #f)]
        [else
         (preferences:set 'framework:exit-when-no-frames #t)])
      
      
      (drscheme:app:check-new-version)
      
      ;;
      ;; Check for any files lost last time.
      ;; Ignore the framework's empty frames test, since
      ;;   the autosave information window may appear and then
      ;;   go away (leaving no frames temporarily) but we are
      ;;   not going to be exiting yet.
      (autosave:restore-autosave-files/gui)
     
      ;; install user's keybindings
      (for-each keymap:add-user-keybindings-file (preferences:get 'drscheme:user-defined-keybindings))
      
      ;; the initial window doesn't set the 
      ;; unit object's state correctly, yet.
      (define (make-basic)
	(let* ([frame (drscheme:unit:open-drscheme-window)]
	       [interactions-edit (send frame get-interactions-text)]
	       [definitions-edit (send frame get-interactions-text)]
	       [filename (send definitions-edit get-filename)])
	  (unless filename
	    (send frame update-shown)
	    (send (send frame get-interactions-canvas) focus))
          (send frame show #t)))
      
      (define (remove-duplicates files)
        (let loop ([files files])
          (cond
            [(null? files) null]
            [else (if (member (car files) (cdr files))
                      (loop (cdr files))
                      (cons (car files) (loop (cdr files))))])))
      
      (let* ([files-to-open (reverse (vector->list argv))]
             [normalized/filtered
              (let loop ([files files-to-open])
                (cond
                  [(null? files) null]
                  [else (let ([file (car files)])
                          (if (file-exists? file)
                              (cons (normalize-path file) (loop (cdr files)))
                              (begin
                                (message-box
                                 (string-constant drscheme)
                                 (format (string-constant cannot-open-because-dne) file))
                                (loop (cdr files)))))]))]
             [no-dups (remove-duplicates normalized/filtered)]
	     [frames
	      (map (λ (f) (handler:edit-file
				f
				(λ () (drscheme:unit:open-drscheme-window f))))
		   no-dups)])
	(when (null? (filter (λ (x) x) frames))
	  (make-basic))))))
