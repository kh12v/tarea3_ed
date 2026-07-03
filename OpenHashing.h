#pragma once

#include <string>
#include <vector>
#include <stdexcept>

template <typename ValueType>
class OpenHashing {
private:
    struct HashNode {
        std::string key;
        ValueType value;
        
        HashNode(const std::string& k, const ValueType& v) : key(k), value(v) {}
    };

    // Hashing Abierto
    std::vector<std::vector<HashNode>> table;
    size_t tableSize;
    size_t numElements;

    // Función hash
    size_t hashFunction(const std::string& key, size_t size) const {
        size_t hash = 5381;

        for (char c : key) {
            // hash * 33 + c
            hash = ((hash << 5) + hash) + c; 
        }

        return hash % size;
    }

public:
    OpenHashing(size_t size = 10007) : tableSize(size), numElements(0) {
        table.resize(tableSize);
    }

    // Insertar un elemento
    void insert(const std::string& key, const ValueType& value) {
        size_t index = hashFunction(key, tableSize);
        
        // Verificar si la clave ya existe para actualizar el valor
        for (auto& node : table[index]) {
            if (node.key == key) {
                node.value = value;
                return;
            }
        }
        
        // Si no existe lo agregamos al final del vector
        table[index].emplace_back(key, value);
        numElements++;
    }

    // Buscar un elemento por su clave
    bool search(const std::string& key, ValueType& outValue) const {
        size_t index = hashFunction(key, tableSize);
        
        for (const auto& node : table[index]) {
            if (node.key == key) {
                outValue = node.value;
                return true;
            }
        }
        return false;
    }

    // Verificar si existe un elemento por su clave
    bool contains(const std::string& key) const {
        size_t index = hashFunction(key, tableSize);
        for (const auto& node : table[index]) {
            if (node.key == key) {
                return true;
            }
        }
        return false;
    }

    // Eliminar un elemento
    bool remove(const std::string& key) {
        size_t index = hashFunction(key, tableSize);
        
        auto& cell = table[index];
        for (auto it = cell.begin(); it != cell.end(); ++it) {
            if (it->key == key) {
                cell.erase(it);
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
