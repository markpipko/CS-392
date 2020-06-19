# Mark Pipko
# I pledge my honor that I have abided by the Stevens Honor System.

#!/bin/bash

file="/home/cs392/junk/junk.sh"

readonly JUNK="~/.junk"

display_message() {
cat << ENDOFTEXT 
Usage: $(basename "$file") [-hlp] [list of files]
   -h: Display help.    
   -l: List junked files.   
   -p: Purge all files.  
   [list of files] with no other arguments to junk those files.
ENDOFTEXT
}

help_flag=0
list_flag=0
purge_flag=0

while getopts ":hlp" option; do
	case "$option" in 
		h) help_flag=1
		   ;;
		l) list_flag=1 
		   ;;
		p) purge_flag=1 
		   ;;
		?)  printf "Error: Uknown option '-%s'.\n" $OPTARG >&2
			display_message
			exit 1
		   ;;
	esac
done

process_file(){
	if [ -e /home/cs392/junk/"$1" ]; then
		mv "$1" "$HOME/.junk"
	else
		echo "Warning: '$1' not found."
	fi
}

if  [ ! -d "$HOME/.junk" ]; then 
	mkdir -p "$HOME/.junk"
fi	

if [ $# -gt 0 ] && [ $OPTIND -lt 2 ]; then
	for f in $@; do
		process_file "$f"
	done
	exit 0
fi

if [ $[$help_flag + $list_flag + $purge_flag] -gt 0 ] && [ $# -gt 1 ]; then
	echo "Error: Too many options enabled."
	display_message
	exit 1
fi

if [ $# -eq 0 ]; then 
	display_message
	exit 1
fi 

if [ $help_flag -eq 1 ]; then
	display_message
	exit 0
fi	

if [ $list_flag -eq 1 ]; then
	ls "$HOME/.junk" -lAF 
	exit 0
fi	

if [ $purge_flag -eq 1 ]; then
	rm -rf /home/cs392/.junk
	exit 0
fi
