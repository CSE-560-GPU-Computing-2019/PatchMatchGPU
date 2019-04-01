# all:
	# nvcc -std=c++11 conv.cu -o conv
	# g++ cpu.cu -o cpu

cpu: cpu.cc
	g++ cpu.cc -o cpu

test: test2.cc
	g++ test2.cc -o test2
	./test2 70

clean:
	rm cpu test
