#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdint>
#include "Character.hpp"

using namespace std;

int main()
{
    size_t size;
	ifstream ifs("SAVE1.DSK");
    Character* list = new Character[getNumberFromUser()];
	
	if(!ifs.good())
		return 69;
	
	ifs.seekg(0, ios::end);
	size = ifs.tellg();
	ifs.seekg(0, ios::beg);
	
	disk = new int8_t[size];
	
	ifs.read((char*)disk, size);
	ifs.close();

    list[0].setName("SARA");
    
    list[0].seekAddress(size);

    list[0].deserializeClass();

    list[0].deserializeStats();

    list[0].deserializeExpOrGold('E');
    list[0].deserializeExpOrGold('G');

    cout << list[0].printCharacter();

	delete[] disk;
    delete[] list;
	return 0;
}

int32_t getNumberFromUser() {
    int32_t number;
    cout << "Inserisci numero di personaggi (maggiore di 0): ";
    cin >> number;

    if (cin.fail() || number <= 0) {
        cin.clear(); 
        cout << "Input non valido. Inserire un numero valido." << endl;
        return getNumberFromUser(); 
    }

    return number;
}
