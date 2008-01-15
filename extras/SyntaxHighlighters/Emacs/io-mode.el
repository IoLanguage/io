;;; io-mode.el --- Major mode for Io source

;; Author: Erik Max Francis <max@alcyone.com>
;; Created: 2004 Apr 23
;; Keywords: io, languages
;; URL: <http://www.alcyone.com/software/io-mode/>

;; Copyright (C) 2004 Erik Max Francis

;; This program is free software; you can redistribute it and/or
;; modify it under the terms of the GNU General Public License as
;; published by the Free Software Foundation; either version 2 of
;; the License, or (at your option) any later version.

;; This program is distributed in the hope that it will be
;; useful, but WITHOUT ANY WARRANTY; without even the implied
;; warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
;; PURPOSE.  See the GNU General Public License for more details.

;; You should have received a copy of the GNU General Public
;; License along with this program; if not, write to the Free
;; Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
;; MA 02111-1307 USA

;;; Commentary:

;; This is a major mode for the Steve Dekorte's Io programming
;; language <http://www.iolanguage.com>.  This mode supports:

;; - indentation based on parenthesis level
;; - electric close parentheses (close parenthesis auto-indents)
;; - syntax highlighting on all three Io comment types
;; - syntax highlighting on both string literals types
;; - syntax highlighting on assigned slot names (that is, the
;;   left-hand side of := and = messages)
;; - syntax highlighting on names:
;;   - standard addons (e.g., IoVM, IoServer)
;;   - standard prototypes (e.g., Object, List, nil)
;;   - conventional messages and slots (e.g., self, if, proto, method, clone)
;;   - conventional operators (e.g., +, -, ..)
;;   - special operators (e.g., ?, @, @@)
;;   - custom names (user-specified)

;; The indentation is designed to work with syntaxes similar to the
;; following:

;; aMethod := method(args, something)
;; anotherMethod := method(args, 
;;   doSomething
;;   doSomethingMore)
;; yetAnotherMethod := method(args,
;;   doSomething
;;   doSomethingMore
;; )
;; if(condition) then(
;;   something
;; ) else(
;;   somethingElse
;; )

;; Customization is possible by tweaking the variable and constants in
;; the "Parameters" sections below.  `io-mode-indent-offset'
;; represents the number of spaces Io will use at each indentation
;; level.  `io-mode-electric-parents-p' indicates whether or not Io
;; mode will use "electric parentheses" -- that is, will autoindent
;; after an open or close parenthesis is typed.
;; `io-mode-clever-indent-p' indicates whether or not Io mode will
;; attempt to do clever dedenting and reindenting based on previous
;; indentation (e.g., a close parenthesis on a blank line is dedented
;; as in the above example).  `io-mode-auto-mode-p' indicates whether
;; or not the Io mode will add itself to the auto mode list (i.e.,
;; loading a .io file will automatically initiate Io mode).  The
;; constants of the form `io-mode-...-names' are simple lists of
;; strings that indicate which names will be highlighted for each
;; category (addon, prototype, message, operator, special, and
;; custom).  The custom names (`io-mode-custom-names') are included as
;; an explicit user-definable list of highlighted keywords.

;; This emacs mode was designed for GNU emacs version 21.  No attempt
;; at backward- or cross-compatibility has currently been made.

;; Thanks to the emacs mode tutorial by Scott Andrew Borton at
;; <http://two-wugs.net/emacs/mode-tutorial.html>.

;;; Change Log:

;; 0.5     2004-12-15  Some additional keywords.
;;
;; 0.4     2004-05-08  Scan back for first non-blank line while indenting;
;;                     blinking close parentheses; some typos fixed and 
;;                     additional keywords added.
;;
;; 0.3     2004-04-25  Add custom, operator, and special names highlighting; 
;;                     unify and expand "slots" and "messages" lists; more 
;;                     customization options; better documentation.
;;
;; 0.2     2004-04-25  Add "customs" highlighting list; highlight ., ?, @, 
;;                     and @@ messages.
;;
;; 0.1     2004-04-23  Initial public release.

;;; Bugs:

;; Presently the indentation code will only look at the previous line
;; for a hint, not scan up to the previous non-blank line.  This
;; doesn't seem like a major issue, however, since most Io styles I've
;; seen don't use double spacing.

;; Typing an open parenthesis on the very first line of a buffer
;; indents when it should not.

;; Two parentheses on the same line result in two indents, not one.
;; It's not clear that this is wrong, though.

;; Commenting out an otherwise fontified line (like: #a := b) will
;; result in other font markup other than the comment (e.g., the a
;; will be highlighted).

;;; Versioning:

(defconst io-mode-version "0.5"
  "Io mode version number")

(defconst io-mode-date "2004-12-15"
  "Io mode version date")

(defconst io-mode-tested-with-io-version "2004-12-06"
  "Io mode tested date")

;;; Parameters:

(defvar io-mode-indent-offset 2
  "Io mode indent offset")

(defvar io-mode-electric-parens-p t
  "Should the Io mode autoindent after parentheses are typed?")

(defvar io-mode-clever-indent-p t
  "Should the Io mode try to dedent and reindent depending on context?")

(defconst io-mode-auto-mode-p t
  "Should the Io mode add itself to the auto mode list?")

(defconst io-mode-addon-names '(
				"IoDesktop"
				"IoServer"
				"IoVM"
				)
  "Io mode add-on names")

(defconst io-mode-prototype-names '(
				    "Array"
				    "AudioDevice"
				    "AudioMixer"
				    "Block"
				    "Box"
				    "Buffer"
				    "CFunction"
				    "CGI"
				    "Color"
				    "Curses"
				    "DBM"
				    "DNSResolver"
				    "DOConnection"
				    "DOProxy"
				    "DOServer"
				    "Date"
				    "Directory"
				    "Duration"
				    "DynLib"
				    "Error"
				    "Exception"
				    "FFT"
				    "File"
				    "Fnmatch"
				    "Font"
				    "Future"
				    "GL"
				    "GLE"
				    "GLScissor"
				    "GLU"
				    "GLUCylinder"
				    "GLUQuadric"
				    "GLUSphere"
				    "GLUT"
				    "Host"
				    "Image"
				    "Importer"
				    "LinkList"
				    "List"
				    "Lobby"
				    "Locals"
				    "MD5"
				    "MP3Decoder"
				    "MP3Encoder"
				    "Map"
				    "Message"
				    "Movie"
				    "nil"
				    "true"
				    "false"
				    "Notification"
				    "Number"
				    "Object"
				    "OpenGL"
				    "Point"
				    "Protos"
				    "Regex"
				    "SGMLTag"
				    "SQLite"
				    "Server"
				    "ShowMessage"
				    "SleepyCat"
				    "SleepyCatCursor"
				    "Socket"
				    "SocketManager"
				    "Sound"
				    "Soup"
				    "Store"
				    "String"
				    "Tree"
				    "UDPSender"
				    "UPDReceiver"
				    "URL"
				    "User"
				    "Warning"
				    "WeakLink"
				    )
  "Io mode type names")

(defconst io-mode-message-names '(
				  "activate"
				  "activeCoroCount"
				  "and"
				  "asString"
				  "block"
				  "break"
				  "catch"
				  "clone"
				  "collectGarbage"
				  "compileString"
				  "continue"
				  "do"
				  "doFile"
				  "doMessage"
				  "doString"
				  "else"
				  "elseif"
				  "exit"
				  "for"
				  "foreach"
				  "forward"
				  "getSlot"
				  "getEnvironmentVariable"
				  "hasSlot"
				  "if"
				  "ifFalse"
				  "ifNil"
				  "ifTrue"
				  "isActive"
				  "isNil"
				  "isResumable"
				  "list"
				  "message"
				  "method"
				  "or"
				  "parent"
				  "pass"
				  "pause"
				  "perform"
				  "performWithArgList"
				  "print"
				  "proto"
				  "raise"
				  "raiseResumable"
				  "removeSlot"
				  "resend"
				  "resume"
				  "return"
				  "schedulerSleepSeconds"
				  "self"
				  "sender"
				  "setSchedulerSleepSeconds"
				  "setSlot"
				  "shallowCopy"
				  "slotNames"
				  "super"
				  "system"
				  "then"
				  "thisBlock"
				  "thisContext"
				  "thisMessage"
				  "try"
				  "type"
				  "uniqueId"
				  "updateSlot"
				  "wait"
				  "while"
				  "write"
				  "yield"
				  )
  "Io mode message names")

(defconst io-mode-operator-names '(
				   "++"
				   "--"
				   "*"
				   "/"
				   "%"
				   "^"
				   "+"
				   "-"
				   ">>"
				   "<<"
				   ">"
				   "<"
				   "<="
				   ">="
				   "=="
				   "!="
				   "&"
				   "^"
				   ".."
				   "|"
				   "&&"
				   "||"
				   "!="
				   "+="
				   "-="
				   "*="
				   "/="
				   "<<="
				   ">>="
				   "&="
				   "|="
				   "%="
				   "="
				   ":="
				   "<-"
				   "<->"
				   "->"
				   )
  "Io mode operator names")

(defconst io-mode-special-names '(
				  "."
				  "?"
				  "@"
				  "@@"
				  )
  "Io mode special names")

(defconst io-mode-custom-names '(
				 ; put your own keywords here
				 "aCustomExample"
				 )
  "Io mode custom names")

;;; Code:

(if io-mode-auto-mode-p
    (add-to-list 'auto-mode-alist '("\\.io\\'" . io-mode)))

(defvar io-mode-hook nil
  "Io mode hook")

(defvar io-mode-keymap 
  (let ((io-mode-keymap (make-sparse-keymap)))
    (if io-mode-electric-parens-p
	(progn
	  (define-key io-mode-keymap "(" 'io-mode-electric-open-paren)
	  (define-key io-mode-keymap ")" 'io-mode-electric-close-paren)))
    io-mode-keymap)
  "Io mode keymap")

(defvar io-mode-font-lock-defaults
  (list
   '("#.*$" . font-lock-comment-face)
   '("//.*$" . font-lock-comment-face)
   '("/\\*.*\\*/" . font-lock-comment-face)
   `(,(concat "\\<" (regexp-opt io-mode-addon-names t) "\\>") . font-lock-constant-face)
   `(,(concat "\\<" (regexp-opt io-mode-prototype-names t) "\\>") . font-lock-type-face)
   `(,(concat "\\<" (regexp-opt io-mode-message-names t) "\\>") . font-lock-keyword-face)
   `(,(concat "\\<" (regexp-opt io-mode-custom-names t) "\\>") . font-lock-builtin-face)
   `(,(regexp-opt io-mode-operator-names t) . font-lock-variable-name-face)
   `(,(regexp-opt io-mode-special-names t) . font-lock-warning-face)
   '("\\(\\w*\\)\\s-*:?=" . '(1 font-lock-function-name-face t nil))
  )
  "Io mode font lock defaults")

(defvar io-mode-syntax-table
  (let ((io-mode-syntax-table (make-syntax-table)))
    (modify-syntax-entry ?_ "w" io-mode-syntax-table)
    (modify-syntax-entry ?/ ". 124b" io-mode-syntax-table)
    (modify-syntax-entry ?* ". 23" io-mode-syntax-table)
    (modify-syntax-entry ?\n "> b" io-mode-syntax-table)
    io-mode-syntax-table)
  "Io mode syntax table")

(defun io-mode-indent-line ()
  "Io mode indent line"
  (interactive)
  (if (bobp)
      (indent-line-to 0)
    (let (current-depth current-close-flag current-close-open-flag
	  last-indent last-depth last-close-flag last-close-open-flag)
      (save-excursion
	(let (start-point end-point)
	  ; get the balance of parentheses on the current line
	  (end-of-line)
	  (setq end-point (point))
	  (beginning-of-line)
	  (setq current-close-flag (looking-at "^[ \\t)]*)[ \\t)]*$"))
	  (setq current-close-open-flag (looking-at "^\\s-*).*(\\s-*$"))
	  (setq start-point (point))
	  (setq current-depth (car (parse-partial-sexp start-point end-point)))
	  ; and the previous non-blank line
	  (while (progn 
		   (forward-line -1)
		   (beginning-of-line)
		   (and (not (bobp))
			(looking-at "^\\s-*$"))))
	  (setq last-indent (current-indentation))
	  (end-of-line)
	  (setq end-point (point))
	  (beginning-of-line)
	  (setq last-close-flag (looking-at "^[ \\t)]*)[ \\t)]*$"))
	  (setq last-close-open-flag (looking-at "^\\s-*).*(\\s-*$"))
	  (setq start-point (point))
	  (setq last-depth (car (parse-partial-sexp start-point end-point)))))
      (let ((depth last-depth))
	(if io-mode-clever-indent-p
	    (setq depth (+ depth
			   (if current-close-flag current-depth 0)
			   (if last-close-flag (- last-depth) 0)
			   (if current-close-open-flag -1 0)
			   (if last-close-open-flag 1 0))))
	(indent-line-to (max 0
			     (+ last-indent
				(* depth io-mode-indent-offset))))))))

(defun io-mode-electric-open-paren ()
  "Io mode electric close parenthesis"
  (interactive)
  (insert ?\()
  (let ((marker (make-marker)))
    (set-marker marker (point-marker))
    (indent-according-to-mode)
    (goto-char (marker-position marker))
    (set-marker marker nil)))

(defun io-mode-electric-close-paren ()
  "Io mode electric close parenthesis"
  (interactive)
  (insert ?\))
  (let ((marker (make-marker)))
    (set-marker marker (point-marker))
    (indent-according-to-mode)
    (goto-char (marker-position marker))
    (set-marker marker nil))
  (blink-matching-open))

(defun io-mode ()
  "Io mode"
  (interactive)
  (kill-all-local-variables)
  (set-syntax-table io-mode-syntax-table)
  (use-local-map io-mode-keymap)
  (set (make-local-variable 'font-lock-defaults) '(io-mode-font-lock-defaults))
  (set (make-local-variable 'indent-line-function) 'io-mode-indent-line)
  (setq major-mode 'io-mode)
  (setq mode-name "Io")
  (run-hooks 'io-mode-hook))

(provide 'io-mode)

;;; io-mode.el ends here
