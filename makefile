
all: draw
draw:draw.py GA_B.csv 
	python3 draw.py

GA_B.csv: binary_GA.cpp input.txt
	g++ -O2 -I. -o b.out binary_GA.cpp parameter.h
	./b.out < input.txt > report.txt
input.txt: 
	g++ -O2 -I. -o DataGen.out gen_data.cpp 
	./DataGen.out > input.txt
clean: 
	rm *.txt
	rm *.out
	rm *.csv 
	rm *.png
