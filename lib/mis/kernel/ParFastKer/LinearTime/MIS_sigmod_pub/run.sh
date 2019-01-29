#!/bin/bash

for dir in "CA-GrQc" "CA-CondMat" "Email-EuAll" "Epinions" "slashdot" "dblp" "patent" "Amazon0601" "web-Google" "WikiTalk" "as-skitter" "LiveJournal" "orkut-links" "uk-2002"
do
	for alg in "greedy" "greedy_dynamic" "BDOne" "BDTwo" "LinearTime" "NearLinear"
	do
		./mis ${alg} ../data/${dir} >> log.txt
	done
done
