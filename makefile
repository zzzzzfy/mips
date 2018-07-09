cc=g++
prom = code
deps = mips.cpp

$(prom):$(deps) 
	g++ -o $(prom) $(deps) -O2 -std=c++17


clean:
	rm -rf $(obj) $(prom)
