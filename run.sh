#!/bin/bash

f=`dirname $0`"/tpm-data"
if [ ! -d $f ]; then
    git clone https://tpmdos:cylabcmu@code.google.com/p/tpm-data2/ tpm-data >/dev/null
    while [ ! -d $f ] 
    do
        sleep 2s
    done
    
    tpm_version > $f/tpm-version-`hostname`
fi

# Execute <tpmdos>
# appfolder=`dirname $0`
sudo ./tpmdos &

# Start uploading result to google code
exec 1>$f/submit_log 2>$f/submit_log

cd $f
# Wait 10s so <tpmdos> can generate some output
sleep 10s
git add tpm-version-`hostname`
git add gnuplot-defineNV-`hostname`
git add gnuplot-read40-`hostname`
git add gnuplot-write40-`hostname`
git add gnuplot-read705-`hostname`
git add gnuplot-write705-`hostname`
git add metadata-`hostname`
#git add log-`hostname`

cd ..

while :
do
    cd $f
    msg="Experiment Result on: "`date` 
    echo $msg
    git commit -a -m "$msg"
    git push --repo https://tpmdos:gR3KV4pE2Sd9@code.google.com/p/tpm-data2/  --all
    sleep 1d
    cd ..
done