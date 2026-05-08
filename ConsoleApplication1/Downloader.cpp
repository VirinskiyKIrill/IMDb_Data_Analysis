#include <windows.h>
#include <iostream>
#include <urlmon.h>

#pragma comment(lib, "urlmon.lib")

using namespace std;
int main()
{
	setlocale(LC_ALL, "");
	const char* places[] = {
		"C:\\Test\\name.basics.tsv.gz",
		"C:\\Test\\title.akas.tsv.gz",
		"C:\\Test\\title.crew.tsv.gz",
		"C:\\Test\\title.episode.tsv.gz",
		"C:\\Test\\title.principals.tsv.gz",
		"C:\\Test\\title.ratings.tsv.gz"};
	const char* url[] = { 
		"https://datasets.imdbws.com/name.basics.tsv.gz",
		"https://datasets.imdbws.com/title.akas.tsv.gz",
		"https://datasets.imdbws.com/title.crew.tsv.gz",
		"https://datasets.imdbws.com/title.episode.tsv.gz",
		"https://datasets.imdbws.com/title.principals.tsv.gz",
		"https://datasets.imdbws.com/title.ratings.tsv.gz"};

	for (int i = 0; i < 6; i++) {
		HRESULT result = URLDownloadToFileA(NULL, url[i], places[i], 0, NULL);

		if (result == S_OK) {
			cout << "Обновление произведено успешно" << endl;
		}
		else {
			cout << "Ошибка обновления" << endl;
		}
	}
	return 67;
}