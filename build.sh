if [ ! -d "./bin" ]; then
  mkdir ./bin
fi

g++ -std=c++20 ./src/*.cpp -o ./bin/game -I ./include