#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <map>

using namespace std;

//working with files:
void openFile(string fileName, ifstream & ourFile); //works
void closeFile(ifstream & ourFile); //works

//getting the data in
void populateVector(ifstream & ourFile, vector<vector<string>> & ourData); //works
void displayVector(const vector<vector<string>> & ourData); //works

//making a new boolian-based table
struct attributeWithRange
{
	string name;
	float lowerBound;
	float upperBound;
};
struct attributeStats
{
	string name;
	float minVal;
	float maxVal;
	float spread;
};
void populateAttributes(vector<attributeStats> & attributes, const vector<vector<string>> & countryData); //works
void populateHeader(vector<attributeWithRange> & header, vector<attributeStats> & attributes, const int divider); //works
void populateBody(vector<vector<bool>> & body, vector<attributeWithRange> & header, vector<vector<string>> & countryData, const int divider); //works in a hacky way

//association rule learning algorithm:
void apriori(const vector<attributeWithRange> & header, const vector<vector<bool>> & body, vector<vector<int>> & currentItemsets, const float minSupport, const float minConfidence, int counter);
void combinations(const vector<vector<int>> & inputVector, vector<int> & tempVector, int start, int end, int index, int combinationSize, vector<vector<int>> & newCombinations);

int main()
{
	//opening our file
	ifstream ourFile;
	string fileName = "country_stats.csv";
	openFile(fileName, ourFile);

	//getting the data into a 2-dimentional vector:
	vector< vector<string> > countryData;
	populateVector(ourFile, countryData);
	//displayVector(countryData);

	//Mapping the Quantitative Associatin Rules Problem into the Boolean Association Rules Problem
	//first row(first vector): set of <attribute, range>
	//other rows(second vector): table of 0 or 1s
	const int divider = 5; //in how many pieces to divide range of a certain attribute
	vector<attributeStats> attributes;
	vector<attributeWithRange> header;
	vector<vector<bool>> body;
	populateAttributes(attributes, countryData);
	populateHeader(header, attributes, divider);
	populateBody(body, header, countryData, divider);



	//association rule mining algorithm:
	const float minSupport = 0.3f; //% of the whole dataset
	const float minConfidence = 0.5f; //% of records that have X, that also have Y
	vector<vector<int>> currentItemsets;
	apriori(header, body, currentItemsets, minSupport, minConfidence, 0);

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

void populateAttributes(vector<attributeStats> & attributes, const vector<vector<string>> & countryData)
{
	for (int x = 0; x < countryData[0].size(); x++)
	{
		attributeStats stat;
		stat.name = countryData[0][x];
		for (int y = 1; y < countryData.size(); y++)
		{
			if (y == 1)
			{
				stat.minVal = stof(countryData[y][x]);
				stat.maxVal = stof(countryData[y][x]);
			}
			else if (stof(countryData[y][x]) < stat.minVal) { stat.minVal = stof(countryData[y][x]); }
			else if (stof(countryData[y][x]) > stat.maxVal) { stat.maxVal = stof(countryData[y][x]); }
		}
		stat.spread = stat.maxVal - stat.minVal;
		attributes.push_back(stat);
		//for debugging
		//cout << "Name: " << stat.name << "   Min:" << stat.minVal << "   Max:" << stat.maxVal << "   Spread:" << stat.spread << endl;
	}
}
void populateHeader(vector<attributeWithRange> & header, vector<attributeStats> & attributes, const int divider)
{
	for (int i = 0; i < attributes.size(); i++)
	{
		for (int d = 0; d < divider; d++)
		{
			attributeWithRange atr;
			atr.name = attributes[i].name;
			atr.lowerBound = attributes[i].minVal + (attributes[i].spread / divider * d);
			atr.upperBound = atr.lowerBound + (attributes[i].spread / divider); //rounding error might be because of this
			header.push_back(atr);
			//debugging
			cout << "Name: " << atr.name << "   Range: " << atr.lowerBound << " - " << atr.upperBound << endl;
		}
	}
	cout << endl << endl;
}
void populateBody(vector<vector<bool>> & body, vector<attributeWithRange> & header, vector<vector<string>> & countryData, const int divider)
{
	for (int y = 1; y < countryData.size(); y++) //all records in database, start from 1 since 0 is header
	{
		vector<bool> tempVec;
		int count = 0;
		for (int x = 0; x < countryData[y].size(); x++) //horizontal size of original database
		{
			for (int z = 0; z < divider; z++) //how many divisions do we apply to our attributes
			{
				float num = stof(countryData[y][x]);
				if (num >= header[x * divider + z].lowerBound && num <= header[x * divider + z].upperBound + 0.0001) //TODO, fix the bug with rounding or something 
				{
					tempVec.push_back(true);
					count++;
				}
				else
				{
					//cout << num << " " << header[x * divider + z].lowerBound << " " << header[x * divider + z].upperBound << endl;

					tempVec.push_back(false);
				}
				//cout << tempVec[x * divider + z] << " ";
			}
		}
		body.push_back(tempVec);
		//cout << count; //for checking number of true values should be equal to number of attributes
		//cout << endl;
	}
}

void apriori(const vector<attributeWithRange> & header, const vector<vector<bool>> & body, vector<vector<int>> & currentItemsets, const float minSupport, const float minConfidence, int counter)
{
	cout << "Level " << counter << endl;
	int populationSize = body.size();
	vector<float> support;
	vector<float> confidence;

	if (counter == 0) //initial "fill-in" of currentItemsets vector
	{
		for (int i = 0; i < header.size(); i++)
		{
			vector<int> tempVec;
			tempVec.push_back(i);
			currentItemsets.push_back(tempVec);
		}
	}
	else //if not initial "run", update the currentItemsets vector kinda hard... =(
	{
		//strategy:
		//1: get all possible combinations
		//2: sort vectors 
		//3: get rid of duplicates
		vector<int> tempVector;
		vector<vector<int>> newCombinations;
		tempVector.resize(counter + 1);

		combinations(currentItemsets, tempVector, 0, currentItemsets.size() - 1, 0, counter + 1, newCombinations);
		currentItemsets = newCombinations;

		//what left: get rid of duplicates =)
	}

	//actual logic
	for (int atr = 0; atr < currentItemsets.size(); atr++)
	{
		int frq = 0;
		for (int rec = 0; rec < populationSize; rec++)
		{
			bool isTrue = true;
			for (int i = 0; i < currentItemsets[atr].size(); i++) //go through attributes in a particular itemset
			{
				if (body[rec][currentItemsets[atr][i]] == false) { isTrue = false; }
			}
			if (isTrue) { frq++; }
		}
		for (int i = 0; i < currentItemsets[atr].size(); i++) {
			if (float(frq) / populationSize < minSupport) { continue; }
			cout << header[currentItemsets[atr][i]].name << " " << header[currentItemsets[atr][i]].lowerBound << "-" << header[currentItemsets[atr][i]].upperBound << " ";
			if (currentItemsets[atr].size() > 1 && i != currentItemsets[atr].size() - 1) { cout << " && "; }
		}
		if (float(frq) / populationSize >= minSupport) { cout << "   frq =" << frq << "   support = " << float(frq) / populationSize << endl; }
		support.push_back(float(frq) / populationSize);
	}
	cout << endl;

	//end conditions
	if (counter == 2) { return; }
	else if (currentItemsets.size() == 0) { return; }
	else if (counter == header.size()) { return; }

	//prep for next "level" in recursion
	//update counter:
	counter++;
	//prep our currentItemsets vector for next "run" (remove things that are bellow support & confidence) DONT forget about confidence, to do it later
	vector<vector<int>> newCurrentItemsets;
	for (int i = 0; i < currentItemsets.size(); i++)
	{
		if (support[i] >= minSupport)
		{
			newCurrentItemsets.push_back(currentItemsets[i]);
		}
	}

	apriori(header, body, newCurrentItemsets, minSupport, minConfidence, counter);
}

void combinations(const vector<vector<int>> & inputVector, vector<int> & tempVector, int start, int end, int index, int combinationSize, vector<vector<int>> & newCombinations)
{
	//what left: get rid of duplicates =)

	//current combination is ready
	if (index == combinationSize)
	{
		for (int i = 0; i < combinationSize; i++)
		{
			cout << tempVector[i] << " ";
		}
		cout << endl;
		newCombinations.push_back(tempVector);
		return;
	}

	//replace index with all possible elements. The condition "end - i + 1 >= r - index"
	//makes sure that including one element at index will make a combination with remaining 
	//elements at remaining positions
	for (int i = start; i <= end && end - i + 1 >= combinationSize - index; i++)
	{
		tempVector[index] = inputVector[i][0];
		combinations(inputVector, tempVector, i + 1, end, index + 1, combinationSize, newCombinations);
	}
}