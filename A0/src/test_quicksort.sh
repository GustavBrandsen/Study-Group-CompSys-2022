#!/bin/sh

# Set this variable to the path to rars.jar.  Avoid backslashes.  You
# can make it "rars.jar" if you always have rars.jar in the current
# directory.
RARS_JAR="/Users/gustavbrandsen/Downloads/rars_27a7c1f.jar"

export GREEN=$(printf '\033[1;32m')
export RED=$(printf '\033[1;31m')
export NC=$(printf '\033[0m')

if which rars >/dev/null; then
    run_rars() {
        rars "$@"
    }
elif [ -f "$RARS_JAR" ]; then
    if which java >/dev/null; then
        run_rars() {
            java -jar "$RARS_JAR" "$@"
        }
    else
        echo "RARS_JAR set, but 'java' executable is not available on PATH."
        exit 1
    fi
else
    echo "RARS_JAR is set to '$RARS_JAR', but this file does not exist."
    exit 1
fi

if [ $# -ne 2 ]; then
    echo "Usage: $0 ASSEMBLYFILE DATAFILE"
    exit 1
fi

assemblyfile=$1
datafile=$2

n=$(wc -l < "$datafile")

output=$( (echo "$n"; cat "$datafile") | run_rars se1 ic "$assemblyfile")

if ! (echo "$output" | grep -q 'Program terminated by calling exit'); then
    echo "$datafile: ${RED}RARS failed:${NC}"
    echo "$output"
    exit 1
fi

numbers=$(echo "$output" | grep -E '^[0-9]+$')
result=$(echo "$numbers" | head -n "$n")
instructions=$(echo "$numbers" | tail -n 1)

if [ "$(sort -n < "$datafile")" = "$result" ]; then
    echo "$datafile: ${GREEN}Correct result.${NC} Instructions executed: $instructions"
else
    echo "$datafile: ${RED}Invalid result.${NC}"
fi
