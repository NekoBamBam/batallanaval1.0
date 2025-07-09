#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#define TAM 12
#define CANT_BARCOS 3

// Estructura para un jugador
typedef struct Jugador{
    int id;
    char nombre[50];
    char apellido[50];
    char tablero[TAM][TAM];
    char disparos[TAM][TAM];
    int barcos_restantes;
    int es_computadora; // 1 si es computadora
} Jugador;
// Nodo de la lista enlazada de jugadores
typedef struct NodoJugador{
    Jugador jugador;
    struct NodoJugador *siguiente;
} NodoJugador;
// Crea un nuevo nodo de la lista de jugadores
NodoJugador *crearNodoJugador(Jugador nuevoJugador){
    NodoJugador *nuevo = (NodoJugador *)malloc(sizeof(NodoJugador));
    nuevo->jugador = nuevoJugador;
    nuevo->siguiente = NULL;
    return nuevo;
}

// Agrega un jugador a la lista dinamica
void agregarJugador(NodoJugador **lista, Jugador nuevoJugador);
// Muestra todos los jugadores registrados
void mostrarListaJugadores(NodoJugador *lista);
// Guarda todos los jugadores registrados en un archivo de texto
void guardarHistorialEnArchivo(NodoJugador *lista);
// Inicializacin de los procedimientos y funciones
void inicializarTablero(char tablero[TAM][TAM]);
void mostrarTablero(char tablero[TAM][TAM]);
void disparar(Jugador *jugador, Jugador *enemigo, char tableroDisparos[TAM][TAM]);
int contarBarcos(char tablero[12][12]);
void colocarBarcosManual(Jugador *j, int cantidad);
void colocarBarcosAleatorio(Jugador *j, int cantidad);

// Inicio del main
int main()
{
    srand(time(NULL)); // Inicializa la seed aleatoria

    NodoJugador *listaJugadores = NULL;
    int id_jugador = 1;
    char opcion;

    do{
        Jugador j1, j2;
        printf("\n--- NUEVA PARTIDA ---\n");

        // Ingreso de datos jugador 1
        j1.id = id_jugador++;
        printf("Nombre del Jugador 1: ");
        scanf("%s", j1.nombre);
        printf("Apellido del Jugador 1: ");
        scanf("%s", j1.apellido);
        j1.es_computadora = 0;
        inicializarTablero(j1.tablero);
        inicializarTablero(j1.disparos);

        int modo;
        printf("Colocar barcos manual (1) o automatico (2)? ");
        scanf("%d", &modo);

        if (modo == 1){
            colocarBarcosManual(&j1, CANT_BARCOS);
        }else{
            colocarBarcosAleatorio(&j1, CANT_BARCOS);
            }

        agregarJugador(&listaJugadores, j1);

        // Eleccion del tipo de jugador PC o Player
        int tipoJugador2;
        char buffer[10];
        int valido = 0;
        while (getchar() != '\n'); //limpieza del buffer
        do {
            printf("Jugar contra otro jugador (1) o contra la computadora (2)? ");
            fgets(buffer, sizeof(buffer), stdin);
        if (buffer[0] == '1' || buffer[0] == '2') {
            tipoJugador2 = buffer[0] - '0'; // convierte '1' o '2' a entero
            valido = 1;
        }else{
            printf("Opcion invalida. Ingresa 1 o 2.\n");
            }
        }while(!valido);
        //Verificacion de si jugador 2 existe
        j2.id = id_jugador++;
        if (tipoJugador2 == 1){
            printf("Nombre del Jugador 2: ");
            scanf("%s", j2.nombre);
            printf("Apellido del Jugador 2: ");
            scanf("%s", j2.apellido);
            j2.es_computadora = 0;
        }else{
            strcpy(j2.nombre, "Computadora");
            strcpy(j2.apellido, "IA");
            j2.es_computadora = 1;
        }

        inicializarTablero(j2.tablero);
        inicializarTablero(j2.disparos);

        if (j2.es_computadora){
            colocarBarcosAleatorio(&j2, CANT_BARCOS);
        }else{
            printf("Colocar barcos manual (1) o automatico (2)? ");
            scanf("%d", &modo);
            if (modo == 1){
                colocarBarcosManual(&j2, CANT_BARCOS);
            }else{
                colocarBarcosAleatorio(&j2, CANT_BARCOS);
            }
        }

        agregarJugador(&listaJugadores, j2);

        // Bucle del juego
        while (j1.barcos_restantes > 0 && j2.barcos_restantes > 0){
            if (!j1.es_computadora){
                printf("\nTurno de %s:\n", j1.nombre);
                mostrarTablero(j1.disparos);
            }

            disparar(&j1, &j2, j1.disparos);

            if (j2.barcos_restantes <= 0){
                break;
            }

            if (!j2.es_computadora){
                printf("\nTurno de %s:\n", j2.nombre);
                mostrarTablero(j2.disparos);
            }
            disparar(&j2, &j1, j2.disparos);
        }

        // Mostrar ganador
        if (j1.barcos_restantes > 0){
            printf("\nGano %s %s\n", j1.nombre, j1.apellido);
        }else{
            printf("\nGano %s %s\n", j2.nombre, j2.apellido);
        }

        // Partida nueva
        printf("\n Iniciar partida nueva?(s/n): ");
        scanf(" %c", &opcion);

    }while (opcion == 's' || opcion == 'S');

    mostrarListaJugadores(listaJugadores);
    guardarHistorialEnArchivo(listaJugadores);

    return 0;
}

void inicializarTablero(char tablero[TAM][TAM]){
    for (int i = 0; i < TAM; i++)
        for (int j = 0; j < TAM; j++)
            tablero[i][j] = '~';
}

void guardarHistorialEnArchivo(NodoJugador *lista){
    FILE *archivo = fopen("historial.txt", "w");
    if (archivo == NULL){
        printf("No se pudo abrir el archivo para guardar.\n");
        return;
    }

    while (lista != NULL){
        fprintf(archivo, "ID: %d - %s %s%s\n", lista->jugador.id,
                lista->jugador.nombre,
                lista->jugador.apellido,
                lista->jugador.es_computadora ? " (Computadora)" : "");
        lista = lista->siguiente;
    }

    fclose(archivo);
    printf("Historial guardado en 'historial.txt'.\n");
}

void agregarJugador(NodoJugador **lista, Jugador nuevoJugador){
    NodoJugador *nuevo = crearNodoJugador(nuevoJugador);
    if (*lista == NULL){
        *lista = nuevo;
    }
    else{
        NodoJugador *aux = *lista;
        while (aux->siguiente != NULL)
            aux = aux->siguiente;
        aux->siguiente = nuevo;
    }
}

void mostrarListaJugadores(NodoJugador *lista){
    printf("\n=== HISTORIAL DE JUGADORES ===\n");
    int contador = 0;
    while (lista != NULL){
        printf("Jugador ID %d: %s %s%s\n", lista->jugador.id,
               lista->jugador.nombre,
               lista->jugador.apellido,
               lista->jugador.es_computadora ? " (Computadora)" : "");
        lista = lista->siguiente;
        contador++;
    }
    printf("Cantidad total de jugadores: %d\n", contador);
}

// Muestra el tablero por consola
void mostrarTablero(char tablero[TAM][TAM]){
    printf("   ");
    for (int i = 0; i < TAM; i++)
        printf("%2d", i);
    printf("\n");
    for (int i = 0; i < TAM; i++)
    {
        printf("%2d ", i);
        for (int j = 0; j < TAM; j++)
        {
            printf("%2c", tablero[i][j]);
        }
        printf("\n");
    }
}

int contarBarcos(char tablero[12][12]){
    int barcos = 0;
    int visitado[12][12] = {0};

    for (int i = 0; i < 12; i++){
        for (int j = 0; j < 12; j++){
            if (tablero[i][j] == 'B' && !visitado[i][j]){
                barcos++;

                int tam = 1;
                while (j + tam < 12 && tablero[i][j + tam] == 'B'){
                    visitado[i][j + tam] = 1;
                    tam++;
                }

                if (tam == 1){
                    while (i + tam < 12 && tablero[i + tam][j] == 'B'){
                        visitado[i + tam][j] = 1;
                        tam++;
                    }
                }
            }
        }
    }
    return barcos;
}
//Barcos aleatorios
void colocarBarcosAleatorio(Jugador *j, int cantidad)
{
    int colocados = 0;
    while (colocados < cantidad){
        int fila = rand() % TAM;
        int col = rand() % TAM;
        int direccion = rand() % 2; // 0 = horizontal, 1 = vertical
        int longitud = 3;           // tamaño del barco
        int puedeColocar = 1;

        // verificacion de espacio disponible en el tablero
        for (int i = 0; i < longitud; i++){
            int f = fila + (direccion == 1 ? i : 0);
            int c = col + (direccion == 0 ? i : 0);
            if (f >= TAM || c >= TAM || j->tablero[f][c] != '~'){
                puedeColocar = 0;
                break;
            }
        }

        // si puede colocar, lo hace
        if (puedeColocar){
            for (int i = 0; i < longitud; i++)
            {
                int f = fila + (direccion == 1 ? i : 0);
                int c = col + (direccion == 0 ? i : 0);
                j->tablero[f][c] = 'B';
            }
            colocados++;
        }
    }
    j->barcos_restantes = cantidad * 3;
}
// Barcos manuales
void colocarBarcosManual(Jugador *j, int cantidad)
{
    int fila, col;
    char direccion;
    int longitud = 3; // igual que arriba

    for (int b = 0; b < cantidad; b++){
        int valido = 0;
        while (!valido)
        {
            mostrarTablero(j->tablero);
            printf("\nBarco %d - Ingresa fila (0 a %d), columna (0 a %d), direccion (H/V)  --- H: Horizontal V: Vertical --- : ", b + 1, TAM - 1, TAM - 1);
            scanf("%d %d", &fila, &col);
            while (getchar() != '\n'); // limpia buffer
            printf("Direccion (H/V): ");
            scanf("%c", &direccion);
            direccion = toupper(direccion);
            //Validacion de la direccion Horizontal o Vertical
            if (direccion != 'H' && direccion != 'V'){
                printf("Direccion invalida. Ingrese H o V.\n");
                continue;
                }
            int puedeColocar = 1;
            for (int i = 0; i < longitud; i++){
                int f = fila + (direccion == 'V' ? i : 0);
                int c = col + (direccion == 'H' ? i : 0);
                if (f >= TAM || c >= TAM || j->tablero[f][c] != '~'){
                    puedeColocar = 0;
                    break;
                }
            }

            if (puedeColocar){
                for (int i = 0; i < longitud; i++)
                {
                    int f = fila + (direccion == 'V' ? i : 0);
                    int c = col + (direccion == 'H' ? i : 0);
                    j->tablero[f][c] = 'B';
                }
                valido = 1;
            }else{
                printf("Posicion invalida, proba de nuevo.\n");
            }
        }
    }
    j->barcos_restantes = cantidad * 3;
    //Genera un salto de linea al terminar de colocar los barcos
    for (int i = 0; i < 50; i++) printf("\n");
}


// Realiza un disparo y marca el resultado
void disparar(Jugador *jugador, Jugador *enemigo, char tableroDisparos[TAM][TAM]){
    int fila, columna;
    int disparo_valido = 0;
    while (!disparo_valido){
        if (jugador->es_computadora){
            fila = rand() % TAM;
            columna = rand() % TAM;
            printf("Computadora dispara a (%d, %d)\n", fila, columna);
        }else{
            printf("Ingrese coordenadas de disparo (fila y columna): ");
            scanf("%d %d", &fila, &columna);
        }

        // Validacion de limites del tablero
        if (fila < 0 || fila >= TAM || columna < 0 || columna >= TAM){
            printf("Coordenadas fuera de rango. Intent� de nuevo.\n");
            continue;
        }
        // Chequear si ya disparaste
        if (tableroDisparos[fila][columna] == 'X' || tableroDisparos[fila][columna] == '*'){
            printf("Ya disparaste en esa posicion. Intenta otra.\n");
            continue; // No continua con el disparo
        }

        // Verificar si ya disparo en esa casilla
        if (enemigo->tablero[fila][columna] == 'B'){
            printf("Impacto!\n");
            tableroDisparos[fila][columna] = 'X';
            enemigo->tablero[fila][columna] = 'X'; // para evitar dobles disparos
            enemigo->barcos_restantes--;           // bajos uno
        }else{
            printf("Agua.\n");
            tableroDisparos[fila][columna] = '*';
        }
        disparo_valido = 1;
    }
}
