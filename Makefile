all:
	clang++ -std=c++20 *.cpp && ./a.out

readme:
	grip -b README.md
