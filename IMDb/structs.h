#pragma once
#include <string>
#include <vector>
using namespace std;

struct ImdbFile {
    string gzPath;
    string tsvPath;
    string sortedPath;
    string baseName;
    string url;
};

struct SortableLine {
    string line;
    string key;

    bool operator<(const SortableLine& other) const {
        return key < other.key;
    }
};

struct RawPrincipal {
    string nconst;
    string category;
    string job;
    string characters;
};

struct MovieShort {
    string tconst;
    string primaryTitle;
    int startYear;
    float averageRating = -1;
};

struct ActorShort {
    string nconst;
    string displayName;
};

struct ActorDetails {
    string nconst;
    string primaryName;
    int birthYear = -1;
    int deathYear = -1;
    vector<string> professions;
    vector<string> knownForTitles;
    vector<string> knownForNames;
};

struct MovieDetails {
    string tconst;
    string primaryTitle;
    string originalTitle;
    string titleType;
    int startYear = -1;
    int endYear = -1;
    string runtimeMinutes;
    vector<string> genres;
    float averageRating = -1;
    int numVotes = -1;
    vector<ActorShort> cast;
    vector<string> directors;
};