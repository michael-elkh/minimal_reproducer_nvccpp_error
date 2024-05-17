FLAGS+=-std=c++20 -O3
NVFLAGS+=-stdpar=gpu 
CC:=nvc++
.PHONY=run clean

NVIDIA:=bin/nv_minimal_error
GCC:=bin/gcc_minimal_error

HEADERS:=cartesian_product.hpp

all: $(NVIDIA) $(GCC)

run: all
	@echo "WITH NVC++" 
	@./$(NVIDIA)
	@echo "WITH G++" 
	@./$(GCC)

$(NVIDIA): main.cpp $(HEADERS)
	$(CC) $(FLAGS) $(NVFLAGS) $< -o $@ $(LIBS)

$(GCC): main.cpp $(HEADERS)
	g++ $(FLAGS) $< -o $@ -ltbb

clean:
	rm -rvf ./bin/**