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
syntax keyword ioType Array AudioDevice AudioMixer Block Box Buffer CFunction
syntax keyword ioType CGI Color Curses DBM DNSResolver DOConnection DOProxy
syntax keyword ioType DOServer Date Directory Duration DynLib Error Exception
syntax keyword ioType FFT File Fnmatch Font Future GL GLE GLScissor GLU
syntax keyword ioType GLUCylinder GLUQuadric GLUSphere GLUT Host Image Importer
syntax keyword ioType LinkList List Lobby Locals MD5 MP3Decoder MP3Encoder Map
syntax keyword ioType Message Movie Notification Number Object
syntax keyword ioType OpenGL Point Protos Regex SGML SGMLElement SGMLParser SQLite Server Sequence
syntax keyword ioType ShowMessage SleepyCat SleepyCatCursor Socket
syntax keyword ioType SocketManager Sound Soup Store String Tree UDPSender
syntax keyword ioType UPDReceiver URL User Warning WeakLink
syntax keyword ioType true false nil Random BigNum Sequence

" equivalent to io-mode-message-names in io-mode.el
syntax keyword ioKeyword activate activeCoroCount and asString block break
syntax keyword ioKeyword catch clone collectGarbage compileString continue
syntax keyword ioKeyword do doFile doMessage doString else elseif exit for
syntax keyword ioKeyword foreach forward getSlot getEnvironmentVariable hasSlot if ifFalse
syntax keyword ioKeyword ifNil ifNilEval ifTrue isActive isNil isResumable list message
syntax keyword ioKeyword method or parent pass pause perform
syntax keyword ioKeyword performWithArgList print println proto raise raiseResumable
syntax keyword ioKeyword removeSlot resend resume return
syntax keyword ioKeyword schedulerSleepSeconds self sender
syntax keyword ioKeyword setSchedulerSleepSeconds setSlot shallowCopy
syntax keyword ioKeyword slotNames super system then thisBlock thisContext
syntax keyword ioKeyword call try type uniqueId updateSlot wait while
syntax keyword ioKeyword write yield

syntax region ioOperator start=':' end='='
syntax region ioOperator start='!' end='='
syntax region ioOperator start='@' end='@'
syntax region ioOperator start='@' end='[^@]'he=e-1
syntax region ioOperator start='?' end='?'
syntax region ioOperator start='?' end='[^?]'he=e-1
syntax region ioOperator start=';' end=';'
syntax region ioOperator start=';' end='[^;]'he=e-1
syntax region ioOperator start='\.' end='\.'
syntax region ioOperator start='\.' end='[^\.]'he=e-1
syntax region ioOperator start='=' end='='
syntax region ioOperator start='=' end=' 'he=e-1
syntax region ioOperator start='[*/>=+-]' end='[ 0-9]'he=e-1

syntax region ioString start=/"/ skip=/\\./ end=/"/
syntax region ioString start=/"""/ skip=/\\./ end=/"""/

syn match   ioHexNumber	"\<0[xX]\x\+[lL]\=\>" display
syn match   ioHexNumber	"\<0[xX]\>" display
syn match   ioNumber	"\<\d\+[lLjJ]\=\>" display
syn match   ioFloat		"\.\d\+\([eE][+-]\=\d\+\)\=[jJ]\=\>" display
syn match   ioFloat		"\<\d\+[eE][+-]\=\d\+[jJ]\=\>" display
syn match   ioFloat		"\<\d\+\.\d*\([eE][+-]\=\d\+\)\=[jJ]\=" display

syn match   ioOctalError	"\<0\o*[89]\d*[lL]\=\>" display
syn match   ioError	"\<0[xX]\X\+[lL]\=\>" display

syntax region ioComment start='#' end='$' keepend
syntax region ioComment start=/\/\*/ end=/\*\//
syntax region ioComment start=/\/\// end=/$/ keepend

highlight link ioType Type
highlight link ioKeyword Function
highlight link ioString String
highlight link ioComment Comment
highlight link ioOperator Operator

highlight link ioHexNumber Number
highlight link ioNumber Number
highlight link ioFloat Float
highlight link ioOctalError Error
highlight link ioError Error

let b:current_syntax = "io"
