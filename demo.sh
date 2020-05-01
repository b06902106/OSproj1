#!/bin/bash
tasks="TIME_MEASUREMENT FIFO_1 PSJF_2 RR_3 SJF_4"
for task in TIME_MEASUREMENT FIFO_1 PSJF_2 RR_3 SJF_4; do
    echo "------------------------------------------------"
    echo "${task}"
    sudo dmesg -C
    sudo ./scheduler < test/${task}.txt
    sudo dmesg | grep Project1
done
