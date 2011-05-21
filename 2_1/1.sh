#!/bin/bash
declare -A ideal;

if [[ !(-r $1 && -r $2)  ]]; then 
	echo "Use ideal check" ;
	exit
fi

while read name_md5 size ;
	do if [ $name_md5 ]; then
		if [ $size ]; then
			md5="$name_md5::$size"
		else
			tmp=$name_md5;
			ideal[$tmp]="$md5"
		fi;
	fi;
done <"./$1"

while read md5 size filename;
	do 
		for name in "${!ideal[@]}"; 
			do 
				if [[ $filename == "$name" && "$md5::$size" != "${ideal["$name"]}" ]]; then
					echo "$filename was ${ideal["$name"]}, now $md5::$size."
				fi;
			done
done <"./$2"