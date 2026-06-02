#include <iostream>
#include <string>
#include <vector>
#include "structs.h"
#include "functions.h"

using namespace std;

void printHeader(int total, int page, int totalPages);
void printMovieList(const vector<MovieShort>& movies, int startIdx);
void printMovieDetails(const MovieDetails& m);
void printActorDetails(const ActorDetails& a);
bool updateData();

int main()
{
    system("chcp 65001 > nul");

    cout << "+==========================================+" << endl;
    cout << "|                IMDb Search               |" << endl;
    cout << "+==========================================+" << endl;

    const int maxPage = 20;

    while (true) {
        cout << "\nDo you wanna update data?(Y/N): ";
        string input;
        getline(cin, input);

        if (input == "Y" || input == "y") {
            if (!updateData()) {
                cout << "Update failed!" << endl;
            }
            break;
        }
        else if (input.empty()) {
            continue;
        }

        cout << "\nEnter movie title (or 'exit'): ";
        getline(cin, input);

        if (input == "exit" || input == "quit") break;
        if (input.empty()) continue;

        cout << "Enter year filter (0 = no filter): ";
        string yearInput;
        getline(cin, yearInput);

        int yearFilter = 0;
        try {
            yearFilter = stoi(yearInput);
        }
        catch (...) {
            yearFilter = 0;
        }

        cout << "Searching..." << endl;
        auto allResults = searchAllMovies(input, yearFilter);

        if (allResults.empty()) {
            cout << "Nothing found." << endl;
            continue;
        }

        int total = allResults.size();
        int totalPages = (total + maxPage - 1) / maxPage;
        int currentPage = 0;

        bool needRedraw = true;

        while (true) {
            if (needRedraw) {
                auto pageMovies = getPage(allResults, currentPage, maxPage);

                printHeader(total, currentPage, totalPages);
                printMovieList(pageMovies, currentPage * maxPage);

                cout << "\n";
                if (totalPages > 1) {
                    if (currentPage > 0) cout << "[p] <- Prev  ";
                    if (currentPage < totalPages - 1) cout << "[n] -> Next  ";
                }
                cout << "[1-" << pageMovies.size() << "] Select  [q] New search" << endl;
                cout << "Your choice: ";

                needRedraw = false;
            }

            string choice;
            getline(cin, choice);

            if (choice.empty()) {
                needRedraw = true;
                continue;
            }

            if (choice == "p" || choice == "P") {
                if (currentPage > 0) {
                    currentPage--;
                    needRedraw = true;
                }
                continue;
            }
            if (choice == "n" || choice == "N") {
                if (currentPage < totalPages - 1) {
                    currentPage++;
                    needRedraw = true;
                }
                continue;
            }
            if (choice == "q" || choice == "Q") {
                break;
            }

            int filmNum;
            try { filmNum = stoi(choice); }
            catch (...) {
                cout << "Invalid input!" << endl;
                needRedraw = true;
                continue;
            }

            int globalIdx = currentPage * maxPage + filmNum - 1;
            if (filmNum < 1 || globalIdx >= total) {
                cout << "Invalid number!" << endl;
                needRedraw = true;
                continue;
            }

            cout << "\nLoading details..." << endl;
            auto details = getMovieDetails(allResults[globalIdx].tconst);

            while (true) {
                printMovieDetails(details);

                cout << "\n[1-" << details.cast.size() << "] Actor for details" << endl;
                cout << "[b] Back to movie list" << endl;
                cout << "Your choice: ";

                string actorChoice;
                getline(cin, actorChoice);

                if (actorChoice == "b" || actorChoice == "B") break;

                int actorNum;
                try { actorNum = stoi(actorChoice); }
                catch (...) {
                    cout << "Invalid input!" << endl;
                    continue;
                }

                if (actorNum < 1 || actorNum >(int)details.cast.size()) {
                    cout << "Invalid number!" << endl;
                    continue;
                }

                cout << "\nLoading actor info..." << endl;
                auto actorDetails = getActorDetails(details.cast[actorNum - 1].nconst);
                printActorDetails(actorDetails);

                cout << "\n[Enter] - back to movie" << endl;
                cin.get();
            }

            needRedraw = true;
        }
    }

    cout << "\nGoodbye!" << endl;
    return 0;
}

void printHeader(int total, int page, int totalPages)
{
    cout << "\n+------------------------------------------+" << endl;
    cout << "|  Found: " << total << " movies";
    if (totalPages > 1) {
        cout << "  Page " << (page + 1) << "/" << totalPages;
    }
    cout << endl;
    cout << "+------------------------------------------+" << endl;
}

void printMovieList(const vector<MovieShort>& movies, int startIdx)
{
    for (size_t i = 0; i < movies.size(); i++) {
        int num = startIdx + i + 1;
        cout << num << ". " << movies[i].primaryTitle << " (" << movies[i].startYear << ")";
        if (movies[i].averageRating > 0) {
            cout << "  *" << movies[i].averageRating;
        }
        cout << endl;
    }
}

void printMovieDetails(const MovieDetails& m)
{
    cout << "\n+==========================================+" << endl;
    cout << "|  " << m.primaryTitle << endl;
    if (m.originalTitle != m.primaryTitle) {
        cout << "|  (" << m.originalTitle << ")" << endl;
    }
    cout << "+==========================================+" << endl;
    cout << "| Type: " << m.titleType;
    cout << "  |  Year: " << m.startYear;
    if (m.endYear > 0) cout << "-" << m.endYear;
    cout << endl;

    if (m.runtimeMinutes != "\\N") {
        cout << "| Runtime: " << m.runtimeMinutes << " min" << endl;
    }

    if (!m.genres.empty()) {
        cout << "| Genres: ";
        for (auto& g : m.genres) cout << g << " ";
        cout << endl;
    }

    if (m.averageRating > 0) {
        cout << "| Rating: " << m.averageRating
            << "/10  (" << m.numVotes << " votes)" << endl;
    }

    cout << "+------------------------------------------+" << endl;
    cout << "| enter number for details:" << endl;

    if (!m.directors.empty()) {
        cout << "| -- Directors --" << endl;
    }

    int actorNum = 1;
    for (auto& actor : m.cast) {
        cout << "| [" << actorNum << "] " << actor.displayName << endl;
        actorNum++;
    }

    cout << "+==========================================+" << endl;
}

void printActorDetails(const ActorDetails& a)
{
    cout << "\n+==========================================+" << endl;
    cout << "|  " << a.primaryName << endl;
    cout << "+==========================================+" << endl;

    cout << "| ID: " << a.nconst << endl;

    cout << "| Years: " << a.birthYear;
    if (a.deathYear > 0) cout << " - " << a.deathYear;
    else cout << " - present";
    cout << endl;

    if (!a.professions.empty()) {
        cout << "| Professions: ";
        for (auto& p : a.professions) cout << p << " ";
        cout << endl;
    }

    if (!a.knownForNames.empty()) {
        cout << "| Known for:" << endl;
        for (size_t i = 0; i < a.knownForNames.size() && i < 10; i++) {
            cout << "|   * " << a.knownForNames[i] << endl;
        }
    }

    cout << "+==========================================+" << endl;
}

bool updateData()
{
    downloadFiles();

    cout << "\n=== Unpacking files ===" << endl;

    for (const auto& file : imdbFiles) {
        if (unpackGz(file.gzPath.c_str(), file.tsvPath.c_str())) {
            cout << "Unpacked: " << file.gzPath << "\n";
        }
        else {
            cout << "Failed: " << file.gzPath << "\n";
            return false;
        }
    }
    prepareAllSortedFiles();
    return true;
}