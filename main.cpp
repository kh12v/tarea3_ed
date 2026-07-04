#include <iostream>
#include <unordered_map>
#include <fstream>
#include <string>
#include <vector>
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
    
    // Check if last row is incomplete without newline
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

//clasificacion 1 user_id, clasificacion 2 user_screen_name
void contadorUnorderedMap (int clasificacion, std::unordered_map<std::string, int>& tweets_unordered_map, std::vector<Tweet>& tweets){
    
    if (clasificacion == 1){
        std::cout << "\nInsertando tweets mediante user_id en unordered_map..." << std::endl;

        for(const auto& tweet : tweets){
            tweets_unordered_map[tweet.user_id]++;
        }

    }else if (clasificacion == 2){
        std::cout << "\nInsertando tweets mediante user_screen_name en unordered_map..." << std::endl;

        for(const auto& tweet : tweets){
            tweets_unordered_map[tweet.user_screen_name]++;
        }
    }else{
        std::cout << "\nError: Clasificacion invalida" << std::endl;
        return;
    }
    std::cout << "Insertados correctamente en unordered_map" << std::endl;
}

//clasificacion 1 user_id, clasificacion 2 user_screen_name
void contadorCloseHashing (int clasificacion, CloseHashing<int>& closeHashTable, std::vector<Tweet>& tweets){
    
    if(clasificacion == 1){
        std::cout << "\nInsertando tweets mediante user_id en CloseHashing..." << std::endl;
        
        for (const auto& tweet : tweets) {
            int count = 0;
            if (closeHashTable.search(tweet.user_id, count)) {
                closeHashTable.insert(tweet.user_id, count + 1); // Actualiza el valor automáticamente
            } else {
                closeHashTable.insert(tweet.user_id, 1); // Primer tweet
            }
        }
    }else if(clasificacion == 2){
        std::cout << "\nInsertando tweets mediante user_screen_name en CloseHashing..." << std::endl;
    
        for (const auto& tweet : tweets) {
            int count = 0;
            if (closeHashTable.search(tweet.user_screen_name, count)) {
                closeHashTable.insert(tweet.user_screen_name, count + 1); // Actualiza el valor automáticamente
            } else {
                closeHashTable.insert(tweet.user_screen_name, 1); // Primer tweet
            }
        }
    }
    else{
        std::cout << "\nError: Clasificacion invalida" << std::endl;
        return;
    }
    std::cout << "Insertados correctamente en CloseHashing" << std::endl;
    
}

void contadorOpenHashing (int clasificacion, OpenHashing<int>& openHashTable, std::vector<Tweet>& tweets){
    if(clasificacion == 1){
        std::cout << "\nInsertando tweets mediante user_id en OpenHashing..." << std::endl;
        
        for (const auto& tweet : tweets) {
            int count = 0;
            if (openHashTable.search(tweet.user_id, count)) {
                openHashTable.insert(tweet.user_id, count + 1); // Actualiza el valor automáticamente
            } else {
                openHashTable.insert(tweet.user_id, 1); // Primer tweet
            }
        }
    }else if(clasificacion == 2){
        std::cout << "\nInsertando tweets mediante user_screen_name en OpenHashing..." << std::endl;
    
        for (const auto& tweet : tweets) {
            int count = 0;
            if (openHashTable.search(tweet.user_screen_name, count)) {
                openHashTable.insert(tweet.user_screen_name, count + 1); // Actualiza el valor automáticamente
            } else {
                openHashTable.insert(tweet.user_screen_name, 1); // Primer tweet
            }
        }
    }
    else{
        std::cout << "\nError: Clasificacion invalida" << std::endl;
        return;
    }
    std::cout << "Insertados correctamente en OpenHashing" << std::endl;
}


int main() {
    std::vector<Tweet> tweets = read_tweets("auspol2019.csv");

    std::cout << "Read " << tweets.size() << " tweets from auspol2019.csv." << std::endl;

    //testing con unordered map
    //implementacion con user_id
    std::unordered_map<std::string, int> tweets_unordered_user_id;
    contadorUnorderedMap(1, tweets_unordered_user_id, tweets);

    //implementacion cun user_screen_name
    std::unordered_map<std::string, int> tweets_unordered_user_screen_name;
    contadorUnorderedMap(2, tweets_unordered_user_screen_name, tweets);


    // funcionamiento en CloseHashing
    // user_id
    CloseHashing<int> closeHashTable(200003, LINEAR);
    contadorCloseHashing(1, closeHashTable, tweets);

    // user_screen_name
    CloseHashing<int> closeHashTable2(200003, LINEAR);
    contadorCloseHashing(2, closeHashTable2, tweets);

    // test openhshing
    // user_id
    OpenHashing<int> openHashTable(200003);
    contadorOpenHashing(1, openHashTable, tweets);

    // user_screen_name
    OpenHashing<int> openHashTable2(200003);
    contadorOpenHashing(2, openHashTable2, tweets);

    return 0;
}
