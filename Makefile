# all:
	# nvcc -std=c++11 conv.cu -o conv
	# g++ cpu.cu -o cpu

cpu: cpu.cc
	g++ cpu.cc -o cpu

clean:
	rm cpu
