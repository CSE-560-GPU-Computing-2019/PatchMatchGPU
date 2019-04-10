# all:
	# nvcc -std=c++11 conv.cu -o conv
	# g++ cpu.cu -o cpu

cpu_cuda: cpu_cuda.cu
	nvcc cpu_cuda.cu -o cpu_cuda

cpu: cpu.cc
	g++ cpu.cc -o cpu
	# nvcc cpu.cc -o cpucu

test: test2.cc
	g++ test2.cc -o test2
	./test2 70

clean:
	rm cpu test
