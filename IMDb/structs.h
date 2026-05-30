#pragma once
#include <string>
#include <vector>
using namespace std;

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
    string displayName;  // Имя + роль для списка
};

struct ActorDetails {
    string nconst;
    string primaryName;
    int birthYear = -1;
    int deathYear = -1;
    vector<string> professions;
    vector<string> knownForTitles;  // tconst фильмов
    vector<string> knownForNames;   // Названия фильмов (подгружаем отдельно)
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
    vector<ActorShort> cast;  // Все участники с nconst для перехода
    vector<string> directors;
};