#!/bin/bash
echo '> Usage:' $0 '<input_file> [precision] [max_cluster_size]';

declare -a ins
declare -a outs

strPos=36;
strLen=9;

for (( clusterSize=1; $clusterSize <= $3; clusterSize+=$2 ))
do
	ins=( "${ins[@]}" $clusterSize)
	out=$( ./fitcircles $1 5 $clusterSize | grep 'time')
	outs=( "${outs[@]}" ${out:$strPos:$strLen})
done

echo "Cluster sizes: [" ${ins[@]} "]"
echo "Execution times: [" ${outs[@]} "]"
