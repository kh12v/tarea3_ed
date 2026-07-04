#include <iostream>
#include <unordered_map>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include "OpenHashing.h"
#include "CloseHashing.h"

struct Tweet {
    std::string created_at;
    std::string id;
    std::string full_text;
    std::string retweet_count;
    std::string favorite_count;
    std::string user_id;
    std::string user_name;
    std::string user_screen_name;
    std::string user_description;
    std::string user_location;
    std::string user_created_at;
};

std::vector<Tweet> read_tweets(const std::string& filename) {
    std::vector<Tweet> tweets;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return tweets;
    }

    std::string line;
    // Skip header
    std::getline(file, line);

    std::string current_field;
    std::vector<std::string> current_row;
    bool in_quotes = false;
    char c;
    
    while (file.get(c)) {
        if (in_quotes) {
            if (c == '"') {
                char next;
                if (file.get(next)) {
                    if (next == '"') {
                        current_field += '"';
                    } else {
                        in_quotes = false;
                        file.unget();
                    }
                } else {
                    in_quotes = false;
                }
            } else {
                current_field += c;
            }
        } else {
            if (c == '"') {
                in_quotes = true;
            } else if (c == ',') {
                current_row.push_back(current_field);
                current_field.clear();
            } else if (c == '\n') {
                current_row.push_back(current_field);
                current_field.clear();
                
                // Add to tweets
                if (current_row.size() >= 11) {
                    Tweet t;
                    t.created_at = current_row[0];
                    t.id = current_row[1];
                    t.full_text = current_row[2];
                    t.retweet_count = current_row[3];
                    t.favorite_count = current_row[4];
                    t.user_id = current_row[5];
                    t.user_name = current_row[6];
                    t.user_screen_name = current_row[7];
                    t.user_description = current_row[8];
                    t.user_location = current_row[9];
                    t.user_created_at = current_row[10];
                    tweets.push_back(t);
                }
                current_row.clear();
            } else if (c == '\r') {
                // ignore \r
            } else {
                current_field += c;
            }
        }
    }
    
    if (!current_row.empty() || !current_field.empty()) {
        current_row.push_back(current_field);
        if (current_row.size() >= 11) {
            Tweet t;
            t.created_at = current_row[0];
            t.id = current_row[1];
            t.full_text = current_row[2];
            t.retweet_count = current_row[3];
            t.favorite_count = current_row[4];
            t.user_id = current_row[5];
            t.user_name = current_row[6];
            t.user_screen_name = current_row[7];
            t.user_description = current_row[8];
            t.user_location = current_row[9];
            t.user_created_at = current_row[10];
            tweets.push_back(t);
        }
    }

    return tweets;
}

// Estructuras dummy para no modificar las originales
struct DummyOpenNode { std::string key; int value; };
struct DummyCloseEntry { std::string key; int value; int state; };

// Estimacion del tamano de std::unordered_map
size_t getUnorderedMapSize(const std::unordered_map<std::string, int>& m) {
    // Estructura base + arreglo de buckets + nodos
    return sizeof(m) + 
           m.bucket_count() * sizeof(void*) + 
           m.size() * (sizeof(std::pair<const std::string, int>) + sizeof(void*));
}

// Estimacion del tamano de OpenHashing
size_t getOpenHashingSize(size_t tableSize, size_t numElements) {
    // Estructura base + overhead del vector de vectores + tamano de los elementos
    return sizeof(OpenHashing<int>) + 
           tableSize * sizeof(std::vector<DummyOpenNode>) + 
           numElements * sizeof(DummyOpenNode);
}

// Estimacion del tamano de CloseHashing
size_t getCloseHashingSize(size_t tableSize) {
    // Estructura base + tamano del vector de Entradas
    return sizeof(CloseHashing<int>) + tableSize * sizeof(DummyCloseEntry);
}

std::string formatSize(size_t bytes) {
    char buf[100];
    if (bytes < 1024) {
        snprintf(buf, sizeof(buf), "%zu B", bytes);
    } else if (bytes < 1024 * 1024) {
        snprintf(buf, sizeof(buf), "%.2f KB", bytes / 1024.0);
    } else {
        snprintf(buf, sizeof(buf), "%.2f MB", bytes / (1024.0 * 1024.0));
    }
    return std::string(buf);
}

void run_experiment(const std::vector<Tweet>& tweets, const std::string& key_type) {
    std::cout << "\n===============================================================================================================\n";
    std::cout << " EXPERIMENTOS CON CLAVE: " << key_type << "\n";
    std::cout << "===============================================================================================================\n";
    std::cout << std::left 
              << std::setw(10) << "Tweets" 
              << std::setw(20) << "UnorderedMap(ms)" << std::setw(15) << "Size(UM)"
              << std::setw(20) << "OpenHashing(ms)" << std::setw(15) << "Size(OH)"
              << std::setw(20) << "CloseHashing(ms)" << std::setw(15) << "Size(CH)"
              << "\n";
    std::cout << std::string(111, '-') << "\n";

    size_t tableSize = 200003;

    for (int limit = 10000; limit <= 180000; limit += 10000) {
        if (limit > static_cast<int>(tweets.size())) {
            std::cout << "No hay suficientes tweets para limit=" << limit << ". Total tweets: " << tweets.size() << "\n";
            break;
        }

        // 1. Unordered Map
        auto start_um = std::chrono::high_resolution_clock::now();
        std::unordered_map<std::string, int> um;
        for (int i = 0; i < limit; i++) {
            const std::string& key = (key_type == "user_id") ? tweets[i].user_id : tweets[i].user_screen_name;
            um[key]++;
        }
        auto end_um = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> diff_um = end_um - start_um;
        size_t size_um = getUnorderedMapSize(um);

        // 2. Open Hashing
        auto start_oh = std::chrono::high_resolution_clock::now();
        OpenHashing<int> oh(tableSize);
        for (int i = 0; i < limit; i++) {
            const std::string& key = (key_type == "user_id") ? tweets[i].user_id : tweets[i].user_screen_name;
            int count = 0;
            if (oh.search(key, count)) {
                oh.insert(key, count + 1);
            } else {
                oh.insert(key, 1);
            }
        }
        auto end_oh = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> diff_oh = end_oh - start_oh;
        size_t size_oh = getOpenHashingSize(tableSize, oh.size());

        // 3. Close Hashing
        auto start_ch = std::chrono::high_resolution_clock::now();
        CloseHashing<int> ch(tableSize, LINEAR);
        for (int i = 0; i < limit; i++) {
            const std::string& key = (key_type == "user_id") ? tweets[i].user_id : tweets[i].user_screen_name;
            int count = 0;
            if (ch.search(key, count)) {
                ch.insert(key, count + 1);
            } else {
                ch.insert(key, 1);
            }
        }
        auto end_ch = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> diff_ch = end_ch - start_ch;
        size_t size_ch = getCloseHashingSize(tableSize);

        // Imprimir resultados
        std::cout << std::left 
                  << std::setw(10) << limit
                  << std::setw(20) << diff_um.count() << std::setw(15) << formatSize(size_um)
                  << std::setw(20) << diff_oh.count() << std::setw(15) << formatSize(size_oh)
                  << std::setw(20) << diff_ch.count() << std::setw(15) << formatSize(size_ch)
                  << "\n";
    }
}

int main() {
    std::cout << "Cargando tweets desde auspol2019.csv...\n";
    std::vector<Tweet> tweets = read_tweets("auspol2019.csv");
    std::cout << "Se cargaron " << tweets.size() << " tweets.\n";

    if (tweets.empty()) {
        std::cerr << "No se pudieron cargar los tweets, verifique el archivo.\n";
        return 1;
    }

    run_experiment(tweets, "user_id");
    run_experiment(tweets, "user_screen_name");

    return 0;
}
