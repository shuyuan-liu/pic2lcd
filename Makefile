CXX = g++
CXXFLAGS = `libpng-config --cflags` -O3
LDFLAGS = `libpng-config --ldflags`

HEADERS = *.h

.PHONY: clean remake

pic2lcd: pic2lcd.o
	$(CXX) -o $@ $^ $(LDFLAGS)

pic2lcd.o: pic2lcd.cpp $(HEADERS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

clean:
	rm -rf pic2lcd pic2lcd.o

remake: clean pic2lcd