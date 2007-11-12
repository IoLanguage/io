;;; io-mode-del -- Major mode for editing Io files

;; Author: Scott Dunlop <swdunlop@verizon.net>
;; Created: 10 Oct 2003
;; Keywords: Io major-mode

;; Copyright (C) 2003 Scott William Dunlop <swdunlop@verizon.net>

;; This program is free software; you may redistribute it, and/or
;; modify it under the terms of the GNU General Public License as
;; published by the Free Software Foundation; either version 2 of
;; the License, or, at your option, any later version.

;; This program is distributed in the hope that it will be useful,
;; but without any warranty; without even the implied warranty of
;; merchantability or fitness for a particular purpose.  See the
;; GNU General Public License for more details.

;; You should have received a copy of the GNU GPL along with this
;; program.  If not, write to the Free Software Foundation, Inc.,
;; 59 Temple Place, Suite 330, Boston, MA, 02111-1307 USA.

;;; Commentary:
;;
;; This mode was derived from the example at 
;; http://two-wugs.net/emacs/mode-tutorial.html written by Scott
;; Andrew Borton <scott@pp.htv.fi>, and would have been most
;; difficult without the information provided.  The tutorial was
;; most appreciated!
;;
;; -- Scott Dunlop.

(defvar io-mode-hook nil)

(defvar io-mode-map nil "Keymap for Io major mode")
(if io-mode-map nil
  (setq io-mode-map (make-keymap)))

(setq auto-mode-alist
  (append
    '(("\\.io\\'" . io-mode))
    auto-mode-alist)
)


(defconst io-font-lock-keywords-1
  (list
   '("b\\(?:\\(?:loc\\|rea\\)k\\)\\|c\\(?:atchException\\|\\(?:lon\\|ontinu\\)e\\)\\|exit\\|list\\|method\\|p\\(?:ause\\|roto\\)\\|r\\(?:aiseException\\|e\\(?:sume\\|turn\\)\\)\\|super\\|wait\\|yield" . font-lock-keyword-face)
   '("\\('\\w*'\\)" . font-lock-variable-name-face)
  )
  "Basic highlighting expressions for Io mode.")

;; Io keywords generated from:
;;
;; (regexp-opt '("super" "continue" "list" "break" "clone" "proto" 
;;   "raiseException" "pause" "wait" "exit" "yield" "block" "return" "method"
;;   "catchException" "resume"
;; ))
;;
;; This is obviously not a list of all the slots commonly used with Io, but
;; expresses the most unusual ones, with an exceptional treatment by the
;; interpreter.

(defvar io-font-lock-keywords io-font-lock-keywords-1 
  "Default highlighting expressions for Io mode.")

;; TODO: Support #->Eol syntax formats.
(defvar io-mode-syntax-table nil "Syntax table for io-mode.")
(defun io-create-syntax-table () 
  (if io-mode-syntax-table () 
    (setq io-mode-syntax-table (make-syntax-table))
    (modify-syntax-entry ?_ "w" io-mode-syntax-table)
    (modify-syntax-entry ?/ ". 124b" io-mode-syntax-table)
    (modify-syntax-entry ?* ". 23" io-mode-syntax-table)
    (modify-syntax-entry ?\n "> b" io-mode-syntax-table))
  (set-syntax-table io-mode-syntax-table))

(defun io-mode () "Major mode for editing source files for the Io programming language."
  (interactive)
  (kill-all-local-variables)
  (io-create-syntax-table)
  (make-local-variable 'font-lock-defaults)
  (setq font-lock-defaults '(io-font-lock-keywords))
  (setq major-mode 'io-mode)
  (setq mode-name "Io")
  (run-hooks 'io-mode-hook))

(provide 'io-mode)

