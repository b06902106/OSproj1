#!/bin/bash
tasks="TIME_MEASUREMENT FIFO PSJF RR SJF"
num="1 2 3 4 5"
for task in FIFO PSJF RR SJF; do
    for num in 1 2 3 4 5; do
        echo "${task}_${num}.txt"
        sudo ./scheduler < test/${task}_${num}.txt > output/${task}_${num}_stdout.txt
        sudo dmesg | grep Project1 > output/${task}_${num}_dmesg.txt
        sudo dmesg -C
    done
done
