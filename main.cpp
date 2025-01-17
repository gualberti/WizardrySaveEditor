#include <iostream>
#include <fstream>
#include <string>

#define CLASSOFFSET 0x24
#define STATSOFFSET 0x2C
#define GOLDOFFSET  0x34
#define EQUIPOFFSET 0x3A
#define EXPOFFSET   0x7C

int getNumberFromUser();
void printCharAsBinary(char c);

class Characters {
    std::string name;
    int savedAddress; 
    char charClass;                     //(1 byte) (0 = fighter, 1 = mage, 2 = priest, 3 = thief, 4 = bishop, 5 = samurai, 6 = lord, 7 = ninja)
    char* stats = new char[4];          //(4 byte) (9 e 25 = 0, str = 4-8, iq = 15-16 + 1-3, piety = 10-14, vitality = 20-24, agility = 31-32 + 17-19, luck = 26-30)
    char* equip = new char[9];          //(2 + 8*8 byte ma 7 inutilizzati per pezzo di equip) (quantità di cose, 0, 8* (hex) 000000000100xx00 dove xx è l'id del pezzo di equipaggiamento)
    char* gold = new char[5];           //(5 byte) (byte1: 1-4 *16, 5-8 *1, byte2: 1-4 *4096, 5-8 *256, byte3: 1-4 *160000, 5-8 *10000, byte4: 1-4 *40960000, 5-8 *2560000, byte5: 1-4 *1600000000, 5-8 *100000000)
    char* exp = new char[5];            //vedi sopra

public:
/* TODO:
    <>
    tutta la cli

    getter e setter di:
        statistiche s
        equip   gs
        gold    gs
        exp     gs

    rigoroso testing
*/

    void seekAddress(char* disk, size_t size)
    {
        size_t i;
        bool found = false;
        for(i = 0; i < (size - name.length()) && !found; i++) {
            found = true;
            for(size_t j = 0; j < name.length(); j++) {
                if (disk[i + j] != name[j]) {
                    found = false;
                    break;
                }
            }
        }

        this->savedAddress = i - 2;
    }

//getter e setter
    void setName(std::string toSet)
    {
        this->name = toSet;
    }

    void getClass(char* disk)
    {
        this->charClass = disk[this->savedAddress + CLASSOFFSET];
    }

    void setClass(char* disk, char toSet)
    {
        disk[this->savedAddress + CLASSOFFSET] = toSet;
    }

    void getStats(char* disk)
    {
        for(int i = 0; i < 4; i++)
            this->stats[i] = disk[this->savedAddress + STATSOFFSET + i];
    }

//interpretatori
    void printClass()
    {
        using namespace std;

        switch(this->charClass)
        {
            case 0: cout << "\n fighter \n"; break;
            case 1: cout << "\n mage \n"; break;
            case 2: cout << "\n priest \n"; break;
            case 3: cout << "\n thief \n"; break;
            case 4: cout << "\n bishop \n"; break;
            case 5: cout << "\n samurai \n"; break;
            case 6: cout << "\n lord \n"; break;
            case 7: cout << "\n ninja \n"; break;

            default: cout << "\n error \n"; break;
        }
    }

    void printStats()
    {
        char* buf = new char[8];
        char str, iq, piet, vit, agi, luc;

        for(int i = 0; i < 4; i++) {
            buf[i] = (this->stats[i] & 0xF0 >> 4);
            buf[i * 2] = (this->stats[i] & 0x0F);
        }

        str = ((buf[1] & 0b0000'0001) << 4) | (buf[2] & 0xF0);
        std::cout << (int)str << std::endl;
        printCharAsBinary(str);
    }
};

int main()
{
    size_t size;
	std::ifstream ifs("SAVE1.DSK");
    Characters* list = new Characters[getNumberFromUser()];
	
	if(!ifs.good())
		return 69;
	
	ifs.seekg(0, std::ios::end);
	size = ifs.tellg();
	ifs.seekg(0, std::ios::beg);
	
	char *disk = new char[size];
	
	ifs.read(disk, size);
	ifs.close();

    list[0].setName("SARA");
    list[0].seekAddress(disk, size);
    list[0].getClass(disk);
    list[0].printClass();
    list[0].getStats(disk);
    list[0].printStats();

	delete[] disk;
	return 0;
}

int getNumberFromUser() {
    int number;
    std::cout << "Inserisci numero di personaggi (maggiore di 0)";
    std::cin >> number;

    if (std::cin.fail() || number <= 0) {
        std::cin.clear(); 
        std::cout << "Input non valido. Inserire un numero valido." << std::endl;
        return getNumberFromUser(); 
    }

    return number;
}

void printCharAsBinary(char c) {
    for (int i = 7; i >= 0; --i) { // Loop from 7 to 0 for 8 bits
        std::cout << ((c >> i) & 1); // Shift and mask to get each bit
    }
    std::cout << std::endl; // New line after printing the binary representation
}