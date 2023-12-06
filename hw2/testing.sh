make
myArray=("case1.txt" "case2.txt" "case3.txt" "case4.txt" "case5.txt" "case6.txt")
for element in "${myArray[@]}"; do
    ./main testcase/$element output.dat
    Solutionchecker/Solutionchecker testcase/$element output.dat
done