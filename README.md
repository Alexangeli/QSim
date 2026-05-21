QSim - Simulatore di circuiti quantistici
==========================================

File inclusi:
- src/main.c       : entry point, gestione argomenti CLI
- src/parser.c     : parsing dei file di input
- src/complex.c    : operazioni sui numeri complessi
- src/matrix.c     : operazioni sulle matrici
- src/thread.c     : logica multi-thread

Compilazione:
  make

Utilizzo:
  ./qsim -s stato.txt -c circuito.txt -t <num_thread>

Esempio:
  ./qsim -s tests/init.txt -c tests/circ.txt -t 4