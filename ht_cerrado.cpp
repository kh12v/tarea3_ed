#include <iostream>
#include <vector>
#include <string>

using namespace std;

enum State { EMPTY, OCCUPIED, DELETED };

struct Entry {
    int key;
    string value;
    State state;

    Entry() : key(0), value(""), state(EMPTY) {}
};

float A = (sqrt(5) - 1) / 2;

int h1(int k, int n) {
    return k % n;
}

int h2(int k, int n) {
    float a = (float)k * A;
    a -= (int)a;
    return n * a;
}

// Linear Probing
int linear_probing(int k, int n, int i) {
    return (h1(k, n) + i) % n;
}

// Quadratic Probing
int quadratic_probing(int k, int n, int i) {
    return (h1(k, n) + i + 2 * i * i) % n;
}

// Double Hashing
int double_hashing(int k, int n, int i) {
    return (h1(k, n) + i * h2(k, n)) % n;
}

class HashTable {
private:
    vector<Entry> table;
    int size;

public:
    HashTable(int tableSize) : size(tableSize) {
        table.resize(size);
    }

    // Usa la técnica que desees aquí (solo cambia el nombre de la función de sondeo)
    int probe(int key, int i) {
        return linear_probing(key, size, i);
        // return quadratic_probing(key, size, i);
        // return double_hashing(key, size, i);
    }

    void insert(int key, string value) {
        for (int i = 0; i < size; i++) {
            int index = probe(key, i);

            if (table[index].state == EMPTY || table[index].state == DELETED) {
                table[index].key = key;
                table[index].value = value;
                table[index].state = OCCUPIED;
                return;
            }

            // Si ya existe, actualizar
            if (table[index].state == OCCUPIED && table[index].key == key) {
                table[index].value = value;
                return;
            }
        }

        cout << "Tabla llena, no se pudo insertar " << key << endl;
    }

    string get(int key) {
        for (int i = 0; i < size; i++) {
            int index = probe(key, i);
            if (table[index].state == EMPTY) break; // ya no va a aparecer

            if (table[index].state == OCCUPIED && table[index].key == key) {
                return table[index].value;
            }
        }
        return "No encontrado";
    }

    void remove(int key) {
        for (int i = 0; i < size; i++) {
            int index = probe(key, i);

            if (table[index].state == EMPTY) break;

            if (table[index].state == OCCUPIED && table[index].key == key) {
                table[index].state = DELETED;
                return;
            }
        }
    }
};

int main() {
    HashTable ht(7);

    ht.insert(10, "A");
    ht.insert(20, "B");
    ht.insert(15, "C");
    ht.insert(7, "D");

    ht.print();

    cout << "Buscar 15: " << ht.get(15) << endl;

    ht.remove(15);
    cout << "Después de eliminar 15:\n";
    ht.print();

    return 0;
}
