#include <vector>
#include <string>
#include <map>

class Chunk {
public:
	Chunk(std::vector<std::wstring> iWords): words(iWords) {}
	int getLength();
	int getWordNum();
	double getLengthVariance();
	double getFrequencyScore(std::map<std::wstring, std::pair<int, std::wstring> > &wordMap);
	std::wstring getFirstWord();
private:
	std::vector<std::wstring> words;
};
