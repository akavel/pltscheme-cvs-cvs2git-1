(module mztext-run mzscheme

(require (lib "mztext.ss" "preprocessor")
         (lib "pp-run.ss" "preprocessor")
         (lib "cmdline.ss"))

(let ([output #f] [run-cmd #f])
  (command-line
   "mztext"
   (current-command-line-arguments)
   (once-each
    [("-o" "--output") file "output file (defaults to standard output)"
     (set! output file)]
    [("-c" "--command-marker") marker "command marker (defaults to \"@\")"
     (command-marker marker)]
    [("--run") cmd "run the command string on a single preprocessed input file"
     (set! run-cmd cmd)])
   (help-labels "   (see the documentation for this option)")
   (=> (lambda (_ . files) (run preprocess run-cmd output files))
       '("input-file")
       (more-help 'mztext "a MzScheme-based preprocessing language"))))

)