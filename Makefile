all:
	@echo "Let's solve a Rubik Cube!"

cubepos:
	g++ cubepos/*.cpp -o cubepos
	./cubepos