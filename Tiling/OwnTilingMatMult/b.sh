gcc -mavx -mavx2 -mfma -funroll-loops -fopenmp -O3 -omm ./main.c
rm r.txt
echo "512" >> r.txt
./mm 512 >> r.txt
echo "1024" >> r.txt
./mm 1024 >> r.txt
echo "2048" >> r.txt
./mm 2048 >> r.txt
echo "4096" >> r.txt
./mm 4096 >> r.txt
echo "8192" >> r.txt
./mm 8192 >> r.txt
echo "16384" >> r.txt
./mm 16384 >> r.txt
