#!/bin/bash
# e.g. sh larger_tests.sh

# Get all the C files in the larger_tests folder
FILES=./*.c

total_size=0
total_time=0

# Run sh simple_run.sh for each file
for f in $FILES
do
    # Remove the .c extension in f
    f=${f%.c}
    echo "Processing $f file..."
    # Execute the run script and save the output to a file
    sh run.sh $f > $f.output

    # Extract the size and time from the output file
    size=$(grep "@@ Size of executable:" $f.output | awk '{print $5}')
    time=$(grep "@@@ Execution time:" $f.output | awk '{print $4}')

    # Add the size and time to the total sum
    total_size=$((total_size + size))
    total_time=$(echo "$total_time + $time" | bc)
done

echo "********** RESULTS **********"

# Print the total sum of sizes and times
echo "=== Total Size ==="
echo "Total size of executables: ${total_size} bytes"

echo "=== Total Execution Time ==="
echo "Total execution time: ${total_time} milliseconds"