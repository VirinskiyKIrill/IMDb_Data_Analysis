#include "functions.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>

using namespace std;

const string dataPath = "C:\\IMDb_Data_Analysis\\IMDb\\Temp\\";

vector<string> split(const string& line, char del)
{
    vector<string> parts;
    string part;
    stringstream ss(line);
    while (getline(ss, part, del)) {
        if (!part.empty() && part.back() == '\r')
            part.pop_back();
        parts.push_back(part);
    }
    return parts;
}
vector<string> splitCom(const string& str)
{
    vector<string> result;
    if (str.empty() || str == "\\N") return result;
    stringstream ss(str);
    string part;
    while (getline(ss, part, ',')) {
        result.push_back(part);
    }
    return result;
}

int IntSafe(const string& str)
{
    if (str.empty() || str == "\\N") return -1;
    try { return stoi(str); }
    catch (...) { return -1; }
}

float FloatSafe(const string& str)
{
    if (str.empty() || str == "\\N") return -1;
    try { return stof(str); }
    catch (...) { return -1.0f; }
}

bool BoolSafe(const string& str)
{
    return str == "1";
}

vector<MovieShort> getPage(const vector<MovieShort>& all, int page, int maxPage)
{
    vector<MovieShort> pageResult;
    int start = page * maxPage;
    if (start >= (int)all.size()) return pageResult;
    int end = min(start + maxPage, (int)all.size());
    for (int i = start; i < end; i++) {
        pageResult.push_back(all[i]);
    }
    return pageResult;
}

vector<MovieShort> searchAllMovies(const string& input)
{
    vector<MovieShort> results;
    ifstream file(dataPath + "title.basics.tsv.txt");
    if (!file) {
        cout << "Error: " << dataPath + "title.basics.tsv.txt not found" << endl;
        return results;
    }
    
    string line;
    getline(file, line);
    
    while (getline(file, line)) {
        auto cols = split(line);
        if (cols.size() < 9) continue;
        if (cols[2].find(input) != string::npos) {
            MovieShort m;
            m.tconst = cols[0];
            m.primaryTitle = cols[2];
            m.startYear = IntSafe(cols[5]);
            results.push_back(m);
        }
    }
    return results;
}

string getMovieTitle(const string& tconst)
{
    ifstream file(dataPath + "title.basics.tsv.txt");
    if (!file) return "Unknown";
    
    string line;
    getline(file, line);
    
    while (getline(file, line)) {
        auto cols = split(line);
        if (cols.size() < 3) continue;
        if (cols[0] == tconst) return cols[2];
    }
    return "Unknown";
}

MovieDetails getMovieDetails(const string& tconst)
{
    MovieDetails result;
    result.tconst = tconst;

    // 1. Основная инфа
    {
        ifstream file(dataPath + "title.basics.tsv.txt");
        if (file) {
            string line;
            getline(file, line);
            while (getline(file, line)) {
                auto cols = split(line);
                if (cols.size() < 9) continue;
                if (cols[0] == tconst) {
                    result.titleType = cols[1];
                    result.primaryTitle = cols[2];
                    result.originalTitle = cols[3];
                    result.startYear = IntSafe(cols[5]);
                    result.endYear = IntSafe(cols[6]);
                    result.runtimeMinutes = cols[7];
                    result.genres = splitCom(cols[8]);
                    break;
                }
            }
        }
    }

    // 2. Рейтинг
    {
        ifstream rfile(dataPath + "title.ratings.tsv.txt");
        if (rfile) {
            string line;
            getline(rfile, line);
            while (getline(rfile, line)) {
                auto cols = split(line);
                if (cols.size() < 3) continue;
                if (cols[0] == tconst) {
                    result.averageRating = FloatSafe(cols[1]);
                    result.numVotes = IntSafe(cols[2]);
                    break;
                }
            }
        }
    }

    // 3. Собираем участников
    
   vector<RawPrincipal> principalsList;
    {
        ifstream pfile(dataPath + "principals.tsv.txt");
        if (pfile) {
            string line;
            getline(pfile, line);
            while (getline(pfile, line)) {
                auto cols = split(line);
                if (cols.size() < 6) continue;
                if (cols[0] == tconst) {
                    RawPrincipal p;
                    p.nconst = cols[2];
                    p.category = cols[3];
                    p.job = cols[4];
                    p.characters = cols[5];
                    principalsList.push_back(p);
                }
            }
        }
    }
    
    // 4. Ищем имена
    if (!principalsList.empty()) {
        ifstream nfile(dataPath + "name.basics.tsv.txt");
        if (nfile) {
            string line;
            getline(nfile, line);
            while (getline(nfile, line) && !principalsList.empty()) {
                auto cols = split(line);
                if (cols.size() < 2) continue;
                
                for (auto it = principalsList.begin(); it != principalsList.end(); ) {
                    if (cols[0] == it->nconst) {
                        string name = cols[1];
                        string display = name + " — " + it->category;

                        if (!it->job.empty() && it->job != "\\N" && it->job != it->category) {
                            display += " (" + it->job + ")";
                        }
                        if (!it->characters.empty() && it->characters != "\\N") {
                            display += " [" + it->characters + "]";
                        }
                        
                        ActorShort as;
                        as.nconst = it->nconst;
                        as.displayName = display;
                        result.cast.push_back(as);
                        
                        if (it->category == "director") {
                            result.directors.push_back(display);
                        }
                        
                        it = principalsList.erase(it);
                    }
                    else {
                        ++it;
                    }
                }
            }
        }
    }

    return result;
}

ActorDetails getActorDetails(const string& nconst)
{
    ActorDetails result;
    result.nconst = nconst;
    
    ifstream file(dataPath + "name.basics.tsv.txt");
    if (!file) return result;
    
    string line;
    getline(file, line);
    
    while (getline(file, line)) {
        auto cols = split(line);
        if (cols.size() < 6) continue;
        
        if (cols[0] == nconst) {
            result.primaryName = cols[1];
            result.birthYear = IntSafe(cols[2]);
            result.deathYear = IntSafe(cols[3]);
            result.professions = splitCom(cols[4]);
            result.knownForTitles = splitCom(cols[5]);
            break;
        }
    }
    
    for (auto& tconst : result.knownForTitles) {
        result.knownForNames.push_back(getMovieTitle(tconst));
    }
    
    return result;
}