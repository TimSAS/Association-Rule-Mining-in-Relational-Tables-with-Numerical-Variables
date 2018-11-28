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
void populateHeader(vector<attributeWithRange> & header, vector<attributeStats> & attributes);
void populateBody(vector<vector<bool>> & body, vector<attributeWithRange> & header, vector<vector<string>> & countryData);

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

	//Mapping the Quantitative Associatin Rules Problem into the Boolean Association Rules Problem
	//first row(first vector): set of <attribute, range>
	//other rows(second vector): table of 0 or 1s
	const int divider = 2; //in how many pieces to divide range of a certain attribute
	vector<attributeStats> attributes;
	vector<attributeWithRange> header;
	vector<vector<bool>> body;
	populateAttributes(attributes, countryData);
	//populateHeader(header, attributes);
	//populateBody(body, header, countryData);



	//association rule mining algorithm:
	const float minSupport = 0.4f;
	const float minConfidence = 0.5f;

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
void populateHeader(vector<attributeWithRange> & header, vector<attributeStats> & attributes)
{}
void populateBody(vector<vector<bool>> & body, vector<attributeWithRange> & header, vector<vector<string>> & countryData)
{}