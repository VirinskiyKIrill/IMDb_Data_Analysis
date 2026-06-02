#include "functions.h"
#include <fstream>
#include <windows.h>
#include <zlib.h>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <map>

#pragma comment(lib, "urlmon.lib")
using namespace std;

const string dataPath = "C:\\IMDb_Data_Analysis\\IMDb\\Temp\\";

const vector <ImdbFile> imdbFiles = {
    {
        dataPath + "name.basics.tsv.gz",
        dataPath + "name.basics.tsv",
        dataPath + "name.basics.tsv.sorted.tsv",
        "name.basics",
        "https://datasets.imdbws.com/name.basics.tsv.gz"
    },
    {
        dataPath + "title.principals.tsv.gz",
        dataPath + "title.principals.tsv",
        dataPath + "title.principals.tsv.sorted.tsv",
        "title.principals",
        "https://datasets.imdbws.com/title.principals.tsv.gz"
    },
    {
        dataPath + "title.ratings.tsv.gz",
        dataPath + "title.ratings.tsv",
        dataPath + "title.ratings.tsv.sorted.tsv",
        "title.ratings",
        "https://datasets.imdbws.com/title.ratings.tsv.gz"
    },
    {
        dataPath + "title.basics.tsv.gz",
        dataPath + "title.basics.tsv",
        dataPath + "title.basics.tsv.sorted.tsv",
        "title.basics",
        "https://datasets.imdbws.com/title.basics.tsv.gz"
    }
};


void downloadFiles()
{
    for (const auto& file : imdbFiles) {
        cout << "\nDownloading " << file.baseName << "..." << endl;

        HRESULT result = URLDownloadToFileA(NULL, file.url.c_str(), file.gzPath.c_str(), 0, NULL);

        if (result == S_OK) {
            cout << "OK: " << file.gzPath << "\n";
        }
        else {
            cout << "ERROR: " << file.gzPath << " code: " << result << "\n";
        }
    }
}

bool unpackGz(const char* from, const char* to) {
    gzFile file = gzopen(from, "rb");
    if (!file) {
        std::cout << "Cannot open: " << from << "\n";
        return false;
    }

    std::ofstream out(to, std::ios::binary);
    if (!out) {
        std::cout << "Cannot create: " << to << "\n";
        gzclose(file);
        return false;
    }

    char buffer[1024];
    int bytes;

    while ((bytes = gzread(file, buffer, sizeof(buffer))) > 0) {
        out.write(buffer, bytes);
    }

    gzclose(file);
    out.close();
    return true;
}

bool fileExists(const string& path)
{
    DWORD attribs = GetFileAttributesA(path.c_str());
    return (attribs != INVALID_FILE_ATTRIBUTES &&
        !(attribs & FILE_ATTRIBUTE_DIRECTORY));
}

void sortFile(const string& inputPath, const string& baseName)
{
    string outputPath = inputPath + ".sorted.tsv";

    if (fileExists(outputPath)) {
        cout << "[OK] " << baseName << ".sorted.tsv already exists." << endl;
        return;
    }

    if (!fileExists(inputPath)) {
        cerr << "ERROR: Source file not found: " << inputPath << endl;
        return;
    }

    cout << "\nSorting " << baseName << ".tsv..." << endl;

    ifstream infile(inputPath);
    if (!infile) {
        cerr << "ERROR: Cannot open " << inputPath << endl;
        return;
    }

    string header;
    getline(infile, header);

    vector <SortableLine > lines;
    string line;
    int count = 0;

    while (getline(infile, line)) {
        auto cols = split(line);
        string key = cols.empty() ? line : cols[0];

        lines.push_back({ line, key });
        count++;

        if (count % 200000 == 0) {
            cout << "  Read " << count << " lines..." << '\r';
        }
    }

    cout << "\n  Total: " << count << " lines. Sorting..." << endl;

    sort(lines.begin(), lines.end());

    cout << "  Writing..." << endl;
    ofstream outfile(outputPath);
    outfile << header << "\n";
    for (auto& item : lines) {
        outfile << item.line << "\n";
    }

    cout << "  Done! Created " << baseName << ".sorted.tsv" << endl;
}

void prepareAllSortedFiles()
{
    cout << "Checking sorted files" << endl;

    for (const auto& file : imdbFiles) {
        sortFile(file.tsvPath, file.baseName);
    }

    cout << "\nAll sorted files ready" << endl;
}

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

string binarySearchInTsv(const string& filePath, const string& key) {
    ifstream file(filePath, ios::binary);
    if (!file.is_open()) {
        cerr << "[ERROR] Cannot open file: " << filePath << endl;
        return "";
    }

    string header;
    getline(file, header);
    streamoff startPos = (streamoff)file.tellg();

    file.seekg(0, ios::end);
    streamoff endPos = (streamoff)file.tellg();

    streamoff left = startPos;
    streamoff right = endPos;
    string line;

    while (left < right) {
        streamoff mid = left + (right - left) / 2;
        file.seekg(mid);

        getline(file, line);

        streamoff lineStart = (streamoff)file.tellg();

        if (!getline(file, line)) {
            right = mid;
            continue;
        }

        auto cols = split(line);
        if (cols.empty()) {
            right = mid;
            continue;
        }

        if (cols[0] == key) {
            return line;
        }
        else if (cols[0] < key) {
            left = lineStart;
        }
        else {
            right = mid;
        }
    }

    return "";
}

vector<MovieShort> searchAllMovies(const string& input, int yearFilter)
{
    vector<MovieShort> results;
    ifstream file(dataPath + "title.basics.tsv.sorted.tsv");
    if (!file) {
        cout << "Error: " << dataPath + "title.basics.tsv not found" << endl;
        return results;
    }

    string line;
    getline(file, line);

    while (getline(file, line)) {
        auto cols = split(line);
        if (cols.size() < 9) continue;

        if (cols[2].find(input) == string::npos) continue;

        int movieYear = IntSafe(cols[5]);
        if (yearFilter != 0 && movieYear != yearFilter) continue;

        MovieShort m;
        m.tconst = cols[0];
        m.primaryTitle = cols[2];
        m.startYear = movieYear;
        results.push_back(m);
    }
    return results;
}

string getMovieTitle(const string& tconst)
{
    string line = binarySearchInTsv(dataPath + "title.basics.tsv.sorted.tsv", tconst);
    if (line.empty()) return "Unknown";
    auto cols = split(line);
    if (cols.size() < 3) return "Unknown";
    return cols[2];
}

MovieDetails getMovieDetails(const string& tconst)
{
    MovieDetails result;
    result.tconst = tconst;

    string line = binarySearchInTsv(dataPath + "title.basics.tsv.sorted.tsv", tconst);
    if (!line.empty()) {
        auto cols = split(line);
        if (cols.size() >= 9) {
            result.titleType = cols[1];
            result.primaryTitle = cols[2];
            result.originalTitle = cols[3];
            result.startYear = IntSafe(cols[5]);
            result.endYear = IntSafe(cols[6]);
            result.runtimeMinutes = cols[7];
            result.genres = splitCom(cols[8]);
        }
    }

    line = binarySearchInTsv(dataPath + "title.ratings.tsv.sorted.tsv", tconst);
    if (!line.empty()) {
        auto cols = split(line);
        if (cols.size() >= 3) {
            result.averageRating = FloatSafe(cols[1]);
            result.numVotes = IntSafe(cols[2]);
        }
    }

    vector <RawPrincipal > principalsList;
    {
        ifstream pfile(dataPath + "title.principals.tsv.sorted.tsv", ios::binary);
        if (pfile) {
            string header;
            getline(pfile, header);
            streamoff startPos = (streamoff)pfile.tellg();

            pfile.seekg(0, ios::end);
            streamoff endPos = (streamoff)pfile.tellg();

            streamoff left = startPos;
            streamoff right = endPos;
            string line;
            streamoff foundPos = -1;

            while (left < right) {
                streamoff mid = left + (right - left) / 2;
                pfile.seekg(mid);
                getline(pfile, line);
                streamoff lineStart = (streamoff)pfile.tellg();

                if (!getline(pfile, line)) {
                    right = mid;
                    continue;
                }

                auto cols = split(line);
                if (cols.empty()) {
                    left = lineStart;
                    continue;
                }

                if (cols[0] == tconst) {
                    foundPos = lineStart;
                    break;
                }
                else if (cols[0] < tconst) {
                    left = lineStart;
                }
                else {
                    right = mid;
                }
            }

            if (foundPos != -1) {
                pfile.seekg(foundPos);
                while (getline(pfile, line)) {
                    auto cols = split(line);
                    if (cols.empty()) continue;
                    if (cols[0] == tconst) {
                        RawPrincipal p;
                        p.nconst = cols[2];
                        p.category = cols[3];
                        p.job = cols[4];
                        p.characters = cols[5];
                        principalsList.push_back(p);
                    }
                    else {
                        break;
                    }
                }
            }
        }
    }

    if (!principalsList.empty()) {
        for (auto& p : principalsList) {
            string nline = binarySearchInTsv(dataPath + "name.basics.tsv.sorted.tsv", p.nconst);
            if (!nline.empty()) {
                auto cols = split(nline);
                if (cols.size() >= 2) {
                    string name = cols[1];
                    string display = name + " - " + p.category;

                    if (!p.job.empty() && p.job != "\\N" && p.job != p.category) {
                        display += " (" + p.job + ")";
                    }
                    if (!p.characters.empty() && p.characters != "\\N") {
                        display += " [" + p.characters + "]";
                    }

                    ActorShort as;
                    as.nconst = p.nconst;
                    as.displayName = display;
                    result.cast.push_back(as);

                    if (p.category == "director") {
                        result.directors.push_back(display);
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

    string line = binarySearchInTsv(dataPath + "name.basics.tsv.sorted.tsv", nconst);
    if (line.empty()) return result;

    auto cols = split(line);
    if (cols.size() < 6) return result;

    result.primaryName = cols[1];
    result.birthYear = IntSafe(cols[2]);
    result.deathYear = IntSafe(cols[3]);
    result.professions = splitCom(cols[4]);
    result.knownForTitles = splitCom(cols[5]);

    for (auto& tconst : result.knownForTitles) {
        result.knownForNames.push_back(getMovieTitle(tconst));
    }

    return result;
}