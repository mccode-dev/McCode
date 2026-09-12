#!/bin/sh
#
# Shellscript snippet for extracting mcdoc info from named components in subdir
#
# NOTE: updated for the Python `mcdoc` front-end (tools/Python/mcdoc). The
# legacy Perl `mcdoc.pl --exact -t COMP.comp` invocation this script used to
# rely on no longer exists in the McCode 3.x tree; `mcdoc` is the current
# tool, and does not support a `-t`/exact-text mode identical to the old one,
# so we parse the component's own %P section text out of the .comp file
# header directly instead of shelling out to mcdoc. This keeps the manual's
# per-component .parms snippets (used by e.g. sources/Source_gen.tex via
# \input{sources/Source_gen.parms}) buildable without depending on a
# discontinued tool.
#

echo "$0 $1 $2 $3"
pwd
ls $1*
DIRNAME=`basename $1`
PW=$PWD
SRCDIR=$2
echo "PW=$PWD, DN=$DIRNAME, SD=$SRCDIR"
PREFIX=$SRCDIR/COMPprefix
HEADER=$SRCDIR/COMPheader
FOOTER=$SRCDIR/COMPfooter

#search upwards for comps directory
COMP_SOURCE_DIR=$PW
while [ ! -d $COMP_SOURCE_DIR/mcstas-comps ] && [ "x$COMP_SOURCE_DIR" != "x" ]
do
  COMP_SOURCE_DIR=${COMP_SOURCE_DIR%/*}
done

for COMP in `cat $SRCDIR/$DIRNAME/mcdoc_index`
do
    echo "get_mcdoc_snippets: generating $DIRNAME/${COMP}.parms"
    sed s/@COMP@/$COMP/g $PREFIX | sed s/@CAT@/$DIRNAME/g > $DIRNAME/$COMP.parms
    cat $HEADER >> $DIRNAME/$COMP.parms
    # Find the .comp file anywhere under mcstas-comps (category subfolder may
    # have moved between releases, e.g. contrib -> optics), then extract the
    # text strictly between the %PARAMETERS and %END (or next %-keyword)
    # markers of its McDoc header, one parameter per non-blank, non-comment
    # line -- equivalent in spirit to the old
    # `mcdoc.pl --exact -t COMP.comp | grep -A1000 '# Input' | grep -B1000 '# Output'`
    # pipeline, but driven directly off the component source rather than a
    # (no longer available) mcdoc text-rendering mode.
    COMPFILE=$(find $COMP_SOURCE_DIR/mcstas-comps -maxdepth 2 -name "${COMP}.comp" | head -n1)
    if [ -z "$COMPFILE" ]; then
      echo "get_mcdoc_snippets: WARNING - could not find ${COMP}.comp under $COMP_SOURCE_DIR/mcstas-comps"
    else
      awk '
        /^\*[ \t]*%P(ARAMETERS)?[ \t]*$/ {inP=1; next}
        /^\*[ \t]*%[A-Za-z]/ {inP=0}
        inP && /^\*/ {sub(/^\* ?/,""); if ($0 !~ /^[ \t]*$/) print}
      ' "$COMPFILE" >> $PW/$DIRNAME/$COMP.parms
    fi
    cat $FOOTER >> $DIRNAME/$COMP.parms
done
touch $DIRNAME.done
