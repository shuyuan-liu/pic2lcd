CXX = clang++
CXXFLAGS = `libpng-config --cflags`
LDFLAGS = `libpng-config --ldflags`

HEADERS = *.h

.PHONY: clean

pic2lcd: pic2lcd.o
	$(CXX) -o $@ $^ $(LDFLAGS)

pic2lcd.o: pic2lcd.cpp $(HEADERS)
	$(CXX) -c -g -o $@ $< $(CXXFLAGS)

clean:
	rm -rf pic2lcd pic2lcd.o