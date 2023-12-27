make
myArray=("example.sp" "ASYNC_DFFHx1_ASAP7_75t_R.sp")
for element in "${myArray[@]}"; do
    ./Lab3 testcase/$element output.dat
    testcase/SolutionChecker testcase/$element output.dat
done