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

    // Función hash primaria (djb2)
    size_t h1(const std::string& key, size_t size) const {
        size_t hash = 5381;
        for (char c : key) {
            hash = ((hash << 5) + hash) + c;
        }
        return hash % size;
    }

    // Algoritmo SDBM para generar un valor numérico a partir del string
    size_t stringToNumericHash(const std::string& key) const {
        size_t hash = 0;
        for (char c : key) {
            hash = c + (hash << 6) + (hash << 16) - hash;
        }
        return hash;
    }

    // Función hash secundaria (Método de la multiplicación de Knuth)
    size_t h2(const std::string& key, size_t size) const {
        size_t k = stringToNumericHash(key);
        double A = (std::sqrt(5.0) - 1.0) / 2.0; // Fracción áurea
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

    // Sondeo cuadrático: h(k, i) = (h1(k) + i + 2*i^2) % n
    size_t quadratic_probing(const std::string& key, size_t size, size_t i) const {
        return (h1(key, size) + i + 2 * i * i) % size;
    }

    // Doble hashing: h(k, i) = (h1(k) + i * h2(k)) % n
    size_t double_hashing(const std::string& key, size_t size, size_t i) const {
        return (h1(key, size) + i * h2(key, size)) % size;
    }

    // Función general de sondeo (probar)
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
        for (size_t i = 0; i < tableSize; i++) {
            size_t index = probe(key, i);

            // Si está vacío o marcado como borrado, insertamos aquí
            if (table[index].state == EMPTY || table[index].state == DELETED) {
                table[index].key = key;
                table[index].value = value;
                table[index].state = OCCUPIED;
                numElements++;
                return;
            }

            // Si la clave ya existe, actualizamos el valor y retornamos
            if (table[index].state == OCCUPIED && table[index].key == key) {
                table[index].value = value;
                return;
            }
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
