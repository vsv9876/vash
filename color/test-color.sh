#!/bin/sh

# test visual representaion of CSI both mono and color
#
# for xterm256 and linux console


for comb in 0 1 2 3 4 5 6 7 8 9
do
for pre in 0 1 2 3 4 5 6 7 8 9
do
#    if [ $comb -lt $pre ]; then
#        continue
#    fi
    printf "\e[%s;%sm" $pre $comb
    printf " %s;%s " $pre $comb
    printf "\e[m"
done
echo ''
done

echo " -- "
#for pre in '' 3 4 9 10
for pre in 3 4 9 10
do
    printf "\e[m"
    for n in 0 1 2 3 4 5 6 7 9
    do
	printf "\e[%s%sm" $pre $n
	printf " %3s%s " $pre $n
        printf "\e[m"
    done
    echo ''
done
printf "\e[m"
#echo ''
