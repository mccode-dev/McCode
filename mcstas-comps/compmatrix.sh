#!/usr/bin/env bash
#
# Shellscript to investigate use of comps in the example instrs.
#   Usage: ./compmatrix.sh
#   Outputs:
#     comps_no_examples.txt
#       (list of comps with 0 examples, disregarding sasmodels, obsolete)
#     sources/sources_examples.txt
#       (list of source comps with their examples)
#     optics/optics_examples.txt
#       (list of optics comps with their examples)
#     etc... 

COMPDIR=$PWD
echo --------------------------------------- > ${COMPDIR}/comps_no_examples.txt
echo Comps with no examples \(disregarding   >> ${COMPDIR}/comps_no_examples.txt
echo  sasmodels and obsolete categories\)    >> ${COMPDIR}/comps_no_examples.txt
echo --------------------------------------- >> ${COMPDIR}/comps_no_examples.txt
# "instr-path comp" for every "= comp(" outside C comments, at any depth below examples/
INDEX=$(find ${COMPDIR}/examples -name '*.instr' -print0 | xargs -0 perl -0777 -ne \
  's{/\*.*?\*/}{}gs; s{//[^\n]*}{}g; print "$ARGV $1\n" while /=\s*(\w+)\s*\(/g')
for category in $(find . -name \*.comp | cut -d / -f2 | sort | uniq | grep -v -x -e parked -e examples)
do
    echo Missing examples from ${category} >> ${COMPDIR}/comps_no_examples.txt
    cd ${COMPDIR}/${category}
    echo --------------------------------------- > ${category}_examples.txt
    echo List of $category comps and the         >> ${category}_examples.txt
    echo example instruments that include them   >> ${category}_examples.txt
    echo --------------------------------------- >> ${category}_examples.txt
    for compfile in *.comp
    do
	comp=${compfile%.comp}
	examples=$(echo "$INDEX" | awk -v c="$comp" '$2 == c {print $1}' | sort -u | sed 's|.*/||')
	num_examples=$(echo $examples | wc -w)
	echo $comp is used in $num_examples example\(s\): >> ${category}_examples.txt
	echo $examples | xargs -n1 echo - >> ${category}_examples.txt
	echo --------------------------------------- >> ${category}_examples.txt
	if [ $num_examples -lt 1 ]
	then
	    if [ $category != "sasmodels" ]
	    then
		if [ $category != "obsolete" ]
		then
		    echo - $comp in $category is not in any example >> ${COMPDIR}/comps_no_examples.txt
		fi
	    fi
	fi
    done
    if [ $category == "sasmodels" ] || [ $category == "obsolete" ]
    then
	echo ... Not reporting comps in $category >> ${COMPDIR}/comps_no_examples.txt
    fi
    echo --------------------------------------- >> ${COMPDIR}/comps_no_examples.txt
done
