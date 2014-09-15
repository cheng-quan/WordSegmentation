#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <locale>

using namespace std;

const string TAG = "BMES";
const map<char, int>::value_type init_value[] =
{
	map<char, int>::value_type('B', 0),
	map<char, int>::value_type('M', 1),
	map<char, int>::value_type('E', 2),
	map<char, int>::value_type('S', 3)
};
const map<char, int> INDEX(init_value, init_value + 4);

bool train(const string &fileName, vector<double> &initProb, vector< vector<double> > &switchProb, vector< map<wchar_t, double> > &transProb); 
void segment(const wstring &sentence, const vector<double> &initProb, const vector< vector<double> > &switchProb, const vector< map<wchar_t, double> > &transProb);

int main()
{
	locale loc("zh_CN.UTF-8");
	locale::global(loc);
	
	vector<double> initProb(4, 0.0);
	vector< vector<double> > switchProb(4, vector<double>(4, 0.0));
	vector< map<wchar_t, double> > transProb(4, map<wchar_t, double>());

	wstring test(L"要科学论证规划的主要目标任务，使其具有严肃性，重大工程、重大项目必须列入规划逐步实施。");
	if (train("corpus-utf8.txt", initProb, switchProb, transProb)) {
		segment(test, initProb, switchProb, transProb);	
	}

	return 0;
}

bool train(const string &fileName, vector<double> &initProb, vector< vector<double> > &switchProb, vector< map<wchar_t, double> > &transProb)
{
	wifstream file(fileName.c_str());
	if (!file.is_open()) {
		cerr << "Can't open the input file!";
		return false;
	}
	
	wstring line;
	while (getline(file, line)) {
		vector<char> tagVec;
		for (int i = 0; i < line.size(); i++) {
			if (line[i] == L' ')
				continue;
			if (i == 0 || line[i - 1] == L' ') {
				if (i == line.size() - 1 || line[i + 1] == L' ')
					tagVec.push_back('S');
				else
					tagVec.push_back('B');
			} else {
				if (i == line.size() - 1 || line[i + 1] == L' ')
					tagVec.push_back('E');
				else
					tagVec.push_back('M');
			}
		}
		// cal init frequency 
		initProb[INDEX.at(tagVec[0])] += 1;
		// cal switch frequency
		for (int i = 0; i < tagVec.size() - 1; i++) {
			switchProb[INDEX.at(tagVec[i])][INDEX.at(tagVec[i + 1])] += 1;
		}
		// cal transform frequency
		wostringstream wss;
		for (int i = 0; i < line.size(); i++) {
			if (line[i] != L' ')
				wss << line[i];
		}
		line = wss.str();
		for (int i = 0; i < tagVec.size(); i++) {
			transProb[INDEX.at(tagVec[i])][line[i]] += 1;
		}
	}
	// cal init probability
	double total = 0.0;
	for (int i = 0; i < initProb.size(); i++)
		total += initProb[i];
	for (int i = 0; i < initProb.size(); i++)
		initProb[i] /= total;
	// cal switch probalility
	for (int i = 0; i < switchProb.size(); i++) {
		total = 0.0;
		for (int j = 0; j < switchProb[i].size(); j++) 
			total += switchProb[i][j];
		for (int j = 0; j < switchProb[i].size(); j++)
			switchProb[i][j] /= total;
	}
	// cal transform frequency
	for (int i = 0; i < transProb.size(); i++) {
		total = 0.0;
		for (map<wchar_t, double>::iterator it = transProb[i].begin(); it != transProb[i].end(); ++it)
			total += it->second;
		for (map<wchar_t, double>::iterator it = transProb[i].begin(); it != transProb[i].end(); ++it) {
			it->second /= total;
		}
	}

	return true;
}

void segment(const wstring &sentence, const vector<double> &initProb, const vector< vector<double> > &switchProb, const vector< map<wchar_t, double> > &transProb)
{
	vector< vector<double> > pathProb(4, vector<double>(sentence.size(), 0.0));
	vector< vector<char> > preTag(4, vector<char>(sentence.size()));
	for (int i = 0; i < sentence.size(); i++) {
		if (i == 0) {
			for (int j = 0; j < initProb.size(); j++) {
				if (transProb[j].find(sentence[i]) != transProb[j].end())
					pathProb[j][i] = initProb[j] * transProb[j].at(sentence[i]);
				else
					pathProb[j][i] = 0.0;
			}
		} else {
			for (int j = 0; j < pathProb.size(); j++) {
				double maxProb = 0.0;
				char preChar;
				for (int k = 0; k < switchProb.size(); k++) {
					double prob;
					if (transProb[j].find(sentence[i]) != transProb[j].end())
						prob = pathProb[k][i - 1] * switchProb[k][j] * transProb[j].at(sentence[i]);
					else
						prob = 0.0;
					if (prob > maxProb) {
						maxProb = prob;
						preChar = TAG[k];
					}
				}
				pathProb[j][i] = maxProb;
				preTag[j][i] = preChar;
			}
		}
	}
	vector<char> resultTag(sentence.size());
	double maxProb = 0.0;
	char curChar;
	for (int i = 0; i < pathProb.size(); i++) {
		if (pathProb[i][sentence.size() - 1] > maxProb) {
			maxProb = pathProb[i][sentence.size() -1];
			curChar = TAG[i];
		}
	}
	resultTag[sentence.size() - 1] = curChar;
	for (int i = sentence.size() - 1; i > 0; i--) {
		curChar = preTag[INDEX.at(curChar)][i];
		resultTag[i - 1] = curChar;
	}
	for (int i = 0; i < resultTag.size(); i++) {
		if (resultTag[i] == 'B' || resultTag[i] == 'M')
			wcout << sentence[i];
		else
			wcout << sentence[i] << L" ";
	}
	wcout << endl;
}	
