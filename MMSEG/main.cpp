#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <locale>
#include "Chunk.h"

using namespace std;

int main() 
{
	locale loc("zh_CN.UTF-8");
	locale::global(loc);

	map<wstring, pair<int, wstring> > wordMap;
	wifstream inFile("WordLibrary.txt");
	if (!inFile.is_open()) {
		cerr << "Can't open word library" << endl;
		return -1;
	}
	wstring line;
	while (getline(inFile, line) != NULL) {
		if (line.size() == 0)
			continue;
		size_t index = line.find(L"\t");	
		wstring word = line.substr(0, index);
		line = line.substr(index + 1);
		index = line.find(L"  ");
		int freq = stoi(line.substr(0, index));
		wstring pos = line.substr(index + 2);
		wordMap[word] = make_pair(freq, pos);
	}

	wstring test = L"南京市长江大桥欢迎您";
	while (test.size() > 0) {
		vector<Chunk> segs;
		for (int i = 1; i <= test.size(); i++) {
			if (wordMap.find(test.substr(0, i)) == wordMap.end())
				continue;
			if (i == test.size()) {
				vector<wstring> words;
				words.push_back(test.substr(0));
				segs.push_back(Chunk(words));
				break;
			}
			for (int j = i + 1; j <= test.size(); j++) {
				if (wordMap.find(test.substr(i, j - i)) == wordMap.end())
					continue;
				if (j == test.size()) {
					vector<wstring> words;
					words.push_back(test.substr(0, i));
					words.push_back(test.substr(i));
					segs.push_back(Chunk(words));
					break;
				}
				for (int k = j + 1; k <= test.size(); k++) { 
					if (wordMap.find(test.substr(j, k - j)) == wordMap.end())
						continue;
					vector<wstring> words;
					words.push_back(test.substr(0, i));
					words.push_back(test.substr(i, j - i));
					words.push_back(test.substr(j, k - j));
					segs.push_back(Chunk(words));
				}
			}
		}
		if (segs.size() == 1) {
                        wstring curWord = segs[0].getFirstWord();
                        wstring curPos = wordMap[curWord].second;
                        wcout << curWord << " " << curPos << endl;     
			test = test.substr(curWord.size());
			continue;
                }
		// filter with segmentation length
		int maxLength = 0;
		for (int i = 0; i < segs.size(); i++) {
			if (segs[i].getLength() > maxLength)
				maxLength = segs[i].getLength();
		}
		vector<Chunk>::iterator it = segs.begin();
		while (it != segs.end()) {
			if (it->getLength() < maxLength) 
				segs.erase(it);
			else
				++it;
		}
		if (segs.size() == 1) {
			wstring curWord = segs[0].getFirstWord();
			wstring curPos = wordMap[curWord].second;
			wcout << curWord << " " << curPos << endl;
			test = test.substr(curWord.size());
			continue;
		}	
		// filter with average word length
		int minNum = 3;
		for (int i = 0; i < segs.size(); i++) {
			if (segs[i].getWordNum() < minNum)
				minNum = segs[i].getWordNum();
		}
		it = segs.begin();
		while (it != segs.end()) {
			if (it->getWordNum() > minNum)
				segs.erase(it);
			else
				++it;
		}
		if (segs.size() == 1) {
                        wstring curWord = segs[0].getFirstWord();
                        wstring curPos = wordMap[curWord].second;
                        wcout << curWord << " " << curPos << endl;
                        test = test.substr(curWord.size());
                        continue;
                }
		// filter with variance of word length
		double minVariance = 10000.0;
		for (int i = 0; i < segs.size(); i++) {
			if (segs[i].getLengthVariance() < minVariance)
				minVariance = segs[i].getLengthVariance();
		}
		it = segs.begin();
		while (it != segs.end()) {
			if (it->getLengthVariance() > minVariance + 0.000001)
				segs.erase(it);
			else 
				++it;
		}
		if (segs.size() == 1) {
                        wstring curWord = segs[0].getFirstWord();
                        wstring curPos = wordMap[curWord].second;
                        wcout << curWord << " " << curPos << endl;
                        test = test.substr(curWord.size());
                        continue;
                }
		// filter with frequency score
		double maxScore = 0.0;
		for (int i = 0; i < segs.size(); i++) {
			if (segs[i].getFrequencyScore(wordMap) > maxScore)
				maxScore = segs[i].getFrequencyScore(wordMap);
		}
		it = segs.begin();
		while (it != segs.end()) {
			if (it->getFrequencyScore(wordMap) < maxScore)
				segs.erase(it);
			else
				++it;
		}
		wstring curWord = segs[0].getFirstWord();
               	wstring curPos = wordMap[curWord].second;
                wcout << curWord << " " << curPos << endl;
                test = test.substr(curWord.size());
	}
		
	return 0;
}
