all: makeup

makeup: makeup.cpp ui.cpp filehandler.cpp
	g++ -o makeup $^  -W  -g

clean:
	rm makeup
