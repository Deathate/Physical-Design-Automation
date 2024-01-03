make
myArray=("example.sp" "ASYNC_DFFHx1_ASAP7_75t_R.sp" "case1.sp" "case2.sp" "case3.sp" "case4.sp")
for element in "${myArray[@]}"; do
    ./Lab3 testcase/$element output.dat
    testcase/SolutionChecker testcase/$element output.dat
done
rm output.dat