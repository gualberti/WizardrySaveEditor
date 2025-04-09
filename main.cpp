#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

#define CLASSOFFSET 0x24
#define STATSOFFSET 0x2C
#define GOLDOFFSET  0x34
#define EQUIPOFFSET 0x3A
#define EXPOFFSET   0x7C

int getNumberFromUser();
void printCharAsBinary(char c);


char* disk;

class Characters {
    std::string name;
    int savedAddress; 
    char charClass;                     //(1 byte) (0 = fighter, 1 = mage, 2 = priest, 3 = thief, 4 = bishop, 5 = samurai, 6 = lord, 7 = ninja)
    int* stats = new int[6];            //(4 byte) (9 e 25 = 0, str = 4-8, iq = 15-16 + 1-3, piety = 10-14, vitality = 20-24, agility = 31-32 + 17-19, luck = 26-30)
    char* equip = new char[9];          //(2 + 8*8 byte ma 7 inutilizzati per pezzo di equip) (quantità di cose, 0, 8* (hex) 000000000100xx00 dove xx è l'id del pezzo di equipaggiamento)
    char* gold = new char[5];           //(5 byte) (byte1: 1-4 *16, 5-8 *1, byte2: 1-4 *4096, 5-8 *256, byte3: 1-4 *160000, 5-8 *10000, byte4: 1-4 *40960000, 5-8 *2560000, byte5: 1-4 *1600000000, 5-8 *100000000)
    char* exp = new char[5];            //vedi sopra

public:
/* TODO:
    <>
    tutta la cli

    getter e setter di:
        statistiche gs
        equip   gs
        gold    gs
        exp     gs

    rigoroso testing
*/

    void seekAddress(size_t size)
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

    void getClass()
    {
        this->charClass = disk[this->savedAddress + CLASSOFFSET];
    }

    void setClass(char toSet)
    {
        disk[this->savedAddress + CLASSOFFSET] = toSet;
    }

    void getStats()
    {
        int buf = disk[this->savedAddress + STATSOFFSET] << 24 |
                  disk[this->savedAddress + STATSOFFSET + 1] << 16 |
                  disk[this->savedAddress + STATSOFFSET + 2] << 8 |
                  disk[this->savedAddress + STATSOFFSET + 3];
        
        int masks[] = {(int) 0b00011111'00000000'00000000'00000000,  //strength
                       (int) 0b00000000'01111100'00000000'00000000,  //piety
                       (int) 0b00000000'00000000'00011111'00000000,  //vitality
                       (int) 0b00000000'00000000'00000000'01111100,  //luck
                       (int) 0b11100000'00000000'00000000'00000000,  //iq pt1
                       (int) 0b00000000'00000011'00000000'00000000,  //iq pt2
                       (int) 0b00000000'00000000'11100000'00000000,  //agility pt1
                       (int) 0b00000000'00000000'00000000'00000011}; //agility pt2

        stats[0] = (buf & masks[0]) >> 24;
        stats[1] = (buf & masks[4]) >> 21 | (buf & masks[5]) >> 13;
        stats[2] = (buf & masks[1]) >> 18;
        stats[3] = (buf & masks[2]) >> 8;
        stats[4] = (buf & masks[6]) >> 13 | (buf & masks[7]) << 3;
        stats[5] = (buf & masks[3]) >> 2;
    }

//interpretatori
    void printClass()
    {
        using namespace std;

        switch(this->charClass)
        {
            case 0: cout << "\n" << "fighter\n";  break;
            case 1: cout << "\n" << "mage   \n";  break;
            case 2: cout << "\n" << "priest \n";  break;
            case 3: cout << "\n" << "thief  \n";  break;
            case 4: cout << "\n" << "bishop \n";  break;
            case 5: cout << "\n" << "samurai\n";  break;
            case 6: cout << "\n" << "lord   \n";  break;
            case 7: cout << "\n" << "ninja  \n";  break;

            default: cerr << "\n error \n"; break;
        }
    }

    void printStats()
    {
        using namespace std;

        cout << "strength: " << stats[0] << endl
             << "iq: " << stats[1] << endl
             << "piety: " << stats[2] << endl
             << "vitality: " << stats[3] << endl
             << "agility: " << stats[4] << endl
             << "luck: " << stats[5] << endl;
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
	
	disk = new char[size];
	
	ifs.read(disk, size);
	ifs.close();

    list[0].setName("SARA");
    list[0].seekAddress(size);
    list[0].getClass();
    list[0].printClass();
    list[0].getStats();
    list[0].printStats();

	delete[] disk;
    delete[] list;
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