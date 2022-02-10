[[ -d Build ]] || mkdir -p Build

g++ -Wall -Wextra -std=c++20 -Wconversion main.cpp -o Build/main
