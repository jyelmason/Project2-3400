#!/bin/bash

EXE="../ids"
REQ="/comp3400.int.req"
RESP="/comp3400.int.resp"

function run_test {

    # parameters
    TAG=$1
    ARGS=$2

    # file paths
    OUTPUT=outputs/$TAG.txt
    DIFF=outputs/$TAG.diff
    EXPECT=expected/$TAG.txt
    VALGRND=valgrind/$TAG.txt
    SHASUM=outputs/$TAG.shasum

    # run test and compare output to the expected version
    if [ ! -a PID ] ; then
      ./server -p PID $REQ $RESP >/dev/null 2>&1 &
      sleep 1
    fi
    $EXE $ARGS >"$OUTPUT" 2>/dev/null

    PTAG=$(printf '%-30s' "$TAG")
    if [ ! -z $(echo $TAG | grep "kill") ] ; then
      sleep 1
      pidfile="PID"
      if [ ! -z $(echo $TAG | grep "start") ] ; then
        pidfile=$(echo $ARGS | awk '{print $3}')
      fi
      if [ -a $pidfile ] ; then
        echo "$PTAG FAIL (did not kill server)"
      fi
    fi
    if [ ! -z $(echo $TAG | grep "output") ] ; then
      shasum -c expected/shasum.txt >"$SHASUM" 2>&1
      if [ $? -ne 0 ] ; then
        echo "$PTAG FAIL (see $SHASUM for details)"
      fi
    fi

    diff -u "$OUTPUT" "$EXPECT" >"$DIFF"
    if [ -s "$DIFF" ]; then

        # try alternative solution (if it exists)
        EXPECT=expected/$TAG-2.txt
        if [ -e "$EXPECT" ]; then
            diff -u "$OUTPUT" "$EXPECT" >"$DIFF"
            if [ -s "$DIFF" ]; then
                echo "$PTAG FAIL (see $DIFF for details)"
            else
                echo "$PTAG pass"
            fi
        else
            echo "$PTAG FAIL (see $DIFF for details)"
        fi
    else
        echo "$PTAG pass"
    fi

    # run valgrind
    valgrind $EXE $ARGS &>$VALGRND
}

# initialize output folders
mkdir -p outputs
mkdir -p valgrind
rm -f outputs/* valgrind/*


# run individual tests
source itests.include

if [ -a PID ] ; then
  kill -USR1 $(cat PID) 2>&1 >/dev/null
  sleep 1
  rm -f PID
fi
rm -f $REQ
rm -f $RESP

# check for memory leaks
LEAK=`cat valgrind/*.txt | grep 'definitely lost' | grep -v ' 0 bytes in 0 blocks'`
if [ -z "$LEAK" ]; then
    echo "No memory leak found."
else
    echo "Memory leak(s) found. See files listed below for details."
    grep 'definitely lost' valgrind/*.txt | sed -e 's/:.*$//g' | sed -e 's/^/  - /g'
fi

