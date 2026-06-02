#pragma once
#include <string>
#include <vector>
#include "structs.h"
#include <map>

extern const string dataPath;
extern const vector <ImdbFile> imdbFiles;

void downloadFiles();
bool unpackGz(const char* from, const char* to);
bool fileExists(const string& path);
void sortFile(const string& inputName, const string& outputName);
void prepareAllSortedFiles();

vector<string> split(const string& line, char del = '\t');
vector<string> splitCom(const string& str);
int IntSafe(const string& str);
float FloatSafe(const string& str);
bool BoolSafe(const string& str);

string binarySearchInFile(const string& filePath, const string& key);
vector<string> binarySearchAllInFile(const string& filePath, const string& key);

vector<MovieShort> searchAllMovies(const string& input, int yearFilter = 0);
vector<MovieShort> getPage(const vector<MovieShort>& all, int page, int maxPage = 20);
MovieDetails getMovieDetails(const string& tconst);
ActorDetails getActorDetails(const string& nconst);
string getMovieTitle(const string& tconst);