
#put this script into your .bashrc/.profile/xxx

# --> fcd
alias cs='pcs() { cd `cat ~/.fastcd.pwd` && ls -l; }; pcs '
alias csm='echo `pwd`" [path]" > ~/.fastcd.pwd'
alias fcd=pfastcd

pfastcd() {
	app=`echo ~`"/tools/fcd.app"
	conf=`echo ~`"/.fastcd"
	tmpf=`echo ~`"/.fastcd.tmp"
	pwdf=`echo ~`"/.fastcd.pwd"
	vimf=`echo ~`"/.fastcd.vim"
	if [ $# -lt 1 ] ; 
	then 
		echo ". [path]" > $pwdf ; $app ; 
		res=(`cat $pwdf | grep "path" | wc -l`)
		if [ $res -eq 1 ];then cd `cat $pwdf`;
		else
			str=`cat $pwdf`; file=`echo ${str##*/}`; path=`echo ${str%$file}`;
			echo "$path [path]" > $pwdf; vim $str;
		fi
		return 0;
	fi
	case $1 in
	"l" | "ls" | "list" | "p" | "h" | "-" | "-"* )
		$app $@
	;;
	"flash")
		$app $@ > $conf
	;;
	[1-9] | [1-9][0-9]*)
		$app p $1 > $pwdf && cd `cat $pwdf`;
	;;
	*)
		grep -n $1 $conf | grep "\[path\]" > $tmpf
		if [ $# -eq 2 ];then
			cd `head -n 1 $tmpf | awk -F: '{print $2}' | awk '{print $1}'`
		else
			cat $tmpf | while read line ; do echo -e "\033[33m $line \033[0m" ; done; 
			res=(`wc -l $tmpf`)
			echo -e "\033[33m ========$res matched======== \033[0m"
			if [ $res -eq 1 ] ; then 
				cd `awk -F: '{print $2}' $tmpf | awk '{print $1}'` ; 
			fi
		fi
		return 0;
	;;
	esac
};
# <-- fcd
