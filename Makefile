BDIR := ./build

CC := mpicc
CFLAGS := -O3 -fopenmp

$(shell mkdir -p $(BDIR))

ALL_SRC := $(shell find . -type f -regex ".*\.c")
ALL_OBJ := $(patsubst %.c, %.o, $(ALL_SRC))

MPI_OBJ := $(filter ./mpi_openmp/%.o, $(ALL_OBJ))
COMMON_OBJ := $(filter ./common/%.o, $(ALL_OBJ))

all: $(BDIR)/main.x

$(BDIR)/main.x: $(ALL_OBJ)
	$(CC) $(CFLAGS) $(MPI_OBJ) $(COMMON_OBJ) -o $@ -lm
	rm -rf $(MPI_OBJ)

.PHONY: clean

clean:
	rm -rf $(BDIR) $(ALL_OBJ)

$(VERBOSE).SILENT:
