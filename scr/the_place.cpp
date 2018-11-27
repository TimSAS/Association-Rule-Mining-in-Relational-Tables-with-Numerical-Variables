#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <map>

using namespace std;

//working with files:
void openFile(string fileName, ifstream & ourFile); //works
void closeFile(ifstream & ourFile); //works

//working with vector
void populateVector(ifstream & ourFile, vector<vector<string>> & ourData); //works
void displayVector(const vector<vector<string>> & ourData); //works

//working with maps:
void populateFrequencyMap(const vector<vector<string>> & ourData, map<string, int> & supportMap);
void displayMap(const map<string, int> & ourMap);
map<string, int> thresholdTheMap(const map<string, int> & ourMap, const float threshold, const int caseCount);
void makePairs(vector<vector<string>> & frqPairs, const map<string, int> & frqMap);
void populateFrequentPairs(const vector<vector<string>> & pairs, map<vector<string>, int> & frqPairs, const vector<vector<string>> & dataSet);
map<string, int> supportTesting(const map<vector<string>, int> & ourMap, const float threshold, const int caseCount);

int main()
{
	//opening our file
	ifstream ourFile;
	string fileName = "country_stats.csv";
	openFile(fileName, ourFile);

	//getting the data into a 2-dimentional vector:
	vector< vector<string> > countryData;
	populateVector(ourFile, countryData);
	displayVector(countryData);

	/*
	//figuring out the support & removing not "popular" items
	map<string, int> frequencyMap;
	populateFrequencyMap(crimeData, frequencyMap);
	//displayMap(&frequencyMap);
	//filter variable(s):
	const float supportThreshold = 0.05f;
	const int caseCount = crimeData.size();
	//removing infrequent items:
	map<string, int> reducedMap = thresholdTheMap(frequencyMap, supportThreshold, caseCount);
	cout << "Frequent items, based on support of 5%: " << endl;
	displayMap(reducedMap);

	//figuring out frequent itemsets
	vector<vector<string>> pairs;
	makePairs(pairs, reducedMap);
	//displayVector(&pairs);
	//counting the occurrences of each pair in all of the cases
	map< vector<string>, int> frequentPairs;
	populateFrequentPairs(pairs, frequentPairs, crimeData);
	//display the frequencies
	//for (const auto &p : frequentPairs){ cout << p.first[0] << " && " << p.first[1] << " show up " << p.second << " times " << endl; }

	//finally run it through support testing
	map<string, int> res = supportTesting(frequentPairs, supportThreshold, caseCount);
	//display the frequencies
	cout << endl << endl << "Most frequent pairs, based on support of 5%:" << endl;
	for (const auto &p : res) { cout << p.first << " show up " << p.second << " times " << endl; }
	*/
	//close file
	closeFile(ourFile);

	std::cin.get();
}

void openFile(string fileName, ifstream & ourFile)
{
	ourFile.open(fileName);
	if (ourFile.is_open())
	{
		cout << fileName << " is successfully opened!" << endl << endl << endl;
	}
	else
	{
		cout << "Unable to open file!" << endl << endl << endl;
	}
}
void closeFile(ifstream & ourFile)
{
	ourFile.close();
}

void populateVector(ifstream & ourFile, vector<vector<string>> & ourData)
{
	//needs some work
	if (ourFile.is_open())
	{
		string row;
		while (getline(ourFile, row))
		{
			vector<string> rowItems;
			string tempStr;
			for (int i = 0; i < row.size(); i++)
			{
				if (row[i] == ',')
				{
					rowItems.push_back(tempStr);
					tempStr = "";
				}
				else if (row[i] == '\"') { continue; }
				else { tempStr += row[i]; }

			}
			rowItems.push_back(tempStr);
			ourData.push_back(rowItems);
		}
	}
}
void displayVector(const vector<vector<string>> & ourData)
{
	for (int y = 0; y < ourData.size(); y++)
	{
		for (int x = 0; x < ourData[y].size(); x++)
		{
			cout << "[" << y << "][" << x << "] " << ourData[y][x] << "    ";
		}
		cout << endl;
	}
}

void populateFrequencyMap(const vector<vector<string>> & ourData, map<string, int> & frqMap)
{
	for (int y = 0; y < ourData.size(); y++)
	{
		for (int x = 0; x < ourData[y].size(); x++)
		{
			//if the item already in our map:
			if (frqMap.count(ourData[y][x]) > 0)
			{
				frqMap[ourData[y][x]]++;
			}
			//the items wasn't added to our map yet:
			else
			{
				frqMap[ourData[y][x]] = 1;
			}
		}
	}
}
void displayMap(const map<string, int> & ourMap)
{
	for (const auto &p : ourMap)
	{
		cout << "ourMap[" << p.first << "] = " << p.second << endl;
	}
}
map<string, int> thresholdTheMap(const map<string, int> & ourMap, const float threshold, const int caseCount)
{
	map<string, int> tempMap;
	for (const auto &p : ourMap)
	{
		//if support for item is higher then threshold - add it to new map
		if (float(p.second) / float(caseCount) > threshold)
		{
			tempMap[p.first] = p.second;
			//cout << (float)p.second / (float)caseCount << endl;
		}
	}
	return tempMap;
}

void makePairs(vector<vector<string>> & frqPairs, const map<string, int> & frqMap)
{
	vector<string> tempVect;
	for (const auto &p : frqMap)
	{
		tempVect.push_back(p.first);
	}
	for (int y = 0; y < tempVect.size(); y++)
	{
		for (int x = y + 1; x < tempVect.size(); x++)
		{
			vector<string> row;
			row.push_back(tempVect[y]);
			row.push_back(tempVect[x]);
			frqPairs.push_back(row);
		}
	}
}
void populateFrequentPairs(const vector<vector<string>> & pairs, map<vector<string>, int> & frqPairs, const vector<vector<string>> & dataSet)
{
	//create a map with all pairs as keys
	for (int i = 0; i < pairs.size(); i++)
	{
		vector<string> tempVec;
		tempVec.push_back(pairs[i][0]);
		tempVec.push_back(pairs[i][1]);
		frqPairs[tempVec] = 0;
	}

	//find how many times do those pairs appear in the data set
	for (int y = 0; y < dataSet.size(); y++)
	{
		int x = 0;
		vector<string> vec1; vec1.push_back(dataSet[y][x]); vec1.push_back(dataSet[y][x + 1]);
		vector<string> vec2; vec2.push_back(dataSet[y][x + 1]); vec2.push_back(dataSet[y][x]);
		//check if it's in our map
		if (frqPairs.count(vec1)) { frqPairs[vec1]++; }
		else if (frqPairs.count(vec2)) { frqPairs[vec2]++; }
	}
}
map<string, int> supportTesting(const map<vector<string>, int> & ourMap, const float threshold, const int caseCount)
{
	map<string, int> tempMap;
	for (const auto &p : ourMap)
	{
		//if support for item is higher then threshold - add it to new map
		if (float(p.second) / float(caseCount) > threshold)
		{
			string str = "";
			str += p.first[0]; str += " && "; str += p.first[1];
			tempMap[str] = p.second;
			//cout << (float)p.second / (float)caseCount << endl;
		}
	}
	return tempMap;
}