# Nome do executável (no Linux não precisa de .exe)
TARGET = projeto_minas

# Compilador
CXX = g++

# Flags: 
# -Wall: Avisos
# -O3: Otimização máxima (bom para meta-heurísticas ficarem rápidas)
# -g: Para debug (opcional, pode tirar se quiser velocidade máxima)
CXXFLAGS = -Wall -O3 

# Pega todos os arquivos .cpp da pasta
SRCS = $(wildcard *.cpp)

# Cria a lista de objetos .o
OBJS = $(SRCS:.cpp=.o)

# Regra padrão
all: $(TARGET)

# Linkagem
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Compilação
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpeza
clean:
	rm -f $(OBJS) $(TARGET)