all: makeup

makeup: makeup.cpp ui.cpp
	g++ -o makeup $^  -W  -g

clean:
	rm makeup
