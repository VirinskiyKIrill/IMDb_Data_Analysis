#pragma once
#include <string>
#include <vector>
#include "structs.h"

// ѕуть к данным (определЄн в functions.cpp)
extern const string dataPath;

vector<string> split(const string& line, char del = '\t');
vector<string> splitCom(const string& str);
int IntSafe(const string& str);
float FloatSafe(const string& str);
bool BoolSafe(const string& str);

vector<MovieShort> searchAllMovies(const string& input);
vector<MovieShort> getPage(const vector<MovieShort>& all, int page, int maxPage = 20);
MovieDetails getMovieDetails(const string& tconst);
ActorDetails getActorDetails(const string& nconst);
string getMovieTitle(const string& tconst);