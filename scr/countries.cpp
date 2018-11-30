#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <math.h>
#include <algorithm>

using namespace std;

//working with files:
void openFile(string fileName, ifstream & ourFile); //works
void closeFile(ifstream & ourFile); //works

//getting the data in
void populateVector(ifstream & ourFile, vector<vector<string>> & ourData); //works
void displayVector(const vector<vector<string>> & ourData); //works

//making a new boolian-based table
//update to handle multiple types of data
struct attributeStats
{
	string name;
	string type;
	//numerical stuff
	float lowerBound;
	float upperBound;
	float spread;
	//categorical
	vector<string> categories;
	string category;
};
void populateAttributes(vector<attributeStats> & attributes, const vector<vector<string>> & countryData, const vector<bool> & isNumerical); //works
void populateHeader(vector<attributeStats> & header, vector<attributeStats> & attributes, const int divider, const vector<bool> & isNumerical); //works
void populateBody(vector<vector<bool>> & body, vector<attributeStats> & header, vector<vector<string>> & countryData, const int divider); //works in a hacky way

//association rule learning algorithm:
void apriori(const vector<attributeStats> & header, const vector<vector<bool>> & body, vector<vector<int>> & currentItemsets, const float minSupport, const float minConfidence, int counter, const vector<vector<string>> & countryData);
void combinations(const vector<vector<int>> & inputVector, vector<int> & tempVector, int start, int end, int index, int combinationSize, vector<vector<int>> & newCombinations);
//for confidence calculations:"
float sup(const vector<vector<bool>> & body, const vector<int> & itemset);
void allSubsetsExceptEmpty(const vector<int> & itemset, vector<vector<int>> & subsets);


int main()
{
	//opening our file
	ifstream ourFile;
	//string fileName = "country_stats.csv";
	string fileName = "music_project_og.csv";
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
	vector<attributeStats> header;
	vector<vector<bool>> body;
	const vector<bool> isNumerical = { 0, 1, 0, 0, 1, 1 };
	populateAttributes(attributes, countryData, isNumerical);
	populateHeader(header, attributes, divider, isNumerical);
	populateBody(body, header, countryData, divider); //still needs an update



	//association rule mining algorithm:
	const float minSupport = 0.3f; //% of the whole dataset
	const float minConfidence = 0.6f; //% of records that have X, that also have Y
	vector<vector<int>> currentItemsets;
	cout << endl << "Divider for numerical attributes: " << divider << endl << "Minimal Support: " << minSupport << endl << "Minimal Confidense: " << minConfidence << endl << endl;
	apriori(header, body, currentItemsets, minSupport, minConfidence, 0, countryData); //updated

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

void populateAttributes(vector<attributeStats> & attributes, const vector<vector<string>> & countryData, const vector<bool> & isNumerical)
{
	for (int x = 0; x < countryData[0].size(); x++)
	{
		attributeStats stat;
		stat.name = countryData[0][x];
		//add stat type things here
		if (isNumerical[x] == true) {
			stat.type = "numerical";
			for (int y = 1; y < countryData.size(); y++)
			{
				if (y == 1)
				{
					stat.lowerBound = stof(countryData[y][x]);
					stat.upperBound = stof(countryData[y][x]);
				}
				else if (stof(countryData[y][x]) < stat.lowerBound) { stat.lowerBound = stof(countryData[y][x]); }
				else if (stof(countryData[y][x]) > stat.upperBound) { stat.upperBound = stof(countryData[y][x]); }
			}
			stat.spread = stat.upperBound - stat.lowerBound;
		}
		else //if it's a categorical value
		{
			stat.type = "categorical";
			//populate the categories here
			vector<string> allCategories;
			for (int y = 1; y < countryData.size(); y++)
			{
				bool isContained = false;
				for (int q = 0; q < stat.categories.size(); q++)
				{
					if (countryData[y][x] == stat.categories[q]) { isContained = true; }
				}
				if (!isContained) { stat.categories.push_back(countryData[y][x]); } //if the item not yet in list of categories for the attribute
			}
			if (!stat.categories.empty()) { sort(stat.categories.begin(), stat.categories.end()); }
		}
		attributes.push_back(stat);
		//for debugging
		/*if (stat.type == "numerical") {
			cout << "Name: " << stat.name << "   Min:" << stat.lowerBound << "   Max:" << stat.upperBound << "   Spread:" << stat.spread << endl;
		}
		else {
			cout << "Name: " << stat.name << " Categories: ";
			for (int z = 0; z < stat.categories.size(); z++)
			{
				if (z != stat.categories.size() - 1) { cout << stat.categories[z] << ", "; }
				else { cout << stat.categories[z] << "  "; }
			}
			cout << "  (" << stat.categories.size() << " categories)" << endl;
		}*/

	}
	cout << "Attributes created!" << endl;
}
void populateHeader(vector<attributeStats> & header, vector<attributeStats> & attributes, const int divider, const vector<bool> & isNumerical)
{
	cout << "Divider of " << divider << " for numerical variables." << endl;
	for (int i = 0; i < attributes.size(); i++)
	{
		if (attributes[i].type == "numerical") {
			for (int d = 0; d < divider; d++)
			{
				attributeStats atr;
				atr.name = attributes[i].name;
				atr.type = "numerical";
				atr.lowerBound = attributes[i].lowerBound + (attributes[i].spread / divider * d);
				atr.upperBound = atr.lowerBound + (attributes[i].spread / divider); //rounding error might be because of this
				header.push_back(atr);
				//debugging
				//cout << "Name: " << atr.name << "   Range: " << atr.lowerBound << " - " << atr.upperBound << endl;
			}
		}
		else
		{
			//create an attribute for each discrete category, from original attribute
			for (int x = 0; x < attributes[i].categories.size(); x++)
			{
				attributeStats atr;
				atr.name = attributes[i].name;
				atr.type = "categorical";
				atr.category = attributes[i].categories[x];
				header.push_back(atr);
				//debugging
				//cout << "Name: " << atr.name << "  Category: " << atr.category << endl;
			}
		}
	}
	//cout << endl << endl;
	cout << "Header populated!" << endl;
}
void populateBody(vector<vector<bool>> & body, vector<attributeStats> & header, vector<vector<string>> & countryData, const int divider)
{
	for (int y = 1; y < countryData.size(); y++) //all records in database, start from 1 since 0 is header
	{
		vector<bool> tempVec;
		//int trueCount = 0;
		for (int x = 0; x < countryData[y].size(); x++) //horizontal size of original database
		{
			for (int i = 0; i < header.size(); i++) //for all of our attributes, that were "sliced", either numerical or not
			{
				if (countryData[0][x] == header[i].name) {
					if (header[i].type == "numerical") //if a numerical attribute
					{
						float num = stof(countryData[y][x]);
						if (num >= header[i].lowerBound && num <= header[i].upperBound + 0.0001)
						{
							tempVec.push_back(true);
							//trueCount++;
						}
						else
						{
							tempVec.push_back(false);
						}
					}
					else //categorical attribute
					{
						if (countryData[y][x] == header[i].category) {
							tempVec.push_back(true);
							//trueCount++; 
						}
						else { tempVec.push_back(false); }
					}
				}
			}

		}
		body.push_back(tempVec);
		//cout << "trueCount: " << trueCount; //for checking number of true values should be equal to number of attributes
		//cout << endl;
	}
	cout << "Body populated!" << endl;
}

void apriori(const vector<attributeStats> & header, const vector<vector<bool>> & body, vector<vector<int>> & currentItemsets, const float minSupport, const float minConfidence, int counter, const vector<vector<string>> & countryData)
{
	cout << "Level " << counter << endl;
	int populationSize = body.size();
	vector<float> support;

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
		newCombinations.clear();
		//what left: get rid of duplicates =)
		for (int y = 0; y < currentItemsets.size(); y++)
		{
			for (int x = 0; x < currentItemsets[y].size(); x++)
			{
				if (x != currentItemsets[y].size() - 1)
				{
					if (currentItemsets[y][x] == currentItemsets[y][x + 1]) { //cout << "got rid of duplicate" << endl; 
						break;
					}
				}
				else
				{
					newCombinations.push_back(currentItemsets[y]);
				}
			}
		}
		currentItemsets = newCombinations;
		if (currentItemsets.size() == 0)
		{
			cout << "No frequent itemsets left." << endl;
			return;
		}
	}



	//actual logic
	int supCount = 1;
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
		if (float(frq) / populationSize >= minSupport)
		{
			if (supCount == 1) { cout << "-----Frequent itemsets that satisfy the support requirement of " << minSupport << "-----" << endl; }
			cout << supCount << ". ";
		}
		for (int i = 0; i < currentItemsets[atr].size(); i++) {
			if (float(frq) / populationSize < minSupport) { continue; }
			//add display option for numerical and categorical
			if (header[currentItemsets[atr][i]].type == "numerical") {
				cout << header[currentItemsets[atr][i]].name << " <" << header[currentItemsets[atr][i]].lowerBound << ", " << header[currentItemsets[atr][i]].upperBound << ">  ";
			}
			else
			{
				cout << header[currentItemsets[atr][i]].name << " <" << header[currentItemsets[atr][i]].category << ">  ";
			}
			if (currentItemsets[atr].size() > 1 && i != currentItemsets[atr].size() - 1) { cout << " && "; }
		}
		if (float(frq) / populationSize >= minSupport) {
			cout << "   frq = " << frq << "   support = " << float(frq) / populationSize << endl;
			supCount++;
		}
		//cout << "   frq =" << frq << "   support = " << float(frq) / populationSize << endl;
		support.push_back(float(frq) / populationSize);
	}

	//end conditions
	if (currentItemsets.size() == 0) { cout << "No frequent itemsets left!" << endl; return; }
	else if (counter == countryData.size() - 1) { cout << "Maximumum number of recursions reached" << endl; return; }
	//else if (counter == 3) { cout << "Exited successfully on #th recursion" << endl; return; }


	//prep our currentItemsets vector for next "run" (remove things that are bellow support & confidence) DONT forget about confidence, to do it later
	vector<vector<int>> newCurrentItemsets;
	for (int i = 0; i < currentItemsets.size(); i++)
	{
		if (support[i] >= minSupport)
		{
			newCurrentItemsets.push_back(currentItemsets[i]);
		}
	}
	cout << endl;
	//association rule finding 
	int confCounter = 1;
	if (counter != 0) {
		cout << "-----Association rules that satisfy the confidence requirement of " << minConfidence << "-----" << endl;
		for (int y = 0; y < newCurrentItemsets.size(); y++) //for all frequent itemsets that satisfied the support criteria
		{
			vector<vector<int>> subsets;
			allSubsetsExceptEmpty(newCurrentItemsets[y], subsets);
			//go through all subsets
			for (int i = 0; i < subsets.size(); i++)
			{
				for (int x = 0; x < subsets.size(); x++)
				{
					if (x == i) { continue; } //if it's the same subset
					else if (subsets[i].size() + subsets[x].size() != newCurrentItemsets[y].size()) { continue; } //if two subsets are smaller in size than a frequent itemset
					else  //candidate for confidence testing
					{
						bool unique = true;
						for (int k = 0; k < subsets[i].size(); k++) //test if there are any conflicting attributes between subsets
						{
							for (int j = 0; j < subsets[x].size(); j++)
							{
								if (subsets[i][k] == subsets[x][j]) { unique = false; }
							}
						}
						if (!unique) { break; }
						if (subsets[i].size() + subsets[x].size() == newCurrentItemsets[y].size())
						{
							float support = sup(body, newCurrentItemsets[y]);
							float confidence = support / sup(body, subsets[i]);
							if (confidence >= minConfidence)
							{
								cout << confCounter << ". ";
								confCounter++;
								for (int h = 0; h < subsets[i].size(); h++)
								{
									if (header[subsets[i][h]].type == "numerical") {
										cout << header[subsets[i][h]].name << " <" << header[subsets[i][h]].lowerBound << ", " << header[subsets[i][h]].upperBound << "> ";
									}
									else
									{
										cout << header[subsets[i][h]].name << " <" << header[subsets[i][h]].category << "> ";
									}
									if (subsets[i].size() > 1 && h != subsets[i].size() - 1) { cout << "&& "; }
								}
								cout << " => ";
								for (int h = 0; h < subsets[x].size(); h++)
								{
									if (header[subsets[x][h]].type == "numerical") {
										cout << header[subsets[x][h]].name << " <" << header[subsets[x][h]].lowerBound << ", " << header[subsets[x][h]].upperBound << "> ";
									}
									else
									{
										cout << header[subsets[x][h]].name << " <" << header[subsets[x][h]].category << "> ";
									}
									if (subsets[x].size() > 1 && h != subsets[x].size() - 1) { cout << "&& "; }
								}
								cout << "   with support of " << support << " and confidence of " << confidence << "" << endl;
							}
						}
					}
				}
			}
		}
	}
	//prep for next "level" in recursion
	//update counter:
	counter++;
	cout << endl << endl;
	apriori(header, body, newCurrentItemsets, minSupport, minConfidence, counter, countryData);
}

void combinations(const vector<vector<int>> & inputVector, vector<int> & tempVector, int start, int end, int index, int combinationSize, vector<vector<int>> & newCombinations)
{

	//current combination is ready
	if (index == combinationSize)
	{
		/*for (int i = 0; i < combinationSize; i++)
		{
			cout << tempVector[i] << " ";
		}
		cout << endl;*/
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
float sup(const vector<vector<bool>> & body, const vector<int> & itemset)
{
	float sup;
	int frq = 0;
	for (int y = 0; y < body.size(); y++)
	{
		bool isTrue = true;
		for (int i = 0; i < itemset.size(); i++)
		{
			if (body[y][itemset[i]] == false) { isTrue = false; break; }
		}
		if (isTrue) { frq++; }
	}
	sup = float(frq) / body.size();
	return sup;
}
void allSubsetsExceptEmpty(const vector<int> & itemset, vector<vector<int>> & subsets)
{
	unsigned int powSetSize = pow(2, itemset.size());

	for (int counter = 1; counter < powSetSize; counter++)
	{
		vector<int> tempVec;
		for (int j = 0; j < itemset.size(); j++)
		{
			if (counter & (1 << j)) { tempVec.push_back(itemset[j]); }
		}
		subsets.push_back(tempVec);
	}
}