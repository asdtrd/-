#include <fcntl.h>
#include<io.h>
#include<iostream>
#include<cmath>
#include<vector>
#include<string>
#include<Windows.h>
#include<fstream>
#define MAXPATH 50
using namespace std;

int searchVal = 4;
vector <wstring> wv;
vector<int> searchRange;
vector<int>* specialRule = NULL;
wchar_t* cache;
int first = -1;


void DFS(long long v = 0, long long m = 0, int rule = -1, int always = -1) { //int써도됨
	double maxVal = 10;
	long long i = 0;
	if(always != -1){
		int k = always;

		int j = searchRange[k];
		i = v + j * j;
		if (i < 10000) i = i % 10000;
		cache[m] = j;
		if (m == searchVal - 1) {
			double k = 210 - (i - int(i / 200) * 200 + 5);
			if (k < maxVal) {
				wv.push_back(cache);
			}
		}
		if (specialRule[k].size() > 0) {
			DFS(i, m + 1, k);
			return;
		}
		DFS(i, m + 1);
		return;
	}
	if (rule >= 0) {
		for (int p = 0; p < specialRule[rule].size(); p++) {
			int k = specialRule[rule][p];
			int j = searchRange[k];
			i = v + j * j;
			if (i < 10000) i = i % 10000;
			cache[m] = j;
			if (m == searchVal - 1) {
				double k = 210 - (i - int(i / 200) * 200 + 5);
				if (k < maxVal) {
					wv.push_back(cache);
				}
				continue;
			}
			if (specialRule[k].size() > 0) {
				DFS(i, m + 1, k);
				continue;
			}
			DFS(i, m + 1);
		}
		return;
	}
	for (int k = 0; k < searchRange.size(); k++) {
		int j = searchRange[k];
		i = v + j * j;
		if(i<10000) i = i % 10000;
		cache[m] = j;
		if (m == searchVal -1) {
			double k = 210 - (i - int(i / 200) * 200 + 5);
			if (k < maxVal) {
				wv.push_back(cache);
			}
			continue;
		}
		if (specialRule[k].size() > 0) {
			DFS(i, m + 1, k);
			continue;
		}
		DFS(i, m + 1);
	}
}

int MakeDefaultSetting() {
	wofstream of("Setting.txt");
	if (of.bad()) return 1;
	wstring s = wstring(L"\
5\
\n//search depth(length of word), bigger value use more time \
\n//검색 깊이(단어 길이), 값이 증가할수록 검색 시간이 기하급수적으로 늘어납니다\
\n ア ウ\
\n//search words\
\n//검색할 단어들\
\nア|ヶ\
\n//search words(Range, Unicode)\
\n//일정 범위 검색(유니코드 기준)\
\n//This program can search all characters without space(\' \') and \'|\' and \'-\' and \'!\'\
\n//공백과 하이픈(-)과 느낌표와 |을 제외한 모든 문자를 검색 가능합니다\
\nキ-ノ\
\nノ‐コ\
\n//Creates a condition where two words must be connected\
\n//words must first be defined If not, condition is ignored ex.(ほ-ひ ほ) ほ is not defined\
\n//DO NOT WRITE CONDITION BEFORE ALL WORDS\
\n//두 단어가 반드시 이어지는 조건을 만듭니다\
\n//검색할 단어에 없을 시엔 무시됩니다\
\n//반드시 단어를 다 쓴 뒤에 조건을 쓰십시오\
\n!キ\
\n//set first word if you want\
\n//첫 문자를 고정합니다(선택)");
	of.write(s.c_str(), s.size());
	wchar_t* path = new wchar_t[MAXPATH];
	GetModuleFileNameW(NULL, path, MAXPATH);
	wcout << L"Default setting file maked in " << path << endl;
	wcout << L"Please check Setting.txt and restart the program";
	of.close();
	return 0;
}

int LoadSetting() {
	wifstream f("Setting.txt", ios::binary);
	if (!f.good()) {
		if (f.bad()) {
			wcout << L"setting file open error";
			return -1;
		}
		return (MakeDefaultSetting()) ? -1 : 1;
	}
	streampos fileSize = f.tellg();
	f.seekg(0, ios::end);
	fileSize = f.tellg() - fileSize;
	f.seekg(0, ios::beg);
	wchar_t* data = new wchar_t[(int)fileSize];
	int i = 0;
	while (!f.eof()){
		data[i++] = f.get();
	}
	fileSize = i - 1;
 	searchVal = data[0] - L'0';
	bool skip = false;
	for (int i = 1; i < fileSize;i++) {
		switch (data[i])
		{
		case L'/':
			if (i + 1 == fileSize || data[i + 1] == L'/') {
				while ((!(i == fileSize)) && data[i++] != L'\n'){}
				i--;
				break;
			}
			break;
		case 13://CR
		case L'\n':
		case L' ':
		case L'|':
		case 0x2010: //L'-' but diffrent with L'-'(0x2d) <-??
		case 0x2d:
			break;
		case 0:
		case 0xffff:
			skip = true;
			break;
		case L'!':
			if (i + 1 != fileSize) {
				for (int j = 0; j < searchRange.size(); j++) {
					if (searchRange[j] == data[i + 1]) {
						first = j;
						i++;
						break;
					}
				}
			}
			break;
		default:
			if (i+1 != fileSize && data[i + 1] == L'|' && i+2 < fileSize) {
				for (int j = data[i]; j < data[i + 2]; j++) {
					searchRange.push_back(j);
				}
				i += 2;
				break;
			}

			if (i + 1 != fileSize && (data[i + 1] == 0x2010 || data[i+1] == 0x2d) && i + 2 < fileSize) {
				if (specialRule == NULL) {
					specialRule = new vector<int>[searchRange.size()];
					for (int k = 0; k < searchRange.size(); k++) {
						specialRule[k] = vector<int>();
						specialRule[k].resize(0);
					}
				}
				int flag = 0;
				int pos = 0;
				int tar = 0;
				for (int j = 0; j < searchRange.size(); j++) {
					if (searchRange[j] == data[i]) {
						pos = j;
						if (++flag == 2) {
							break;
						}
					}
					if (searchRange[j] == data[i + 2]) {
						tar = j;
						if (++flag == 2) break;
					}
				}
				if(flag == 2) specialRule[pos].push_back(tar);
				i += 2;
				break;
			}
			searchRange.push_back(data[i]);
		}
		if (skip) break;
	}
	if (specialRule == NULL) {
		specialRule = new vector<int>[searchRange.size()];
		for (int i = 0; i < searchRange.size(); i++) {
			specialRule[i] = vector<int>();
			specialRule[i].resize(0);
		}
	}
	if ((searchRange.size() < 1)) {
		wchar_t* path = new wchar_t[MAXPATH];
		GetModuleFileNameW(NULL, path, MAXPATH);
		wcout << L"No have any search words in Setting.txt\
\nPlease check the file or delete file and restart program\
\nPath of file is "<<path;
		f.close();
		return 1;
	}
	f.close();
	return 0;
}

int main() {
	vector<int> ve;
	_setmode(_fileno(stdout), _O_U16TEXT);
	std::locale::global(std::locale(".UTF-8"));
	if (LoadSetting()) {
		return 0;
	}
	cache = new wchar_t[searchVal+1];
	cache[searchVal] = 0;
	
	DFS(0, 0, -1, first);
	if (wv.size() == 0) {
		wcout << "N/A";
		return 0;
	}
	for (wstring ws : wv) {
		wcout << ws << endl;
	}
	system("pause");
	return 0;
}