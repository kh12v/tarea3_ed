#include <iostream>
#include <unordered_map>
#include <fstream>
#include <string>
#include <vector>
#include "OpenHashing.h"

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

int main() {
    std::vector<Tweet> tweets = read_tweets("auspol2019.csv");

    std::cout << "Read " << tweets.size() << " tweets from auspol2019.csv." << std::endl;
    
    // Testing OpenHashing
    std::cout << "\nInserting tweets into OpenHashing table..." << std::endl;
    OpenHashing<Tweet> hashTable(200003);
    
    for (const auto& tweet : tweets) {
        hashTable.insert(tweet.id, tweet);
    }
    
    std::cout << "Successfully inserted elements. Hash table size: " << hashTable.size() << std::endl;
    
    if (!tweets.empty()) {
        std::string testId = tweets[0].id;
        std::cout << "\nSearching for tweet with ID: " << testId << std::endl;
        
        Tweet foundTweet;
        if (hashTable.search(testId, foundTweet)) {
            std::cout << "Tweet found Text: " << foundTweet.full_text << std::endl;
        } else {
            std::cout << "Tweet not found" << std::endl;
        }
    }


    //testing con unordered map

    //formato id : contador
    //implementacion con user_id
    std::cout << "\nInsertando tweets en unordered_map..." << std::endl;
    std::unordered_map<std::string, int> tweets_unordered_user_id;

    for(const auto& tweet : tweets){
        if (tweets_unordered_user_id.count(tweet.user_id)) {
            tweets_unordered_user_id[tweet.user_id] += 1;
        }else{
            tweets_unordered_user_id[tweet.user_id] = 1;
        }
    }

    //implementacion cun user_screen_name
    std::cout << "\nInsertando tweets en unordered_map..." << std::endl;
    std::unordered_map<std::string, int> tweets_unordered_user_screen_name;

    for(const auto& tweet : tweets){
        if (tweets_unordered_user_screen_name.count(tweet.user_screen_name)) {
            tweets_unordered_user_screen_name[tweet.user_screen_name] += 1;
        }else{
            tweets_unordered_user_screen_name[tweet.user_screen_name] = 1;
        }
    }
    return 0;
}
