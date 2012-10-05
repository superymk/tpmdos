#!/bin/bash

f=`dirname $0`"/tpm-data"
echo $f
if [ ! -d $f ]; then
    git clone https://tpmdos:cylabcmu@code.google.com/p/tpm-data/ >/dev/null
fi

exec 1>$f/submit_log 2>$f/submit_log

cd $f
git add $f/test
git add $f/metadata-`hostname`
git add $f/log-`hostname`
cd ..

while :
do
    cd $f
    msg="Experiment Result on: "`date` 
    echo $msg
    git commit -a -m "$msg"
    git push --repo https://tpmdos:gR3KV4pE2Sd9@code.google.com/p/tpm-data/ 
    sleep 10s
    cd ..
done