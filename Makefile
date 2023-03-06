build:
	gcc process_generator.c -o process_generator.out -lm
	gcc clk.c -o clk.out -lm
	gcc scheduler.c -o sch.out -lm
	gcc process.c -o process.out -lm
	gcc test_generator.c -o test_generator.out -lm

clean:
	rm -f *.out  processes.txt
	./test_generator.out 

all: build run


run:

	./process_generator.out processes.txt -sch 0 -q 3

