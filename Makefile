all: gpu cpu

gpu: gpu.cu
	nvcc gpu.cu -o gpu

cpu: cpu.cc
	g++ cpu.cc -o cpu

test: test2.cc
	g++ test2.cc -o test2
	./test2 70

clean:
	rm cpu gpu
