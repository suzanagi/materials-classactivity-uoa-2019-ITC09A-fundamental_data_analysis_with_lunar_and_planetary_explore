while read line
do
    ./command/brief.sh ${line} >> ./$2
done < ./$1