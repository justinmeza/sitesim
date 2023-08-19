all:
	clang++ -std=c++11 *.cpp && ./a.out

readme:
	grip -b README.md
