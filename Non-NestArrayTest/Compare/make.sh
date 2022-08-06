rm -rf non-nested-good non-nested-bad
gcc-12 -onon-nested-good -O3 -std=c11 ./non-nested-good.c
gcc-12 -onon-nested-bad -O3 -std=c11 ./non-nested-bad.c
echo "good"
./non-nested-good 1000
echo "bad"
./non-nested-bad 1000