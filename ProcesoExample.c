#include <stdio.h>
#include <unistd.h>

int main() {
    pid_t pid = fork(); // Crea un nuevo proceso

    if (pid < 0) {
        // Error al crear el proceso
        perror("Error en fork");
        return 1;
    } else if (pid == 0) {
        // Código del proceso hijo
        printf("Soy el proceso hijo. Mi PID es %d\n", getpid());
    } else {
        // Código del proceso padre
        printf("Soy el proceso padre. Mi PID es %d y el PID de mi hijo es %d\n", getpid(), pid);
    }

    return 0;
}
