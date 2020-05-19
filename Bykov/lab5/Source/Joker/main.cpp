
#include <iostream>
#include <cstring>
#include <vector>
#include <string>

class Bohr
{
private:
	std::string alphabet = "ACGTN";

	struct Numbers
	{
		int index; // индекс в тексте
		int patternNum; // индекс в строке паттерна
	};

	struct BohrPoint
	{
		int directLinks[5]; // переходы в боре
		int patternNum[40]; // номер паттерна
		int suffixLink;// суффиксная ссылка
		int charrLink[5]; // переходы в автомате
		int parentIndex; // индекс вершины родителя
		int compressedLink; // переход по конечной ссылке
		bool terminal; // является ли вершина конечной
		char charr; // символ перехода от родителя
	};

	std::vector <BohrPoint> bohr; // бор
	std::string text; // строка поиска
	std::string patternStr; // строка c паттерном
	std::vector <std::string> patterns; // вектор хранения подстрок - паттернов
	std::vector <int> patternPos; // вектор хранения индексов паттернов
	std::vector<Numbers> num; // вектор хранния найденных паттернов

public:
	void init() // создание бора
	{
		std::cout << "Creating bohr\n";
		bohr.push_back(createPoint(-1, -1));
		char joker;
		std::cin >> text >> patternStr >> joker;
		for (int i = 0; i < patternStr.size(); i++)
		{
			std::string pat;
			if (patternStr[i] != joker)
			{
				patternPos.push_back(i + 1);
				for (int j(i); patternStr[j] != joker && j != patternStr.length(); j++)
				{
					pat += patternStr[j];
					i++;
				}
				std::cout << "\n-----\nNew pattern - " << pat << std::endl;
				std::cout << "Add symbols of new pattern in prefix tree\n";
				patterns.push_back(pat);
				addString(pat); // добавляем паттерн в бор
			}
		}
	}

	BohrPoint createPoint(int p, char c) // функция для создания вершины в боре
	{
		BohrPoint v;
		for (int i = 0; i < 40; i++)
		{
			if (i < 5)
			{
				v.directLinks[i] = -1;
				v.charrLink[i] = -1;
			}
			v.patternNum[i] = -1;
		}
		v.terminal = false;
		v.suffixLink = -1;
		v.parentIndex = p;
		v.charr = c;
		v.compressedLink = -1;
		return v;
	}

	void addString(std::string& s) // функция для добавления паттерна
	{
		int index = 0;
		for (int i = 0; i < s.size(); i++)
		{
			std::cout << "\nReceived character " << s[i] << std::endl;
			char ch = alphabet.find(s[i]);
			if (bohr[index].directLinks[ch] == -1) // если такого состояния в боре нет, то добавляем новое
			{
				std::cout << "Such way not found, adding new point with number " << bohr.size() << std::endl;
				bohr.push_back(createPoint(index, ch));
				bohr[index].directLinks[ch] = bohr.size() - 1;
			}
			index = bohr[index].directLinks[ch];
			std::cout << "Switching to point - " << index << "\n";
		}
		bohr[index].terminal = true;
		for (int i = 0; i < 40; i++)
		{
			if (bohr[index].patternNum[i] == -1)
			{
				bohr[index].patternNum[i] = patterns.size() - 1; // запоминаем номер паттерна
				break;
			}
		}
	}

	int getSuffixLink(int point) // функция для получения суффиксной ссылки
	{
		if (bohr[point].suffixLink == -1) // если суффиксная ссылка не существует
			if (point == 0 || bohr[point].parentIndex == 0) // если текущая вершина - корень или если родительская вершина - корень
				bohr[point].suffixLink = 0; // возвращаем 0
			else // иначе ищем ссылку на нижнем уровне
				bohr[point].suffixLink = getLink(getSuffixLink(bohr[point].parentIndex), bohr[point].charr);
		if (bohr.at(point).suffixLink)
			std::cout << "Suffix link to point " << bohr.at(point).suffixLink << "!\n";
		else
			std::cout << "Suffix link not found\n";
		return bohr.at(point).suffixLink;
	}

	int getLink(int point, char charr) // функция для получения обычной ссылки
	{
		if (bohr[point].charrLink[charr] == -1) // если ссылки нет
			if (bohr[point].directLinks[charr] != -1) // проверяем есть ли ссылка в боре
				bohr[point].charrLink[charr] = bohr[point].directLinks[charr];
			else // пробуем найти суффиксную ссылку
			{
				if (point == 0)
					bohr[point].charrLink[charr] = 0;
				else
					bohr[point].charrLink[charr] = getLink(getSuffixLink(point), charr);
			}

		std::cout << "link to point " << bohr.at(point).charrLink[charr] << " by char " << alphabet[charr] << std::endl;
		return bohr.at(point).charrLink[charr];
	}

	int getCompressedLink(int point) // функция для получения конечной ссылки
	{
		if (bohr[point].compressedLink == -1) // если конечной ссылки нет
		{
			int curr = getSuffixLink(point);
			if (curr == 0)
				bohr[point].compressedLink = 0;
			else
			{
				if (bohr[curr].terminal) // если суффиксная ссылка указывает на конечную вершину
					bohr[point].compressedLink = curr; // возвращаем конечную ссылку на конечную вершину
				else // иначе пробуем запустить функцию для найденной вершины
					bohr[point].compressedLink = getCompressedLink(curr);
			}
		}
		if (bohr.at(point).compressedLink)
			std::cout << "Compressed link to point " << bohr.at(point).compressedLink << "!\n";
		else
			std::cout << "Compressed link not found" << std::endl;
		return bohr.at(point).compressedLink;
	}

	void find(int v, int i) // функция для проверки терминальности текущей вершины и запуска поиска конечных ссылок
	{
		struct Numbers s;
		for (int u(v); u != 0; u = getCompressedLink(u))
		{
			if (bohr[u].terminal) // если найдена конечная вершина
			{
				std::cout << "Terminal point was found!" << std::endl;
				for (int j(0); j < 40; j++)
				{
					if (bohr[u].patternNum[j] != -1)
					{
						s.index = i - patterns[bohr[u].patternNum[j]].size(); // запоминаем индекс в строке
						s.patternNum = bohr[u].patternNum[j]; // запоминаем номер паттерна
						num.push_back(s);
						std::cout << "\n-----\nPattern was found in index " << s.index << ". Pattern number - " << s.patternNum << "\n-----\n\n";
					}
					else
						break;
				}
			}
		}
	}

	void AHO() // запуск алгоритма Ахо-Корасик
	{
		std::cout << "\n-----\nSearching patterns in text \n\n";
		int u = 0;
		for (int i(0); i < text.size(); i++) // перебор всех символов текста
		{
			std::cout << "-----\nNew char: " << text.at(i) << std::endl << std::endl;
			u = getLink(u, alphabet.find(text[i]));
			find(u, i + 1);
		}
	}

	void print() // вывод результатов
	{
		std::cout << "\n\n-----\nAnalysing found patterns\n";
		std::vector <int> c(text.size(), 0);
		for (int i(0); i < num.size(); i++)
		{
			std::cout << "Current pattern - " << patterns[num[i].patternNum] << std::endl;
			if (num[i].index < patternPos[num[i].patternNum] - 1)
				continue;
			c[num[i].index - patternPos[num[i].patternNum] + 1]++;
			std::cout << "On index " << num[i].index - patternPos[num[i].patternNum] + 2 << " - coincidence " << c[num[i].index - patternPos[num[i].patternNum] + 1] << " out of " << patterns.size() << std::endl;
			if (c[num[i].index - patternPos[num[i].patternNum] + 1] == patterns.size() &&
				num[i].index - patternPos[num[i].patternNum] + 1 <= text.size() - patternStr.size())
				std::cout << "Whole patter was found on " << num[i].index - patternPos[num[i].patternNum] + 2 << std::endl;
		}

		std::cout << "\n-----\nVariant 2 -- Indivualization" << std::endl;
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
					std::cout << "Adjacent patterns: pattern " << num.at(b).patternNum << " and " << num.at(a).patternNum;
					std::cout << ", index " << num.at(b).index << " and " << num.at(a).index << std::endl;
				}
			}
			num.erase(num.begin());

		}
		std::cout << "Points number: " << bohr.size() << std::endl;
		if (flag) std::cout << "No adjusting patterns!\n";
		printBohr(); // вывод автомата

	}

	void printBohr()
	{
		std::cout << "\n-----\nMachine built during the operation of the algorithm\n\n";
		for (int i = 0; i < bohr.size(); i++)
		{
			std::cout << "Point " << i << " with paths: \n";
			for (int j = 0; j < 5; j++)
				if (bohr.at(i).charrLink[j] != -1)
					std::cout << "\tPoint " << bohr.at(i).charrLink[j] << " with paht " << alphabet[j] << std::endl;
		}
	}
};

int main()
{

	Bohr obj;
	obj.init();
	obj.AHO();
	obj.print();
	return 0;
}/*ACGNCGTACGT ACG%CGT %*/