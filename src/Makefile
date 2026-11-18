# Name of the output executable
TARGET = exec

# Compiler and optimization flags (ignoring the specific warnings listed)
# C++ compiler used to build the project.
CXX = g++
CXXFLAGS = -O3 -Wall -std=c++11 \
           -Wno-misleading-indentation \
           -Wno-comment \
           -Wno-unused-result \
           -Wno-unused-variable \
           -Wno-unused-but-set-variable

# Automatically lists all .cpp files in the current directory
SRCS = $(wildcard *.cpp)

# Converts the list of .cpp files into .o objects
OBJS = $(SRCS:.cpp=.o)

# Main rule (default)
all: $(TARGET)

# Links the final executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Individual compilation rule for each .cpp file -> .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Automatic creation of the directory structure for reports and solutions
dirs:
	mkdir -p Relatorios_Finais/VNS-Referencia
	mkdir -p Relatorios_Finais/VNS-RLP
	mkdir -p Relatorios_Finais/AG-RLP
	mkdir -p Solucoes

# Convenience command: creates the folders, compiles and runs everything (make run)
run: dirs all
	./$(TARGET)

# Cleanup of the generated binary files
clean:
	rm -f $(OBJS) $(TARGET)

# Targets that are action names rather than files; they always run.
.PHONY: all run clean dirs