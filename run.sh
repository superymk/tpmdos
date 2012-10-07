#!/bin/bash
f=`dirname $0`
#sh $f/hourly_send.sh & > /dev/null
sh $f/hourly_send.sh &

folder=`dirname $0`"/tpm-data"

while [ ! -d $folder ] 
do
    sleep 2s
done

#$f/tpmdos
echo "AAAAAA"
