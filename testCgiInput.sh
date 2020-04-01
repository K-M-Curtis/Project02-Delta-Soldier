#!/bin/bash
function usage
{
    echo "usage : $0 <input file> <cgi program>" 
    echo ".e.g:  $0 1.txt ./echo"
}

if [ $# -ne "2" ]
then
    usage "$0"
    exit -1
fi

len=`wc -c "$1" | awk '{print $1}'`
cat "$1" | CONTENT_LENGTH=$len "$2"
