#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <cmath>

enum ProbingType { LINEAR, QUADRATIC, DOUBLE_HASHING };

template <typename ValueType>
class CloseHashing {
private:
    enum State { EMPTY, OCCUPIED, DELETED };

    struct Entry {
        std::string key;
        ValueType value;
        State state;

        Entry() : key(""), value(ValueType()), state(EMPTY) {}
    };

    std::vector<Entry> table;
    size_t tableSize;
    size_t numElements;
    ProbingType probingType;

    // Funcion hash primaria (djb2)
    size_t h1(const std::string& key, size_t size) const {
        size_t hash = 5381;
        for (char c : key) {
            hash = ((hash << 5) + hash) + c;
        }
        return hash % size;
    }

    // Algoritmo SDBM para generar un valor numerico a partir del string
    size_t stringToNumericHash(const std::string& key) const {
        size_t hash = 0;
        for (char c : key) {
            hash = c + (hash << 6) + (hash << 16) - hash;
        }
        return hash;
    }

    // Funcion hash secundaria (Metodo de la multiplicacion de Knuth)
    size_t h2(const std::string& key, size_t size) const {
        size_t k = stringToNumericHash(key);
        double A = (std::sqrt(5.0) - 1.0) / 2.0; // Fraccion aurea
        double a = static_cast<double>(k) * A;
        a -= std::floor(a);
        size_t val = static_cast<size_t>(size * a);
        
        // El salto en double hashing no debe ser 0 para evitar bucles infinitos
        if (val == 0) return 1;
        return val;
    }

    // Sondeo lineal: h(k, i) = (h1(k) + i) % n
    size_t linear_probing(const std::string& key, size_t size, size_t i) const {
        return (h1(key, size) + i) % size;
    }

    // Sondeo cuadrativo: h(k, i) = (h1(k) + i + 2*i^2) % n
    size_t quadratic_probing(const std::string& key, size_t size, size_t i) const {
        return (h1(key, size) + i + 2 * i * i) % size;
    }

    // Doble hashing: h(k, i) = (h1(k) + i * h2(k)) % n
    size_t double_hashing(const std::string& key, size_t size, size_t i) const {
        return (h1(key, size) + i * h2(key, size)) % size;
    }

    // Funcion general de sondeo (probar)
    size_t probe(const std::string& key, size_t i) const {
        switch (probingType) {
            case QUADRATIC:
                return quadratic_probing(key, tableSize, i);
            case DOUBLE_HASHING:
                return double_hashing(key, tableSize, i);
            case LINEAR:
            default:
                return linear_probing(key, tableSize, i);
        }
    }

public:
    CloseHashing(size_t size = 10007, ProbingType type = LINEAR) 
        : tableSize(size), numElements(0), probingType(type) {
        table.resize(tableSize);
    }

    // Insertar un elemento
    void insert(const std::string& key, const ValueType& value) {
        int firstDeletedIndex = -1;
        for (size_t i = 0; i < tableSize; i++) {
            size_t index = probe(key, i);

            if (table[index].state == EMPTY) {
                // Si encontramos una celda VACIA, el elemento no existe.
                // Insertamos en el primer slot DELETED encontrado para reusar espacio, o en el actual EMPTY.
                if (firstDeletedIndex != -1) {
                    table[firstDeletedIndex].key = key;
                    table[firstDeletedIndex].value = value;
                    table[firstDeletedIndex].state = OCCUPIED;
                } else {
                    table[index].key = key;
                    table[index].value = value;
                    table[index].state = OCCUPIED;
                }
                numElements++;
                return;
            }

            if (table[index].state == DELETED) {
                // Guardamos el primer slot DELETED para reusarlo si la clave no existe después
                if (firstDeletedIndex == -1) {
                    firstDeletedIndex = static_cast<int>(index);
                }
            }

            // Si la clave ya existe, actualizamos su valor
            if (table[index].state == OCCUPIED && table[index].key == key) {
                table[index].value = value;
                return;
            }
        }

        // Si recorrimos toda la tabla sin encontrar EMPTY, pero hay un slot DELETED, insertamos ahí
        if (firstDeletedIndex != -1) {
            table[firstDeletedIndex].key = key;
            table[firstDeletedIndex].value = value;
            table[firstDeletedIndex].state = OCCUPIED;
            numElements++;
            return;
        }

        throw std::runtime_error("Tabla hash cerrada llena, no se pudo insertar");
    }

    // Buscar un elemento por su clave
    bool search(const std::string& key, ValueType& outValue) const {
        for (size_t i = 0; i < tableSize; i++) {
            size_t index = probe(key, i);
            
            // Si encontramos una celda VACÍA, significa que el elemento no existe en la tabla
            if (table[index].state == EMPTY) {
                break;
            }

            // Si la casilla está ocupada y la clave coincide, la encontramos
            if (table[index].state == OCCUPIED && table[index].key == key) {
                outValue = table[index].value;
                return true;
            }
        }
        return false;
    }

    // Verificar si existe un elemento por su clave
    bool contains(const std::string& key) const {
        for (size_t i = 0; i < tableSize; i++) {
            size_t index = probe(key, i);
            if (table[index].state == EMPTY) {
                break;
            }
            if (table[index].state == OCCUPIED && table[index].key == key) {
                return true;
            }
        }
        return false;
    }

    // Eliminar un elemento
    bool remove(const std::string& key) {
        for (size_t i = 0; i < tableSize; i++) {
            size_t index = probe(key, i);
            
            // Si la casilla está VACÍA, el elemento no existe en la tabla
            if (table[index].state == EMPTY) {
                break;
            }

            // Si coincide, hacemos borrado perezoso (lazy deletion)
            if (table[index].state == OCCUPIED && table[index].key == key) {
                table[index].state = DELETED;
                numElements--;
                return true;
            }
        }
        return false;
    }

    // Obtener la cantidad de elementos en la tabla
    size_t size() const {
        return numElements;
    }
};
