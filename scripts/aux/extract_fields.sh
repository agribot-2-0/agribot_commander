#!/usr/bin/env sh
cat raw_log.csv | tail -n+2 | awk -F',' 'BEGIN { OFS="," } { $1=$1 } 1 {print $7,$8}'
