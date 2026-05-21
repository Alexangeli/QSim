#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

    char *state_file = NULL;
    char *circuit_file = NULL;
    int n_threads = 1;
    int opt;

    while ((opt = getopt(argc, argv, "s:c:t:")) != -1) {
        switch (opt) {
            case 's':
                state_file = optarg;
                break;
            case 'c':
                circuit_file = optarg;
                break;
            case 't':
                n_threads = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s -s <state_file> -c <circuit_file> -t <n_threads>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (!state_file || !circuit_file) {
        fprintf(stderr, "Error: State file -s and circuit file -c are required.\n");
        exit(EXIT_FAILURE);
    }

    printf("State:   %s\n", state_file);
    printf("Circuit: %s\n", circuit_file);
    printf("Threads: %d\n", n_threads);

    return 0;

}
