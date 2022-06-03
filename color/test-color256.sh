#!/bin/sh

# test visual representaion of CSI both mono and color
#
# for xterm256 and linux console


xf=36
for pre in '38;5;' '48;5;'
do
n=0
while [ $n -le 255 ]
do
    d40=`expr $n % $xf`
    if [ $d40 -eq 0 ]; then
	head=`expr \( $n / $xf \) \* $xf`
	printf "\e[m $head"
	echo ''
    fi
    d10=`expr $n % 10`
    #printf "\e[%s%sm%2d" $pre $n $d10
    printf "\e[%s%sm%2x" $pre $n $n
    n=`expr $n + 1`
done
done
printf "\e[m --"
echo ''

for pre in '38;2' '48;2'
do
for n in 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
do
    printf "\e[%s;%s;%s;%sm %2d" $pre $n $n $n $n
done
printf "\e[m"
echo ''
done
