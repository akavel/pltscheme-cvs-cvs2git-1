#!/bin/sh
#|
exec mzscheme -r "$0" "$@"
|#

;; Automated tests of the use cases. These are meant to be run as a single
;; client, once, against a testing backend. This tests that the servlet has the
;; functionality we want for a single user, once.

(require (lib "text-ui.ss" "schemeunit")
         (lib "etc.ss")
         (lib "class.ss")
         (lib "match.ss")
         (lib "test.ss" "schemeunit")
         (lib "send-assertions.ss" "web-server" "tools")

         "create-data.ss"
         "../src/pages-transitions.ss"
         (prefix backend: "../src/backend.ss")
         )

(define (the-servlet)
  (transition-login #f)) ;;; #f is usually req

(define test-servlet
  (make-test-suite
    "Test all the use cases"

    (make-test-case
      "A user enters an invalid username and/or password"
      (assert-output-response/suspended
        the-servlet
        (list (list form->k-url
                    (list (cons 'username "student one")
                          (cons 'password "The Wrong Password"))))
        (login-page "Invalid username or password.")))

    (make-test-case
      "A user logs in, then logs out"
      (assert-output-response/suspended
        the-servlet
        (list (list form->k-url
                    (list (cons 'username "student one")
                          (cons 'password "password")))
              (list (hyperlink->k-url "Logout") '()))
        (login-page)))

    (make-test-case
      "A user logs in, then closes the Web browser"
      (assert-output-response/suspended
        the-servlet
        (list (list form->k-url
                    (list (cons 'username "student one")
                          (cons 'password "password"))))
        logged-in-page))

    (make-test-case
      (string-append
        "A user logs in, logs out, goes back to the logged-in page, and "
        "logs out again")
      (assert-output-response/suspended
        the-servlet
        (list (list form->k-url
                    (list (cons 'username "student one")
                          (cons 'password "password")))
              (list (hyperlink->k-url "Logout") '())
              'back
              'forward)
        restart-session-page))

    (make-test-case
      (string-append
        "A user logs in, sucessfully changes his or her password, then logs "
        "out")
      (assert-output-response/suspended
        the-servlet
        (list (list form->k-url
                    (list (cons 'username "student one")
                          (cons 'password "password")))
              (list (hyperlink->k-url "Change Password") '())
              (list form->k-url
                    (list (cons 'old-password "password")
                          (cons 'new-password1 "The New Password")
                          (cons 'new-password2 "The New Password")))
              (list (hyperlink->k-url "Logout") '()))
        (login-page))
      (void)
      (backend:update-password! "student one" "password"))

    (make-test-case
      (string-append
        "A user logs in, attempts to change his or her password, enters an "
        "invalid old password, then enters mismatched new passwords, then "
        "sucessfully changes his or her password, then logs out")
      (assert-output-response/suspended
        the-servlet
        (list (list form->k-url
                    (list (cons 'username "student one")
                          (cons 'password "password")))
              (list (hyperlink->k-url "Change Password") '())
              (list form->k-url
                    (list (cons 'old-password "The Wrong Password")
                          (cons 'new-password1 "The New Password")
                          (cons 'new-password2 "The New Password")))
              (list form->k-url
                    (list (cons 'old-password "password")
                          (cons 'new-password1 "The New Password1")
                          (cons 'new-password2 "The New Password2")))
              (list form->k-url
                    (list (cons 'old-password "password")
                          (cons 'new-password1 "The New Password")
                          (cons 'new-password2 "The New Password")))                
              (list (hyperlink->k-url "Logout") '()))
        (login-page))
      (void)
      (backend:update-password! "student one" "password"))

    (make-test-case
      (string-append
        "A user attempts to create an account, enters the wrong Northeastern "
        "ID, then enters a taken username, then enters mismatched passwords, "
        "then succeeds in creating an account, then logs out")
      (assert-output-response/suspended
        the-servlet
        (list (list (hyperlink->k-url "Create Username") '())
              (list form->k-url
                    (list (cons 'name "Unmade User")
                          (cons 'neu-id "1010") ;; The wrong ID
                          (cons 'username "unmade")
                          (cons 'password1 "unmade")
                          (cons 'password2 "unmade")))
              (list form->k-url
                    (list (cons 'name "Unmade User")
                          (cons 'neu-id "1234")
                          (cons 'username "student one") ;; Taken
                          (cons 'password1 "unmade")
                          (cons 'password2 "unmade")))
              (list form->k-url
                    (list (cons 'name "Unmade User")
                          (cons 'neu-id "1234")
                          (cons 'username "unmade")
                          (cons 'password1 "p1")
                          (cons 'password2 "p2"))) ;; Mismatched
              (list form->k-url
                    (list (cons 'name "Unmade User")
                          (cons 'neu-id "1234")
                          (cons 'username "unmade")
                          (cons 'password1 "unmade")
                          (cons 'password2 "unmade")))
              (list (hyperlink->k-url "Logout") '()))
        (login-page))
      (begin
        (db-do "DELETE FROM people WHERE name = 'Unmade User'")
        (db-do "INSERT INTO people (name, neu_id) VALUES ('Unmade User',1234)"))
      (void))

    (make-test-case
      (string-append
        "Create a username such that the username is not already taken, but "
        "the student for whom the username is being made already has a "
        "username. This should not be possible.")
      (assert-output-response/suspended
        the-servlet
        (list (list (hyperlink->k-url "Create Username") '())
              (list form->k-url
                    (list (cons 'name "student one")
                          (cons 'neu-id "1111")
                          (cons 'username "not taken as a username")
                          (cons 'password1 "p")
                          (cons 'password2 "p"))))
        (create-username-page "You already have a username."))
      ;;; - "not taken as a username" must not be taken;
      ;;; - "The Test Name" must have a username.
      (backend:destroy-user! "not taken as a username")
      ;;; "The Test Name"'s username must be "student one"
      (backend:destroy-user! "not taken as a username")
      )

    (make-test-case
      (string-append
        "A student logs in, selects his or her course, changes his or her "
        "password, goes back to the course selection, selects the same course "
        "and logs out.")
      (assert-output-response/suspended
        the-servlet
        (list (list form->k-url
                    (list (cons 'username "student one")
                          (cons 'password "password")))
              (list (hyperlink->k-url "The Test Course") '())
              (list (hyperlink->k-url "Change Password") '())
              (list form->k-url
                    (list (cons 'old-password "password")
                          (cons 'new-password1 "The New Password")
                          (cons 'new-password2 "The New Password")))
              (list (hyperlink->k-url "Courses") '())
              (list (hyperlink->k-url "The Test Course") '())
              (list (hyperlink->k-url "Logout") '()))
        (login-page))
      ;; setup
      (void)
      ;; cleanup
      (backend:update-password! "student one" "password"))

    (make-test-case
      (string-append
        "A non-student logs in, selects his or her course, changes his or her "
        "password, goes back to the course selection, selects the same course "
        "and logs out.")
      (assert-output-response/suspended
        the-servlet
        (list (list form->k-url
                    (list (cons 'username "staff one")
                          (cons 'password "password")))
              (list (hyperlink->k-url "The Test Course") '())
              (list (hyperlink->k-url "Change Password") '())
              (list form->k-url
                    (list (cons 'old-password "password")
                          (cons 'new-password1 "The New Password")
                          (cons 'new-password2 "The New Password")))
              (list (hyperlink->k-url "Courses") '())
              (list (hyperlink->k-url "The Test Course") '())
              (list (hyperlink->k-url "Logout") '()))
        (login-page))
      ;; setup
      (void)
      ;; cleanup
      (backend:update-password! "staff one" "password"))

    (make-test-case
      (string-append
        "A user logs in, selects a course in which he or she is a student, "
        "goes to the partnership management page, adds a partner, logs out.")
      (assert-output-response/suspended
        the-servlet
        (list (list form->k-url
                    (list (cons 'username "student three")
                          (cons 'password "password")))
              (list (hyperlink->k-url "The Test Course") '())
              (list (hyperlink->k-url "Partners") '())
              (list form->k-url
                    (list (cons 'username "student four")
                          (cons 'password "password")))
              (list (hyperlink->k-url "Logout") '()))
        (login-page))
      ;; setup: student one has no partners
      (reset-partner)
      ;; cleanup: student one has no partners
      (reset-partner))

    (make-test-case
      (string-append
        "A user logs in, selects a course in which he or she is a student, "
        "goes to the partnership management page, views partners logs out.")
      (assert-output-response/suspended
        the-servlet
        (list (list form->k-url
                    (list (cons 'username "student one")
                          (cons 'password "password")))
              (list (hyperlink->k-url "The Test Course") '())
              (list (hyperlink->k-url "Partners") '())
              (list (hyperlink->k-url "Logout") '()))
        (login-page)))

    (make-test-case
      (string-append
        "A user logs in, selects a course in which he or she is a student, "
        "already has the correct number of partners, goes to the assignments "
        "page, logs out.")
      (assert-output-response/suspended
        the-servlet
        (list (list form->k-url
                    (list (cons 'username "student one")
                          (cons 'password "password")))
              (list (hyperlink->k-url "The Test Course") '())
              (list (hyperlink->k-url "Assignments") '())
              (list (hyperlink->k-url "Logout") '()))
        (login-page)))

    (make-test-case
      (string-append
        "A user logs in, selects a course in which he or she is a student, "
        "forms a partnership, views assignments, views 'The Test Assignment "
        "One', goes back, views 'The Test Assignment Four', goes back, logs out.")
      (assert-output-response/suspended
        the-servlet
        (list (list form->k-url
                    (list (cons 'username "student three")
                          (cons 'password "password")))
              (list (hyperlink->k-url "The Test Course") '())
              (list (hyperlink->k-url "Partners") '())
              (list form->k-url
                    (list (cons 'username "student four")
                          (cons 'password "password")))
              (list (hyperlink->k-url "Assignments") '())
              (list (hyperlink->k-url "The Test Assignment One") '())
              'back
              (list (hyperlink->k-url "The Test Assignment Four") '())
              'back
              (list (hyperlink->k-url "Logout") '()))
        (login-page))
      (reset-partner)
      (reset-partner))

    (make-test-case
      (string-append
        "A user logs in, selects a course in which he or she is a student, "
        "views assignments, submits an assignment, logs out.")
      (assert-output-response/suspended
        the-servlet
        (list (list form->k-url
                    (list (cons 'username "student one")
                          (cons 'password "password")))
              (list (hyperlink->k-url "The Test Course") '())
              (list (hyperlink->k-url "Assignments") '())
              (list form->k-url
                    (list (cons 'file "/etc/passwd"))
                    (list (cons 'enctype #"multipart/form-data")))
              (list (hyperlink->k-url "Logout") '()))
        (login-page)))

    #|
    ;;; I don't know how to write this!
    (make-test-case
      (string-append
        "A user logs in, selects a course in which he or she is an instructor, "
        "selects ``Add/Drop Student'', adds a student to the database and to the "
        "course, adds a student to the course that is already in "
        "the database, drops a student, then logs out.")
      (assert-output-response/suspended
        the-servlet
        (list (list form->k-url
                    (list (cons 'username "instructor one")
                          (cons 'password "password")))
              (list (hyperlink->k-url "The Test Course") '())
              (list (hyperlink->k-url "Add/Drop Student") '())
              (list (form-n->k-url 2)
                    (list (cons 'name "Joe Schmo")
                          (cons 'neu-id "1234")))
              (list (form-n->k-url 1)
                    (list (cons 'id 
                                (id-of-student-in-db-but-not-in-course))))
              (list (form-n->k-url 3)
                    (list (cons 'id
                                (id-of-student-in-course))))
              (list (hyperlink->k-url "Logout") '()))
        (login-page)))
    |#

    #|
    ;;; This is another hard test case. I need to figure out the IDs for
    ;;; the partnerships as this is running.
    (make-test-case
      (string-append
        "A user logs in, selects a course in which he or she is an instructor, "
        "selects ``Partnerships'', merges two or more partnerships, breaks "
        "those partnerships, toggles the submission bits for those and other "
        "partnerships.")
      (assert-output-response/suspended
        the-servlet
        (list (list form->k-url
                    (list (cons 'username "instructor one")
                          (cons 'password "password")))
              (list (hyperlink->k-url "The Test Course") '())
              (list (hyperlink->k-url "Partnerships") '())
              (list form->k-url
                    (list (cons 'action "Merge")
                          (cons 'id "???")
                          (cons 'id "???")
                          (cons 'id "???")))
              (list form->k-url
                    (list (cons 'action "Break")
                          (cons 'id "???")
                          (cons 'id "???")))
              (list form->k-url
                    (list (cons 'action "Toggle Submission")
                          (cons 'id "???")
                          (cons 'id "???")))
              (list (hyperlink->k-url "Logout") '()))
        (login-page)))
    |#

     ))

(define (id-of-student-in-db-but-not-in-course)
  (with-handlers ((exn? (lambda (e)
                          (fail
                            (format
                              "id-of-student-in-db-but-not-in-course: ~a~n"
                              (exn-message e))))))
    (send backend:*connection* query-value
          (string-append
            "SELECT id FROM people "
            "WHERE id NOT IN ("
            "SELECT c_p.person_id "
            "FROM course_people c_p "
            "JOIN courses c ON c.id = c_p.course_id "
            "WHERE c_p.position = 'student' AND c.number = 'TTT000'"
            ") AND name != 'Joe Schmo' LIMIT 1"))))

(define (id-of-student-in-course)
  (with-handlers ((exn? (lambda (e)
                          (fail 
                            (format
                              "id-of-student-in-course: ~a~n"
                              (exn-message e))))))
    (send backend:*connection* query-value
          (string-append
            "SELECT p.id FROM people p "
            "JOIN course_people c_p ON c_p.person_id = p.id "
            "JOIN courses c ON c.id = c_p.course_id "
            "WHERE p.name = 'Joe Schmo' "
            "AND c.number = 'TTT000'"))))

;; The URL for the nth for in a HTML page.
(define (form-n->k-url d)
  (let ((n d))
    (letrec ((f
               (match-lambda
                 (('form ((attrs rhss) ...) . rest)
                  (if (= n 1)
                    (ormap (lambda (attr rhs) (and (eqv? attr 'action) rhs))
                           attrs rhss)
                    (begin
                      (set! n (sub1 n))
                      #f)))
                 ((tag (attrs ...) body ...)
                  (ormap f body))
                 (else
                   #f))))
      f)))

;; reset-partner : ->
;; Clear the partnership between student three and student four, if any.
(define (reset-partner)
  (db-do
    (string-append
      "UPDATE partners SET ended = now() "
      "WHERE ended IS NULL "
      "AND student_id = "
      "(SELECT id FROM people WHERE username = 'student four') "
      "OR student_id = "
      "(SELECT id FROM people WHERE username = 'student three')"))
  (db-do
    (format
      (string-append
        "INSERT INTO partners "
        "(student_id, partner_id, course_id, created, can_submit) "
        "VALUES ((SELECT id FROM people WHERE username = 'student three'),"
        "(SELECT max(partner_id) + 1 FROM partners), ~a, now(), 'f')")
      cid))
  (db-do
    (format
      (string-append
        "INSERT INTO partners "
        "(student_id, partner_id, course_id, created, can_submit) "
        "VALUES ((SELECT id FROM people WHERE username = 'student four'),"
        "(SELECT max(partner_id) + 1 FROM partners), ~a, now(), 'f')")
      cid)))

;; login-page : [string] -> xexpr/callback
;; The login page, with an optional error message.
(define login-page
  (opt-lambda ((message #f))
    `(html () (head () (title () "Please Log In"))
           (body () (h1 () "Please Log In")
                 ,(if message `(p () ,message) "")
                 (form ((action ,(make-unknown)))
                       (p () (label ((for "username")) "User name")
                          (input ((name "username") (id "username")
                                                    (type "text"))))
                       (p () (label ((for "password")) "Password")
                          (input ((name "password") (id "password")
                                                    (type "password"))))
                       (p () (input ((type "submit") (value "Log in")))))))))

;; logged-in-page : xexpr/callback
;; The page after logging in.
(define logged-in-page
  `(html () (head () (title () "You Are Logged In"))
         (body () (h1 () "You Are Logged In")
               (p "Congrats, you've logged in.")
               (p (a ((href ,(make-unknown))) "Logout")))))

;; create-username-page : [string] -> xexpr/callback
;; The create username page, with an optional error message.
(define create-username-page
  (opt-lambda ((message #f))
    `(html () (head () (title () "Create A Username"))
           (body () (h1 () "Create A Username")
                 ,(if message `(p () ,message))
                 (form ((method "POST")
                        (action ,(make-unknown)))
                       (p () (label ((for "name")) "Last name")
                          (input ((type "text") (id "name") (name "name"))))
                       (p () (label ((for "neu-id")
                                     "Last four digits of Northeastern ID"))
                          (input ((type "text") (id "neu-id") (name "neu-id"))))
                       (p () (label ((for "username")) "Desired username")
                          (input ((type "text") (id "username")
                                                (name "username"))))
                       (p () (label ((for "password1")) "Password")
                          (input ((type "password") (id "password1")
                                                    (name "password1"))))
                       (p () (label ((for "password2")) "Password")
                          (input ((type "password") (id "password2")
                                                    (name "password2"))))
                       (p () (input ((type "submit") (value "Log in")))))))))

;; restart-session-page : xexpr/callback
;; The session has timed out.
(define restart-session-page
  `(html () (head () (title () "Timeout"))
         (body () (p () ,(string-append
                           "The transaction referred to by this url is no "
                           "longer active.  Please ")
                     ,(make-unknown) " the transaction."))))

;; ********************************************************************
(with-handlers ((exn? (lambda (e) (db-do "ROLLBACK") (raise e))))
  (db-do "BEGIN")
  (cleanup)
  (setup)
  (db-do "COMMIT")
  (test/text-ui test-servlet)
  (db-do "BEGIN")
  (cleanup)
  (db-do "COMMIT")
  )
