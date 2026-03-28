#!/usr/bin/env bash

total=0
passed=0
for tst in build/bin/tests/*; do
    total=$((total + 1))
    timeout 1s ./$tst &>/dev/null
    exit_status=$?
    result=""
    if [ $exit_status -eq 0 ]; then
        passed=$((passed + 1))
        result="Passed"
    else
        result="Failed"
    fi
    printf "%-20s : %s\n" ${tst##*/} $result
done

echo "$passed/$total tests passed"