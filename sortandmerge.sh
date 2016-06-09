#!/bin/bash

sorter="/home/NiGIRI/data4eb/code/sorter"
merger="/home/NiGIRI/data4eb/code/merger"
path="/home/NiGIRI/data1/sync_test_eurica"
if [ $# -eq 1 ]
then
    echo "doing run number" $1
    for i in `seq 1 5`;
    do
        $sorter -i $path/run$1_sync_test_m$i.nig -o $path/run$1_sorted_m$i.nig -m 10000 -t 0
    done   
    echo "sorting run" $1 "done."
    $merger -i $path/run$1_sorted -o $path/run$1_merged.root -n 5 -t 1
    echo "merging run" $1 "done."
else
    echo "usage: ./sortandmerge.sh <runnumber>"
fi