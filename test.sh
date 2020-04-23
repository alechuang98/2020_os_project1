sudo dmesg -C
./main < test_data/$1_1.txt > output/$1_1_stdout.txt
dmesg | grep Project1 > output/$1_1_dmesg.txt
sudo dmesg -C
./main < test_data/$1_2.txt > output/$1_2_stdout.txt
dmesg | grep Project1 > output/$1_2_dmesg.txt
sudo dmesg -C
./main < test_data/$1_3.txt > output/$1_3_stdout.txt
dmesg | grep Project1 > output/$1_3_dmesg.txt
sudo dmesg -C
./main < test_data/$1_4.txt > output/$1_4_stdout.txt
dmesg | grep Project1 > output/$1_4_dmesg.txt
sudo dmesg -C
./main < test_data/$1_5.txt > output/$1_5_stdout.txt
dmesg | grep Project1 > output/$1_5_dmesg.txt
sudo dmesg -C
./main < test_data/TIME_MEASUREMENT.txt > output/TIME_MEASUREMENT_stdout.txt
dmesg | grep Project1 > output/TIME_MEASUREMENT_dmesg.txt
