#include <iostream>
#include <vector>
#include <string>

using namespace std;



class Bohr
{
private:
	string alphabet = "ACGTN";
	struct BohrPoint
	{
		int directLinks[5]; // переходы в боре
		bool terminal; // является ли вершина конечной
		int num; // номер паттерна
		int parentIndex; // индекс вершины родителя
		int suffixLink; // суффиксная ссылка
		int charrLinks[5]; // переходы в автомате
		int charFromParent; // символ перехода от родителя
		int compressedLink; // переход по конечной ссылке
	};

	struct Adjacent
	{
		int size;  // размер паттерна
		int index; // индекс в тексте
		int num; // номер паттерна
	};

	vector <Adjacent> pats; // вектор для хранения найденных паттернов
	string text; // строка поиска
	vector<BohrPoint> bohr;  // бор
public:
	void init() // создание бора
	{
		cout << "Creating bohr\n";
		bohr.push_back({ {-1, -1, -1, -1, -1}, false, 0, 0, -1, {-1, -1, -1, -1, -1}, -1, -1 }); // добавляем корневую вершину
		cin >> text;
		int c;
		cin >> c;
		for (int i = 0; i < c; i++)
		{
			string curr;
			cin >> curr;
			cout << "Add symbols of new pattern in prefix tree\n";
			pushPoint(curr, i + 1); // добавляем паттерн в бор
		}
	}

	void pushPoint(string str, int number) // функция для добавления паттерна
	{
		int index = 0;
		int charr;
		for (int i(0); i < str.size(); i++)
		{
			switch (str.at(i)) // определяем символ перехода
			{
			case 'A':
				charr = 0;
				cout << "\nReceived character A" << endl;
				break;
			case 'C':
				charr = 1;
				cout << "\nReceived character C" << endl;
				break;
			case 'G':
				charr = 2;
				cout << "\nReceived character G" << endl;
				break;
			case 'T':
				charr = 3;
				cout << "\nReceived character T" << endl;
				break;
			case 'N':
				charr = 4;
				cout << "\nReceived character N" << endl;
				break;
			}
			if (bohr[index].directLinks[charr] == -1) // если такого состояния в боре нет, то добавляем новое
			{
				cout << "Such way not found, adding new point with number " << bohr.size() << endl;
				bool isEnd = false;
				if (i == str.size() - 1)
					isEnd = true;
				bohr.push_back({ {-1, -1, -1, -1, -1}, isEnd, number, index, -1, {-1, -1, -1, -1, -1}, charr, -1 });
				bohr[index].directLinks[charr] = bohr.size() - 1;
			}
			index = bohr[index].directLinks[charr];
			cout << "Switching to point - " << index << "\n";
			if (i == str.size() - 1) // проверка терминальности вершины
			{
				bohr[index].terminal = true;
				bohr[index].num = number;
			}

		}
	}

	int getSuffixLink(int point) // функция для получения суффиксной ссылки
	{
		if (bohr.at(point).suffixLink == -1) // если суффиксная ссылка не существует
		{
			if (point == 0 || bohr.at(point).parentIndex == 0) // если текущая вершина - корень или если родительская вершина - корень
				bohr.at(point).suffixLink = 0; // возвращаем 0
			else // иначе ищем ссылку на нижнем уровне
				bohr.at(point).suffixLink = getLink(getSuffixLink(bohr.at(point).parentIndex), bohr.at(point).charFromParent);
		}
		if (bohr.at(point).suffixLink)
			cout << "Suffix link to point " << bohr.at(point).suffixLink << "!\n";
		else
			cout << "Suffix link not found\n";
		return bohr.at(point).suffixLink;
	}

	int getLink(int point, int charr) // функция для получения обычной ссылки
	{
		if (bohr.at(point).charrLinks[charr] == -1) // если ссылки нет
		{
			if (bohr.at(point).directLinks[charr] != -1) // проверяем есть ли ссылка в боре
				bohr.at(point).charrLinks[charr] = bohr.at(point).directLinks[charr];
			else // пробуем найти суффиксную ссылку
			{
				if (point == 0)
					bohr.at(point).charrLinks[charr] = 0;
				else
					bohr.at(point).charrLinks[charr] = getLink(getSuffixLink(point), charr);
			}
		}
		cout << "link to point " << bohr.at(point).charrLinks[charr] << " by char " << alphabet[charr] << endl;
		return bohr.at(point).charrLinks[charr];
	}

	int getCompressedLink(int point) // // функция для получения конечной ссылки
	{
		if (bohr.at(point).compressedLink == -1) // если конечной ссылки нет
		{
			int curr = getSuffixLink(point); // получаем суффиксную ссылку
			if (curr == 0)
				bohr.at(point).compressedLink = 0;
			else
			{
				if (bohr.at(curr).terminal)// если суффиксная ссылка указывает на конечную вершину
					bohr.at(point).compressedLink = curr; // возвращаем конечную ссылку на конечную вершину
				else // иначе пробуем запустить функцию для найденной вершины
					bohr.at(point).compressedLink = getCompressedLink(curr);
			}
		}
		if (bohr.at(point).compressedLink)
			cout << "Compressed link to point " << bohr.at(point).compressedLink << "!\n";
		else
			cout << "Compressed link not found" << endl;
		return bohr.at(point).compressedLink;
	}

	void find(int v, int i) // функция для проверки терминальности текущей вершины и запуска поиска конечных ссылок
	{
		int j = 0;
		for (int u(v); u != 0; u = getCompressedLink(u), j++)
		{
			if (bohr.at(u).terminal) // если найдена конечная вершина
			{
				cout << "Terminal point was found!" << endl;
				int delta = 0;
				int curr = u;
				while (bohr.at(curr).parentIndex != 0)
				{
					curr = bohr.at(curr).parentIndex;
					delta++; // получаем размер паттерна, с помощью возврата до корня
				}
				cout << "\n-----\nPattern was found in index " << i - delta << ". Patter number - " << bohr.at(u).num << "\n-----\n\n";
				pats.push_back({ delta, i - delta, bohr.at(u).num });
			}
		}

	}

	void AHO_COR() // запуск алгоритма Ахо-Корасик
	{
		cout << "\n-----\nSearching patterns in text \n\n";
		int point = 0, charr = 0;
		for (int i(0); i < text.length(); i++) // перебор всех символов текста
		{
			cout << "-----\nNew char: " << text.at(i) << endl << endl;
			switch (text.at(i))
			{
			case 'A':
				charr = 0;
				break;
			case 'C':
				charr = 1;
				break;
			case 'G':
				charr = 2;
				break;
			case 'T':
				charr = 3;
				break;
			case 'N':
				charr = 4;
				break;
			}
			point = getLink(point, charr);
			cout << "Current point: " << point << endl;
			find(point, i + 1);
		}

		cout << "\n-----\nVariant 2 -- Indivualization" << endl;
		bool flag = true;
		for (int i = 0; i < pats.size(); i++)
		{
			int a, b;
			for (int j = 0; j < pats.size(); j++)
			{
				if (pats.at(j).index > pats.at(i).index)
				{
					a = j;
					b = i;
				}
				else
				{
					a = i;
					b = j;
				}
				if (i != j && pats.at(a).index < pats.at(b).index + pats.at(b).size)
				{
					flag = false;
					cout << "Adjacent patterns: pattern " << pats.at(b).num << " and " << pats.at(a).num;
					cout << ", index " << pats.at(b).index << " and " << pats.at(a).index << endl;
				}
			}
			pats.erase(pats.begin());

		}
		cout << "Points number: " << bohr.size() << endl;
		if (flag) cout << "No adjusting patterns!\n";
		printBohr(); // вывод автомата
	}

	void printBohr()
	{
		cout << "\n-----\nMachine built during the operation of the algorithm\n\n";
		for (int i = 0; i < bohr.size(); i++)
		{
			cout << "Point " << i << " with paths: \n";
			for (int j = 0; j < 5; j++)
				if (bohr.at(i).charrLinks[j] != -1)
					cout << "\tPoint " << bohr.at(i).charrLinks[j] << " with paht " << alphabet[j] << endl;
		}
	}
};

int main()
{
	Bohr bor;
	bor.init();
	bor.AHO_COR();
	return 0;
}
/*ACGTNNNACGT 3 ACG CGT NNN*/
/*ACGTNNN 3 ACG ACGT AC*/