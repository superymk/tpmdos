#!/bin/bash
f=`dirname $0`
sh $f/hourly_send.sh & > /dev/null
$f/tpmdos
