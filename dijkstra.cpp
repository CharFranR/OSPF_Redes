#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <queue>    // Necesario para la cola de prioridad de Dijkstra
#include <limits>   // Necesario para definir "infinito"
#include <algorithm>// Necesario para invertir el camino final

// Usamos el valor máximo de entero como "Infinito"
const int INF = std::numeric_limits<int>::max();

class grafo {
private:
    // nodo: lista de adyacencia.
    // Formato: [idVecino1, peso1, idVecino2, peso2, ...]
    std::vector<std::vector<int>> nodo;
    // departamentos: almacena los nombres de los nodos (Router A, Router 1, etc.)
    std::vector<std::string> departamentos;

public:
    grafo() {
        nodo = {};
        departamentos = {};
    }

    ~grafo() {
        for (size_t i = 0; i < nodo.size(); i++) {
            nodo[i].clear();
        }
        nodo.clear();
        departamentos.clear();
    }

    // Método auxiliar para obtener el ID numérico dado el nombre (necesario para Dijkstra)
    int obtenerIdNodo(std::string nombre) {
        for (size_t i = 0; i < departamentos.size(); i++) {
            if (departamentos[i] == nombre) {
                return i;
            }
        }
        return -1; // No encontrado
    }

    void agregarNodo(std::string nombre) {
        departamentos.push_back(nombre);
        nodo.push_back({});
    }

    void agregarVertices(std::string origen, std::string destino, int peso) {
        int idOrigen = obtenerIdNodo(origen);
        int idDestino = obtenerIdNodo(destino);

        if (idOrigen == -1 || idDestino == -1) {
            std::cout << "Error: Uno de los nodos no existe." << std::endl;
            return;
        }

        // Agregamos vecino y peso (Grafo no dirigido: ida y vuelta)
        nodo[idOrigen].push_back(idDestino);
        nodo[idOrigen].push_back(peso);

        nodo[idDestino].push_back(idOrigen);
        nodo[idDestino].push_back(peso);
    }

    // --- IMPLEMENTACIÓN DEL ALGORITMO OSPF (DIJKSTRA) ---
    void calcularRutaMasCorta(std::string inicio, std::string fin) {
        int idInicio = obtenerIdNodo(inicio);
        int idFin = obtenerIdNodo(fin);

        if (idInicio == -1 || idFin == -1) {
            std::cout << "Nodos de inicio o fin no válidos." << std::endl;
            return;
        }

        int n = departamentos.size();
        
        // 1. Inicialización de distancias
        std::vector<int> distancias(n, INF); // Todos a infinito
        std::vector<int> previo(n, -1);      // Para reconstruir el camino
        std::vector<bool> visitado(n, false);

        // La distancia al origen es 0
        distancias[idInicio] = 0;

        // Cola de prioridad para seleccionar el nodo con menor distancia
        // Guarda pares: <Distancia, ID_Nodo>
        // "greater" hace que el top() sea el menor elemento (Min-Heap)
        std::priority_queue<std::pair<int, int>, 
                            std::vector<std::pair<int, int>>, 
                            std::greater<std::pair<int, int>>> pq;

        pq.push({0, idInicio});

        std::cout << "\n--- Iniciando calculo SPF (Dijkstra) desde " << inicio << " ---\n";

        while (!pq.empty()) {
            int u = pq.top().second;
            int d = pq.top().first;
            pq.pop();

            // Si ya encontramos un camino más corto antes a este nodo, ignorar
            if (d > distancias[u]) continue;

            // Si llegamos al destino, podemos detenernos (opcional, OSPF real calcula todo)
            if (u == idFin) break; 

            // Explorar vecinos
            // Recordar: tu estructura es [vecino, peso, vecino, peso...]
            for (size_t k = 0; k < nodo[u].size(); k += 2) {
                int v = nodo[u][k];        // ID del vecino
                int peso = nodo[u][k+1];   // Peso del enlace

                // Relajación de la arista
                if (distancias[u] + peso < distancias[v]) {
                    distancias[v] = distancias[u] + peso;
                    previo[v] = u; // Guardamos de dónde venimos
                    pq.push({distancias[v], v});
                }
            }
        }

        // --- RECONSTRUCCIÓN E IMPRESIÓN DE LA RUTA ---
        if (distancias[idFin] == INF) {
            std::cout << "No hay ruta posible entre " << inicio << " y " << fin << std::endl;
        } else {
            std::cout << "Ruta mas corta encontrada!\n";
            std::cout << "Costo Total (Latencia): " << distancias[idFin] << " ms\n";
            
            std::vector<std::string> camino;
            for (int at = idFin; at != -1; at = previo[at]) {
                camino.push_back(departamentos[at]);
            }
            std::reverse(camino.begin(), camino.end());

            std::cout << "Ruta: ";
            for (size_t i = 0; i < camino.size(); i++) {
                std::cout << camino[i];
                if (i < camino.size() - 1) std::cout << " -> ";
            }
            std::cout << std::endl;
        }
    }

    void imprimirListaAdyacencia() {
        std::cout << "\n--- Topologia de la Red (LSDB) ---" << std::endl;
        for (size_t i = 0; i < departamentos.size(); i++) {
            std::cout << "Router " << std::setw(4) << departamentos[i] << " conecta con: ";
            for (size_t j = 0; j < nodo[i].size(); j += 2) {
                std::cout << "[" << departamentos[nodo[i][j]] 
                          << " | " << nodo[i][j+1] << "ms] ";
            }
            std::cout << std::endl;
        }
    }
};

int main() {
    grafo OSPF_Net;

    // 1. CARGA DE NODOS (Según tu imagen)
    // Nodos principales
    OSPF_Net.agregarNodo("A");  // ULSA (Rojo)
    OSPF_Net.agregarNodo("B");  // Chapi (Azul)
    
    // Nodos intermedios numéricos
    OSPF_Net.agregarNodo("1");
    OSPF_Net.agregarNodo("2");
    OSPF_Net.agregarNodo("3");
    OSPF_Net.agregarNodo("4");
    OSPF_Net.agregarNodo("5");
    OSPF_Net.agregarNodo("6");
    OSPF_Net.agregarNodo("7");
    OSPF_Net.agregarNodo("8");
    OSPF_Net.agregarNodo("9");
    OSPF_Net.agregarNodo("10");

    // 2. CARGA DE CONEXIONES (Pre-cargado en el Main como pediste)
    // Conexiones desde A (ULSA)
    OSPF_Net.agregarVertices("A", "1", 6);
    OSPF_Net.agregarVertices("A", "2", 11);
    OSPF_Net.agregarVertices("A", "3", 10);
    OSPF_Net.agregarVertices("A", "4", 12);

    // Conexiones desde 1
    OSPF_Net.agregarVertices("1", "2", 30);
    OSPF_Net.agregarVertices("1", "5", 35);
    OSPF_Net.agregarVertices("1", "6", 125);

    // Conexiones desde 2
    OSPF_Net.agregarVertices("2", "3", 21);
    OSPF_Net.agregarVertices("2", "6", 50);
    OSPF_Net.agregarVertices("2", "7", 90);

    // Conexiones desde 3
    OSPF_Net.agregarVertices("3", "7", 70);
    OSPF_Net.agregarVertices("3", "8", 80);

    // Conexiones desde 4
    OSPF_Net.agregarVertices("4", "8", 75);

    // Conexiones desde 5
    OSPF_Net.agregarVertices("5", "6", 91);
    OSPF_Net.agregarVertices("5", "9", 24);

    // Conexiones desde 6
    OSPF_Net.agregarVertices("6", "10", 50);

    // Conexiones desde 7
    OSPF_Net.agregarVertices("7", "10", 10);
    OSPF_Net.agregarVertices("7", "B", 40);

    // Conexiones desde 8
    OSPF_Net.agregarVertices("8", "B", 40);

    // Conexiones desde 9
    OSPF_Net.agregarVertices("9", "10", 35);

    // Conexiones desde 10
    OSPF_Net.agregarVertices("10", "B", 40);


    // 3. EJECUCIÓN
    // Primero mostramos que el router conoce la topología
    OSPF_Net.imprimirListaAdyacencia(); 

    // Calculamos la ruta
    // De ULSA (A) a Chapi (B)
    OSPF_Net.calcularRutaMasCorta("A", "B");

    return 0;
}