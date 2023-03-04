#!/bin/bash

now=$(date +"%r")
echo "Server start time: $now, "

./replica 1 & ./replica 2 & ./replica 3 & ./replica 4 & ./replica 5 &

# Set value 100 times
for VARIABLE in {1..10}
do
    ./client 100 $VARIABLE >> log.txt &
    sleep 1
    ps -ef | grep "./client 100 $VARIABLE" | grep -v grep | awk '{print $2}' | xargs kill
done


# Read value 100 times
for VARIABLE in {1..10}
do 
    ./client 100 >> log.txt &
    sleep 1
    ps -ef | grep "./client 100" | grep -v grep | awk '{print $2}' | xargs kill
done

ps -ef | grep "./replica 1" | grep -v grep | awk '{print $2}' | xargs kill
ps -ef | grep "./replica 2" | grep -v grep | awk '{print $2}' | xargs kill
ps -ef | grep "./replica 3" | grep -v grep | awk '{print $2}' | xargs kill
ps -ef | grep "./replica 4" | grep -v grep | awk '{print $2}' | xargs kill
ps -ef | grep "./replica 5" | grep -v grep | awk '{print $2}' | xargs kill

total_sets=0
total_gets=0

while IFS= read -r line
do 
    if [[ $line == Writer* ]]
    then
        total_sets=$((total_sets+1))
    fi
    if [[ $line == Read* ]]
    then
        total_gets=$((total_gets+1))
    fi 
done < log.txt


echo "total sets $total_sets done, "
echo "total gets $total_gets done"