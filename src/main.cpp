// Code Compression and Decompression

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <bitset>

using namespace std;

vector<string> Code;     // Original Code
ofstream Comp;          // Compressed Code File
ofstream Decomp;       // Decompressed Code File
string opcode;        // Original opcode to be compressed
string CC;           // Compressed Code stream
stringstream C;     // Stream after compression
string Dict[8];    // Dictionary


/************Functions for Compression*******************/

// Creating the Dictionary

void MakeDictionary()
{
	vector<string> temp;
	vector<int> count;
	temp = Code;
	string buff;
	for (int i = 0; i < temp.size(); i++)
	{
		count.push_back(1);
		for (int j = (i + 1); j < temp.size(); j++)
		{
			if (temp[i] == temp[j])
			{
				count[i] = count[i] + 1;
				temp.erase(temp.begin() + j);
				j--;
			}
		}
	}
	for (int i = 1; i < count.size(); i++)
	{
		int j = i;
		int tmp;
		while (j > 0 && count[j - 1] < count[j]) {
			tmp = count[j];
			count[j] = count[j - 1];
			count[j - 1] = tmp;
			buff = temp[j];
			temp[j] = temp[j - 1];
			temp[j - 1] = buff;
			j--;
		}
	}
	for (int i = 0; i < 8; i++)
	{
		Dict[i] = temp[i];
	}
}

// Compression of the code

void Compress()
{
	string opcode;
	string tmp;
	string mask;
	bitset<32> hamming;
	int dist[8];
	int min_val;
	int index = 0;
	for (int i = 0; i < Code.size(); i++)
	{
		opcode = Code[i];
		// checking for RLE
		if (i > 0)        //RLE only applicable from 2nd element
		{
			if (opcode == Code[i - 1])
			{
				int count = 0;
				for (int j = 1; j < 4; j++)
				{
					if (Code[i + j] == opcode)
					{
						count++;
					}
					if (Code[i + j] != opcode)
					{
						goto fin;
					}
				}
			fin:
				C << "000" << bitset<2>(count);
				i = i + count;
				goto here;
			}
		}
		// checking for Direct Mapping
		for (int j = 0; j < 8; j++)
		{
			if (opcode == Dict[j])
			{
				C << "101" << bitset<3>(j);
				goto here;
			}
		}
		// Checker for number of mismatches
		for (int j = 0; j < 8; j++)
		{
			hamming = bitset<32>(opcode) ^ bitset<32>(Dict[j]);
			dist[j] = hamming.count();
		}
		index=0;
		for (int k = 0; k< 8; k++)
		{
			if (dist[k] < dist[index])
			{
				index = k;
			}
		}
		min_val = dist[index];
		hamming = bitset<32>(opcode) ^ bitset<32>(Dict[index]);
		tmp = hamming.to_string();
		// if only 1 mismatch
		if (min_val < 5)
		{
			if (min_val == 1)
			{
				int pos = tmp.find("1");
				C << "010" << bitset<5>(pos) << bitset<3>(index);
				goto here;
			}
			else
			{
				int pos1 = tmp.find("1");
				int pos2 = tmp.find_last_of("1");
				mask = tmp.substr(pos1, (pos2 - pos1 + 1));
				long long  val = stoll(mask, nullptr, 2);
		// if 2 mismatches
				if (min_val == 2)
				{
				// if 2 consecutive mismatches
					if (val == 3)
					{
						C << "011" << bitset<5>(pos1) << bitset<3>(index);
						goto here;
					}
                // if 2 mismatches greater than 4 spaces apart
					else if (val > 9)
					{
						C << "100" << bitset<5>(pos1) << bitset<5>(pos2) << bitset<3>(index);
						goto here;
					}
				// if 2 mismatches less than 4 spaces apart
					else
					{
						goto bitmask;
					}
				}
				else if (min_val == 3 || min_val == 4)
				{
			// bitmask encoding if possible
				bitmask:
					if (val < 16)
					{
						C << "001" << bitset<5>(pos1) << tmp.substr(pos1, 4) << bitset<3>(index);
						goto here;
					}
				}
			}
		}
		// original code
		C << "110" << Code[i];
	here:   i = i+1-1;  // nop
	}
}

// Putting the compressed code onto the output file

void Print()
{
	Comp.open("cout.txt", std::ofstream::out);
	string output = C.str();
	while (output.length() >= 32)
	{
		Comp<< output.substr(0, 32) << endl;
		output = output.substr(32);
	}
	if (!output.empty())
	{
		Comp << output;
		for (int i = 0; i < (32 - output.length()); i++)
		{
			Comp << "1";
		}
		Comp << endl;
	}
	Comp << "xxxx" << endl;
	for (int i = 0; i < 8; i++)
	{
		Comp << Dict[i];
		if (i != 7)
		{
			Comp << endl;
		}
	}
}

// Reading the whole code and putting it onto a vector

void Initialization()
{
	ifstream OgCode;
	string Buffer;
	string temp;
	OgCode.open("original.txt", std::ifstream::in);
	while (!OgCode.eof())
	{
		getline(OgCode, Buffer);
		if (!Buffer.empty())
		{
			Code.push_back(Buffer.substr(0,32));
		}
	}
	OgCode.close();
}


/****************Functions for Decompression*******************************/

//Seperating out the compressed code and dictionary 

void Extraction()
{
	ifstream Compressed;
	string Buffer;
	stringstream temp;
	Compressed.open("compressed.txt", std::ifstream::in);
	while (!Compressed.eof())
	{
		getline(Compressed, Buffer);
		if (Buffer.length() != 32 || Buffer.length() != 4 )
		{
			Buffer=Buffer.substr(0,Buffer.length()-1);
		}
		if(!Buffer.empty())
		{
			temp << Buffer;
			if (Buffer=="xxxx")
			{
				for (int i = 0; i < 8; i++)
				{
					getline(Compressed, Buffer);
					if (Buffer.length()!= 32)
					{
						Buffer=Buffer.substr(0,32);
					}
					Dict[i] = Buffer;
				}
			}
		}
	}
	Compressed.close();
	CC = temp.str();
}

// Decompressing the code 

void Decompress()
{
	Decomp.open("dout.txt", std::ofstream::out);
	int pos;
	int ind;
	int pos1;
	int pos2;
	bitset<32> op;
	bitset<4> mask;
	int runs;
	string opc;

	while (!CC.empty())
	{
		int type = stoi(CC.substr(0, 3), nullptr, 2);
		CC = CC.substr(3);
		if (!opc.empty() && type != 7)
		{
			Decomp << endl;
		}
		switch (type)
		{
		case 0:
			runs = stoi(CC.substr(0, 2), nullptr, 2);
			CC = CC.substr(2);
			for (int i = 0; i <= runs; i++)
			{
				Decomp << opc;
				if (i != runs)
				{
					Decomp << endl;
				}
			}
			break;
		case 1:
			pos = stoi(CC.substr(0, 5), nullptr, 2);
			CC = CC.substr(5);
			mask = bitset<4>(CC.substr(0, 4));
			CC = CC.substr(4);
			ind = stoi(CC.substr(0, 3), nullptr, 2);
			CC = CC.substr(3);
			op = bitset<32>(Dict[ind]);
			for (int i = 3; i >= 0; i--)
			{
				if (mask[i] == 1)
				{
					op.flip(31 - pos - 3 + i);
				}
			}
			opc = op.to_string();
			Decomp << opc;
			break;
		case 2:
			pos = stoi(CC.substr(0, 5), nullptr, 2);
			CC = CC.substr(5);
			ind = stoi(CC.substr(0, 3), nullptr, 2);
			CC = CC.substr(3);
			op = bitset<32>(Dict[ind]);
			op.flip(31 - pos);
			opc = op.to_string();
			Decomp << opc;
			break;
		case 3:
			pos = stoi(CC.substr(0, 5), nullptr, 2);
			CC = CC.substr(5);
			ind = stoi(CC.substr(0, 3), nullptr, 2);
			CC = CC.substr(3);
			op = bitset<32>(Dict[ind]);
			op.flip(31 - pos);
			op.flip(31 - pos - 1);
			opc = op.to_string();
			Decomp << opc;
			break;
		case 4:
			pos1 = stoi(CC.substr(0, 5), nullptr, 2);
			CC = CC.substr(5);
			pos2 = stoi(CC.substr(0, 5), nullptr, 2);
			CC = CC.substr(5);
			ind = stoi(CC.substr(0, 3), nullptr, 2);
			CC = CC.substr(3);
			op = bitset<32>(Dict[ind]);
			op.flip(31 - pos1);
			op.flip(31 - pos2);
			opc = op.to_string();
			Decomp << opc;
			break;
		case 5:
			opc = Dict[stoi(CC.substr(0, 3), nullptr, 2)];
			Decomp << opc;
			CC = CC.substr(3);
			break;
		case 6:
			opc = CC.substr(0, 32);
			CC = CC.substr(32);
			Decomp << opc;
			break;
		case 7:
			CC.clear();
			break;
		}
	}
}

/******************** Main Function ********************************/


int main(int argc, char *argv[])
{
	string parameter = argv[1];
	if (parameter == "1") // Parameter 1 for Compression
	{
		Initialization();
		MakeDictionary();
		Compress();
		Print();
	}
	else if (parameter == "2") // Parameter 2 for Decompression
	{
		Extraction();
		Decompress();
	}
	else  // incase of exceptions
	{
		cout << "please input the correct parameter" << endl;
	}
	return 0;
}
