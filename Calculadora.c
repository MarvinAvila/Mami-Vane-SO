#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h> // Para wait()

int main() {
    pid_t pid;
    int opcion;
    float num1, num2, resultado;

    do {
        printf("\n=== Calculadora con procesos hijos ===\n");
        printf("1. Suma\n");
        printf("2. Resta\n");
        printf("3. Multiplicación\n");
        printf("4. División\n");
        printf("5. Salir\n\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);

        if (opcion >= 1 && opcion <= 4) {
            printf("\nIngrese el primer número: ");
            scanf("%f", &num1);
            printf("Ingrese el segundo número: ");
            scanf("%f", &num2);

            pid = fork(); // Se crea un hijo para la operación

            if (pid < 0) {
                perror("Error en fork");
                return 1;
            } else if (pid == 0) {
                // Proceso hijo hace la operación
                printf("\nSoy el hijo con PID %d y mi padre es %d\n", getpid(), getppid());

                if (opcion == 1) {
                    resultado = num1 + num2;
                    printf("Resultado de la suma: %.2f\n", resultado);
                } else if (opcion == 2) {
                    resultado = num1 - num2;
                    printf("Resultado de la resta: %.2f\n", resultado);
                } else if (opcion == 3) {
                    resultado = num1 * num2;
                    printf("Resultado de la multiplicación: %.2f\n", resultado);
                } else if (opcion == 4) {
                    if (num2 != 0) {
                        resultado = num1 / num2;
                        printf("Resultado de la división: %.2f\n", resultado);
                    } else {
                        printf("Error: división por cero no permitida.\n");
                    }
                }

                printf("Hijo PID %d termina.\n", getpid());
                return 0; // Importante: terminar el hijo aquí
            } else {
                // Proceso padre
                printf("Soy el padre (PID %d). Creé al hijo (PID %d) para la operación.\n", getpid(), pid);
                wait(NULL); // El padre espera al hijo
                printf("El hijo (PID %d) terminó, ahora vuelve el padre.\n", pid);
            }
        }

    } while (opcion != 5);

    printf("\nEl proceso padre termina.\n");

    return 0;
}
