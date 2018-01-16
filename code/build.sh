pushd ../build
gcc -c ../code/pi3_ft800.cpp -o libpi3gd2.so --shared -fPIC
gcc -o test ../code/pi3_main.cpp -L ../build -l:libpi3gd2.so -l wiringPi
# gcc -o test ../code/pi3_main.cpp -l wiringPi
popd
