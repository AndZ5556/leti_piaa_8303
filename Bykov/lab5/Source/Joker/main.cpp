
#include <iostream>
#include <cstring>
#include <vector>
#include <string>

using namespace std;
class Bohr
{
private:
	string alphabet = "ACGTN";

	struct Numbers
	{
		int index; // ������ � ������
		int patternNum; // ������ � ������ ��������
	};

	struct BohrPoint
	{
		int directLinks[5]; // �������� � ����
		int patternNum[40]; // ����� ��������
		int suffixLink;// ���������� ������
		int charrLink[5]; // �������� � ��������
		int parentIndex; // ������ ������� ��������
		int compressedLink; // ������� �� �������� ������
		bool terminal; // �������� �� ������� ��������
		char charr; // ������ �������� �� ��������
	};

	vector <BohrPoint> bohr; // ���
	string text; // ������ ������
	string patternStr; // ������ c ���������
	vector <string> patterns; // ������ �������� �������� - ���������
	vector <int> patternPos; // ������ �������� �������� ���������
	vector<Numbers> num; // ������ ������� ��������� ���������

public:
	Bohr() // �������� ����
	{
		cout << "Creating bohr\n";
		bohr.push_back(createPoint(-1, -1));
		char joker;
		cin >> text >> patternStr >> joker;
		for (int i = 0; i < patternStr.size(); i++)
		{
			string pat;
			if (patternStr[i] != joker)
			{
				patternPos.push_back(i + 1);
				for (int j(i); patternStr[j] != joker && j != patternStr.length(); j++)
				{
					pat += patternStr[j];
					i++;
				}
				cout << "\n-----\nNew pattern - " << pat << endl;
				cout << "Add symbols of new pattern in prefix tree\n";
				patterns.push_back(pat);
				addString(pat); // ��������� ������� � ���
			}
		}
	}

	BohrPoint createPoint(int p, char c) // ������� ��� �������� ������� � ����
	{
		BohrPoint v;
		memset(v.directLinks, -1, sizeof(v.directLinks));
		memset(v.charrLink, -1, sizeof(v.charrLink));
		memset(v.patternNum, -1, sizeof(v.patternNum));
		v.terminal = false;
		v.suffixLink = -1;
		v.parentIndex = p;
		v.charr = c;
		v.compressedLink = -1;
		return v;
	}

	void addString(string& s) // ������� ��� ���������� ��������
	{
		int index = 0;
		for (int i = 0; i < s.size(); i++)
		{
			cout << "\nReceived character " << s[i] << endl;
			char ch = alphabet.find(s[i]);
			if (bohr[index].directLinks[ch] == -1) // ���� ������ ��������� � ���� ���, �� ��������� �����
			{
				cout << "Such way not found, adding new point with number " << bohr.size() << endl;
				bohr.push_back(createPoint(index, ch));
				bohr[index].directLinks[ch] = bohr.size() - 1;
			}
			index = bohr[index].directLinks[ch];
			cout << "Switching to point - " << index << "\n";
		}
		bohr[index].terminal = true;
		for (int i = 0; i < 40; i++)
		{
			if (bohr[index].patternNum[i] == -1)
			{
				bohr[index].patternNum[i] = patterns.size() - 1; // ���������� ����� ��������
				break;
			}
		}
	}

	int getSuffixLink(int point) // ������� ��� ��������� ���������� ������
	{
		if (bohr[point].suffixLink == -1) // ���� ���������� ������ �� ����������
			if (point == 0 || bohr[point].parentIndex == 0) // ���� ������� ������� - ������ ��� ���� ������������ ������� - ������
				bohr[point].suffixLink = 0; // ���������� 0
			else // ����� ���� ������ �� ������ ������
				bohr[point].suffixLink = getLink(getSuffixLink(bohr[point].parentIndex), bohr[point].charr);
		if (bohr.at(point).suffixLink)
			cout << "Suffix link to point " << bohr.at(point).suffixLink << "!\n";
		else
			cout << "Suffix link not found\n";
		return bohr.at(point).suffixLink;
	}

	int getLink(int point, char charr) // ������� ��� ��������� ������� ������
	{
		if (bohr[point].charrLink[charr] == -1) // ���� ������ ���
			if (bohr[point].directLinks[charr] != -1) // ��������� ���� �� ������ � ����
				bohr[point].charrLink[charr] = bohr[point].directLinks[charr];
			else // ������� ����� ���������� ������
			{
				if (point == 0)
					bohr[point].charrLink[charr] = 0;
				else
					bohr[point].charrLink[charr] = getLink(getSuffixLink(point), charr);
			}

		cout << "link to point " << bohr.at(point).charrLink[charr] << " by char " << alphabet[charr] << endl;
		return bohr.at(point).charrLink[charr];
	}

	int getCompressedLink(int point) // ������� ��� ��������� �������� ������
	{
		if (bohr[point].compressedLink == -1) // ���� �������� ������ ���
		{
			int curr = getSuffixLink(point);
			if (curr == 0)
				bohr[point].compressedLink = 0;
			else
			{
				if (bohr[curr].terminal) // ���� ���������� ������ ��������� �� �������� �������
					bohr[point].compressedLink = curr; // ���������� �������� ������ �� �������� �������
				else // ����� ������� ��������� ������� ��� ��������� �������
					bohr[point].compressedLink = getCompressedLink(curr);
			}
		}
		if (bohr.at(point).compressedLink)
			cout << "Compressed link to point " << bohr.at(point).compressedLink << "!\n";
		else
			cout << "Compressed link not found" << endl;
		return bohr.at(point).compressedLink;
	}

	void find(int v, int i) // ������� ��� �������� �������������� ������� ������� � ������� ������ �������� ������
	{
		struct Numbers s;
		for (int u(v); u != 0; u = getCompressedLink(u))
		{
			if (bohr[u].terminal) // ���� ������� �������� �������
			{
				cout << "Terminal point was found!" << endl;
				for (int j(0); j < 40; j++)
				{
					if (bohr[u].patternNum[j] != -1)
					{
						s.index = i - patterns[bohr[u].patternNum[j]].size(); // ���������� ������ � ������
						s.patternNum = bohr[u].patternNum[j]; // ���������� ����� ��������
						num.push_back(s);
						cout << "\n-----\nPattern was found in index " << s.index << ". Pattern number - " << s.patternNum << "\n-----\n\n";
					}
					else
						break;
				}
			}
		}
	}

	void AHO() // ������ ��������� ���-�������
	{
		cout << "\n-----\nSearching patterns in text \n\n";
		int u = 0;
		for (int i(0); i < text.size(); i++) // ������� ���� �������� ������
		{
			cout << "-----\nNew char: " << text.at(i) << endl << endl;
			u = getLink(u, alphabet.find(text[i]));
			find(u, i + 1);
		}
	}

	void print() // ����� �����������
	{
		cout << "\n\n-----\nAnalysing found patterns\n";
		vector <int> c(text.size(), 0);
		for (int i(0); i < num.size(); i++)
		{
			cout << "Current pattern - " << patterns[num[i].patternNum] << endl;
			if (num[i].index < patternPos[num[i].patternNum] - 1)
				continue;
			c[num[i].index - patternPos[num[i].patternNum] + 1]++;
			cout << "On index " << num[i].index - patternPos[num[i].patternNum] + 2 << " - coincidence " << c[num[i].index - patternPos[num[i].patternNum] + 1] << " out of " << patterns.size() << endl;
			if (c[num[i].index - patternPos[num[i].patternNum] + 1] == patterns.size() &&
				num[i].index - patternPos[num[i].patternNum] + 1 <= text.size() - patternStr.size())
				cout << "Whole patter was found on " << num[i].index - patternPos[num[i].patternNum] + 2 << endl;
		}

		cout << "\n-----\nVariant 2 -- Indivualization" << endl;
		bool flag = true;
		for (int i = 0; i < num.size(); i++)
		{

			int a, b;
			for (int j = 0; j < num.size(); j++)
			{
				if (num.at(j).index > num.at(i).index)
				{
					a = j;
					b = i;
				}
				else
				{
					a = i;
					b = j;
				}
				if (i != j && num.at(a).index < num.at(b).index + patterns[num[b].patternNum].size())
				{
					flag = false;
					cout << "Adjacent patterns: pattern " << num.at(b).patternNum << " and " << num.at(a).patternNum;
					cout << ", index " << num.at(b).index << " and " << num.at(a).index << endl;
				}
			}
			num.erase(num.begin());

		}
		cout << "Points number: " << bohr.size() << endl;
		if (flag) cout << "No adjusting patterns!\n";
		printBohr(); // ����� ��������

	}

	void printBohr()
	{
		cout << "\n-----\nMachine built during the operation of the algorithm\n\n";
		for (int i = 0; i < bohr.size(); i++)
		{
			cout << "Point " << i << " with paths: \n";
			for (int j = 0; j < 5; j++)
				if (bohr.at(i).charrLink[j] != -1)
					cout << "\tPoint " << bohr.at(i).charrLink[j] << " with paht " << alphabet[j] << endl;
		}
	}
};

int main()
{

	Bohr obj;
	obj.AHO();
	obj.print();
	return 0;
}/*ACGNCGTACGT ACG%CGT %*/