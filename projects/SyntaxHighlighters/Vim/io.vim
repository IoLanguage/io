" Vim Syntax File
" Language:     Io
" Creator:      Scott Dunlop <swdunlop@verizon.net>
" Fixes:        Manpreet Singh <junkblocker@yahoo.com>
"               Jonathan Wright <quaggy@gmail.com>
"               Erik Garrison <erik.garrison@gmail.com>
" Last Change:  2006 Nov 16

if version < 600
    syntax clear
elseif exists("b:current_syntax")
    finish
endif

syntax case match

" equivalent to io-mode-prototype-names in io-mode.el
syntax keyword xType Array AudioDevice AudioMixer Block Box Buffer CFunction
syntax keyword xType CGI Color Curses DBM DNSResolver DOConnection DOProxy
syntax keyword xType DOServer Date Directory Duration DynLib Error Exception
syntax keyword xType FFT File Fnmatch Font Future GL GLE GLScissor GLU
syntax keyword xType GLUCylinder GLUQuadric GLUSphere GLUT Host Image Importer
syntax keyword xType LinkList List Lobby Locals MD5 MP3Decoder MP3Encoder Map
syntax keyword xType Message Movie Notification Number Object
syntax keyword xType OpenGL Point Protos Regex SGML SGMLElement SGMLParser SQLite Server Sequence
syntax keyword xType ShowMessage SleepyCat SleepyCatCursor Socket
syntax keyword xType SocketManager Sound Soup Store String Tree UDPSender
syntax keyword xType UPDReceiver URL User Warning WeakLink
syntax keyword xType true false nil Random BigNum Sequence

" equivalent to io-mode-message-names in io-mode.el
syntax keyword xKeyword activate activeCoroCount and asString block break
syntax keyword xKeyword catch clone collectGarbage compileString continue
syntax keyword xKeyword do doFile doMessage doString else elseif exit for
syntax keyword xKeyword foreach forward getSlot getenv hasSlot if ifFalse
syntax keyword xKeyword ifNil ifNilEval ifTrue isActive isNil isResumable list message
syntax keyword xKeyword method or parent pass pause perform
syntax keyword xKeyword performWithArgList print println proto raise raiseResumable
syntax keyword xKeyword removeSlot resend resume return
syntax keyword xKeyword schedulerSleepSeconds self sender
syntax keyword xKeyword setSchedulerSleepSeconds setSlot shallowCopy
syntax keyword xKeyword slotNames super system then thisBlock thisContext
syntax keyword xKeyword call try type uniqueId updateSlot wait while
syntax keyword xKeyword write yield

syntax region xOperator start=':' end='='
syntax region xOperator start='!' end='='
syntax region xOperator start='\.' end='\.'
syntax region xOperator start='\.' end='[^\.]'he=e-1
syntax region xOperator start='=' end='='
syntax region xOperator start='=' end=' 'he=e-1
syntax region xOperator start='[*>=+-]' end='[ 0-9]'he=e-1

syntax region xString start=/"/ skip=/\\./ end=/"/
syntax region xString start=/"""/ skip=/\\./ end=/"""/

syntax region xComment start='#' end='$' keepend
syntax region xComment start=/\/\*/ end=/\*\//
syntax region xComment start=/\/\// end=/$/ keepend

highlight link xType Type
highlight link xKeyword Keyword
highlight link xString String
highlight link xComment Comment
highlight link xOperator Operator
highlight Operator ctermfg=5

let b:current_syntax = "io"
