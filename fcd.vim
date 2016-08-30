
" Do not load fcd.vim if is has already been loaded.
if exists("loaded_fcd")
    finish
endif
let loaded_fcd = 1

" cd----------->
autocmd BufEnter * if expand("%:p:h") !~ '^/tmp' | call AutoLcd() | endif

let s:userFirstOpenVim = 1 
function! AutoLcd(...)
	if (s:userFirstOpenVim == 1 && isdirectory(expand("%:p")) ) " first open dir for not cd
		"pass 
	else
		execute "lcd %:p:h"
	end
	let s:userFirstOpenVim = 0
endfunction
" <-----------cd

" fastcd--------->
function! Fcd(...)
	if a:0 == 1
		echom system('grep -n -i ' . a:1 . ' ~/.fastcd | grep "\[path\]" ' . "| awk -F: '{print $2}' > ~/.fastcd.vim")
	elseif a:0 == 2
		echom system('grep -n -i ' . a:1 . " ~/.fastcd | awk -F: '{print $2}' > ~/.fastcd.vim")
	end
	call Pcd()
	execute "e ~/.fastcd.vim"
	let @/ = '\<'.a:1.'\>'
endfunction

function! Pcd(...)
	echom system( "echo " . expand("%:p:h") . "> ~/.fastcd.pwd")
endfunction

function! GoFile(...)
	set iskeyword+=. 
	set iskeyword+=/
	let l:WordValue = expand("<cword>")
	set iskeyword-=. 
	set iskeyword-=/
	execute "tabnew"
	call Fcd(l:WordValue , "fake")
endfunction

function! FindDefine(...)
	let l:WordValue = expand("<cword>")
	let l:WordFile = expand("%:p")
	echom system( "fcd.app search_define " . l:WordValue . " " . l:WordFile . " 5 7 > ~/.fastcd.vim" )
	execute "tabnew"
	execute "e ~/.fastcd.vim"
	let @/ = '\<'.l:WordValue.'\>'
endfunction

" nmap <C-f> <Esc>:call Fcd( expand("<cword>") , 'all' )<CR>
nmap <C-e> <Esc>:call GoFile()<CR>
nmap <C-f> <Esc>:call FindDefine()<CR>
command! -nargs=* Fcd call Fcd( <f-args> )
command! -nargs=* Pcd call Pcd( <f-args> )
" <----------fastcd
