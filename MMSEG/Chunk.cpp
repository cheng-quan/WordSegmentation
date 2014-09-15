#include <cmath>
#include "Chunk.h"

using namespace std;

int Chunk::getLength()
{
	int length = 0;
	for (int i = 0; i < words.size(); i++)
		length += words[i].size();

	return length;
}

int Chunk::getWordNum()
{
	return words.size();
}

double Chunk::getLengthVariance()
{
	double average = 0.0;
	double variance = 0.0;
	average = static_cast<double>(getLength()) / words.size();	
	for (int i = 0; i < words.size(); i++)
		variance += (words[i].size() - average) * (words[i].size() - average);

	return variance;
}

double Chunk::getFrequencyScore(std::map<std::wstring, pair<int, std::wstring> > &wordMap)
{
	double score = 0.0;
	for (int i = 0; i < words.size(); i++)
		score += log(wordMap[words[i]].first) / log(2);

	return score;
}

wstring Chunk::getFirstWord()
{
	return words[0];
}
