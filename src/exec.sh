#!/bin/bash

# Ensures permission and creates the report folder structure
mkdir -p Relatorios_Finais/VNS-Referencia
mkdir -p Relatorios_Finais/VNS-RLP
mkdir -p Relatorios_Finais/AG-RLP
mkdir -p Solucoes

# Compiles the program if it has not been compiled yet
# Runs make only if the file ./executo is missing (the Makefile builds the binary named exec).
if [ ! -f ./executo ]; then
    echo "Compilando o projeto pela primeira vez..."
    make
fi

# Runs the program
echo "Iniciando as execuções das minas..."
# Runs the program from the current folder, so the relative paths (../dados, Relatorios_Finais) resolve.
./exec