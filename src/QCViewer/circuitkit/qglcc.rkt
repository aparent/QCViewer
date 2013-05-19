#!/usr/bin/env racket
#lang racket
(require xml
         xml/xexpr)

(define (do-statement s)
  (match s
    [(list-rest 'assignment '()
                (list-no-order
                  `(variable () ,var)
                  `(value () ,val)))
      (printf "Assigned ~a to ~a~n" var val)]
    [(list-rest 'gate '()
                (list-no-order
                  `(name () ,name)
                  (list-rest 'quantum '() targets)
                  optionals ...))
     (printf "Applied gate ~a to ~a with optionals ~a~n"
             name
             targets
             optionals)]
    [(list-rest (and type
                     (or 'control 'controlnot))
                '()
                (list-no-order
                  (list-rest 'quantum '() controls)
                  (list-rest 'block '() block)))
     (printf "Control ~a block over ~a containing ~a~n"
             type
             controls
             block)]
    [(list-rest 'repeat '()
                (list-no-order
                  `(variable () ,counter)
                  `(times () ,times)
                  (list-rest 'block '() block)))
     (printf "Repeat [~a][~a] block ~a~n"
             counter
             times
             block)]
    [(list-rest 'parallel '()
                (list-no-order
                  `(variable () ,counter)
                  `(times () ,times)
                  (list-rest 'block '() block)))
     (printf "Parallel [~a][~a] block ~a~n"
             counter
             times
             block)]
    [(list-rest 'parallel '()
                `(,(list-rest 'block '() blocks) ...))
     (printf "Parallel blocks: ~a~n"
             blocks)]
    [(list-rest 'with '()
                (list-no-order
                  (list-rest 'block '() with-block)
                  (list-rest 'do '() do-block)))
     (printf "With block ~a do block ~a~n"
             with-block
             do-block)]
    [(list-rest 'call '()
                (list-no-order
                  `(name () ,func)
                  optionals ...))
     (printf "Called ~a with optionals ~a~n"
             func optionals)]
    [(list-rest 'procedure '()
                (list-no-order
                  `(name () ,name)
                  optionals ...))
     (printf "Procedure ~a declared as ~a~n"
             name
             optionals)]
    [else
      (error 'statement "Malformed statement: ~a" s)]))

(define (do-circuit c)
  (match c
    [(list-rest 'circuit
                xmlns
                `((statement () ,statements) ...))
      (map do-statement statements)]
    [else
      (error 'circuit "Unexpected form in circuit body.")]))

(define (main)
  (define (sanitize xexp)
    (define (whitespace? xexp)
      (define whitespace-chars '(#\newline #\return #\tab #\space))
      (and (string? xexp)
           (andmap (lambda(c) (memf (lambda(d) (eq? c d)) whitespace-chars))
                   (string->list xexp))))
    (if (list? xexp)
        (foldr
          (lambda(xexp base)
            (cond [(whitespace? xexp) base]
                  [else (cons (sanitize xexp) base)]))
          empty
          xexp)
        xexp))
  (define quigl (sanitize (xml->xexpr (document-element (read-xml)))))
  (pretty-print quigl)
  (printf "~n=====================================~n")
  (do-circuit quigl))

(main)

