#include <windows.h>
#include <iostream>
#include <urlmon.h>
#include <map>

#pragma comment(lib, "urlmon.lib")

using namespace std;
int main()
{
	setlocale(LC_ALL, "");
	map<string, string> files
	{
		{"C:\\Test\\name.basics.tsv.gz", "https://datasets.imdbws.com/name.basics.tsv.gz"},
		{"C:\\Test\\title.akas.tsv.gz", "https://datasets.imdbws.com/title.akas.tsv.gz"},
		{"C:\\Test\\title.crew.tsv.gz", "https://datasets.imdbws.com/title.crew.tsv.gz"},
		{"C:\\Test\\title.episode.tsv.gz", "https://datasets.imdbws.com/title.episode.tsv.gz"},
		{"C:\\Test\\title.principals.tsv.gz", "https://datasets.imdbws.com/title.principals.tsv.gz"},
		{"C:\\Test\\title.ratings.tsv.gz", "https://datasets.imdbws.com/title.ratings.tsv.gz"}
	};
	for (const auto& file : files)
	{
		const string& place = file.first;
		const string& url = file.second;

		HRESULT result = URLDownloadToFileA(NULL, url.c_str(), place.c_str(), 0, NULL);

		if (result == S_OK) {
			cout << "Обновление произведено успешно" << endl;
		}
		else {
			cout << "Ошибка обновления: " << place << "Код: " << result << endl;
		}
	}
	return 67;
}