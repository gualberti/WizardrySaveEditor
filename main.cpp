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

char* disk;

class Characters {
    std::string name;
    int savedAddress; 
    char charClass;                     //(1 byte) (0 = fighter, 1 = mage, 2 = priest, 3 = thief, 4 = bishop, 5 = samurai, 6 = lord, 7 = ninja)
    int* stats = new int[6];            //(4 byte) (9 e 25 = 0, str = 4-8, iq = 15-16 + 1-3, piety = 10-14, vitality = 20-24, agility = 31-32 + 17-19, luck = 26-30)
    char* equip = new char[9];          //(2 + 8*8 byte ma 7 inutilizzati per pezzo di equip) (quantità di cose, 0, 8* (hex) 000000000100xx00 dove xx è l'id del pezzo di equipaggiamento)
    long gold;                          //(5 byte) (byte1: 1-4 *16, 5-8 *1, byte2: 1-4 *4096, 5-8 *256, byte3: 1-4 *160000, 5-8 *10000, byte4: 1-4 *40960000, 5-8 *2560000, byte5: 1-4 *1600000000, 5-8 *100000000)
    long exp;                           //vedi sopra

/* TODO:
    <>
    tutta la cli:
        ottieni nome personaggio
        menù per decidere statistiche
        menù per decidere equipaggiamento (serve lista)
        menù per gold ed esperienza (uguali)

    getter e setter di:
        equip   gs
        gold    gs
        exp     gs

    rigoroso testing
*/

public:
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

//classe
    void deserializeClass()
    {
        this->charClass = disk[this->savedAddress + CLASSOFFSET];
    }

    void serializeClass(char toSet)
    {
        disk[this->savedAddress + CLASSOFFSET] = toSet;
    }

//statistiche
    void deserializeStats()
    {
        int buf = disk[this->savedAddress + STATSOFFSET + 0] << 24 |
                  disk[this->savedAddress + STATSOFFSET + 1] << 16 |
                  disk[this->savedAddress + STATSOFFSET + 2] << 8 |
                  disk[this->savedAddress + STATSOFFSET + 3];
        
        static const int masks[] = {
			(int) 0b00011111'00000000'00000000'00000000,  //strength
			(int) 0b00000000'01111100'00000000'00000000,  //piety
			(int) 0b00000000'00000000'00011111'00000000,  //vitality
			(int) 0b00000000'00000000'00000000'01111100,  //luck
			(int) 0b11100000'00000000'00000000'00000000,  //iq pt1
			(int) 0b00000000'00000011'00000000'00000000,  //iq pt2
			(int) 0b00000000'00000000'11100000'00000000,  //agility pt1
			(int) 0b00000000'00000000'00000000'00000011   //agility pt2
		};

        stats[0] = (buf & masks[0]) >> 24;
        stats[1] = (buf & masks[4]) >> 21 | (buf & masks[5]) >> 13;
        stats[2] = (buf & masks[1]) >> 18;
        stats[3] = (buf & masks[2]) >> 8;
        stats[4] = (buf & masks[6]) >> 13 | (buf & masks[7]) << 3;
        stats[5] = (buf & masks[3]) >> 2;
    }
    
    void serializeStats()
    {
		int buf =  stats[0] << 24 |
				  (stats[1] & 0b00000111) << 21 | (stats[1] & 0b00011000) << 13 |
				   stats[2] << 18 |
				   stats[3] << 8  |
				  (stats[4] & 0b00000111) << 13 | (stats[4] & 0b00011000) >> 3 |
				   stats[5] << 2;
		
		disk[this->savedAddress + STATSOFFSET + 0] = (buf & 0xFF'00'00'00) >> 24;
		disk[this->savedAddress + STATSOFFSET + 1] = (buf & 0x00'FF'00'00) >> 16;
		disk[this->savedAddress + STATSOFFSET + 2] = (buf & 0x00'00'FF'00) >> 8;
		disk[this->savedAddress + STATSOFFSET + 3] = (buf & 0x00'00'00'FF);
	}

	void setStr(int str) {
		stats[0] = str;
	}
	
	void setIq(int iq) {
		stats[1] = iq;
	}
	
	void setPiet(int piety) {
		stats[2] = piety;
	}
	
	void setVit(int vitality) {
		stats[3] = vitality;
	}
	
	void setAg(int agility) {
		stats[4] = agility;
	}
	
	void setLuck(int luck) {
		stats[5] = luck;
	}
	
	void setStats(int* buf) {
		setStr(buf[0]);
		setIq(buf[1]);
		setPiet(buf[2]);
		setVit(buf[3]);
		setAg(buf[4]);
		setLuck(buf[5]);
	}

//oro ed esperienza
    void deserializeExpOrGold(char which)
    {
        long nibbleArray[10], buf, resultTemp;
        static const long mask = 0xF; //deve poter contenere 5 byte

        if(which == 'E'){
            buf = disk[this->savedAddress + EXPOFFSET]; //shift causa perdita di dati

            buf =  buf << 32 |
                    disk[this->savedAddress + EXPOFFSET + 1] << 24|
                    disk[this->savedAddress + EXPOFFSET + 2] << 16|
                    disk[this->savedAddress + EXPOFFSET + 3] << 8 |
                    disk[this->savedAddress + EXPOFFSET + 4];
        }
        
        if(which == 'G') {
            buf = disk[this->savedAddress + GOLDOFFSET];

            buf =  buf << 32 |
                    disk[this->savedAddress + GOLDOFFSET + 1] << 24|
                    disk[this->savedAddress + GOLDOFFSET + 2] << 16|
                    disk[this->savedAddress + GOLDOFFSET + 3] << 8 |
                    disk[this->savedAddress + GOLDOFFSET + 4];
        }
        
        for(int i = 0; i < 10; i++) {
            nibbleArray[9 - i] = (buf & (mask << (4 * i))) >> (4 * i);
        }

        //purtroppo non c'è una logica secondo cui moltiplicare
        resultTemp =    nibbleArray[0] * 16         +
                        nibbleArray[1]              +
                        nibbleArray[2] * 4096       + 
                        nibbleArray[3] * 256        + 
                        nibbleArray[4] * 160000     +
                        nibbleArray[5] * 10000      +
                        nibbleArray[6] * 40960000   +
                        nibbleArray[7] * 2560000    + 
                        nibbleArray[8] * 1600000000 +
                        nibbleArray[9] * 100000000;

        if(which == 'E')
            exp = resultTemp;
        
        if(which == 'G')
            gold == resultTemp;
    }

    void serializeExpOrGold()
    {

    }

//sezione di stampa
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
             << "iq: "       << stats[1] << endl
             << "piety: "    << stats[2] << endl
             << "vitality: " << stats[3] << endl
             << "agility: "  << stats[4] << endl
             << "luck: "     << stats[5] << endl;
    }

    void printExpOrGold(char which)
    {
        using namespace std;

        if(which == 'E')
            cout << "exp. points: " << exp << endl;
        
        if(which == 'G')
            cout << "gold: " << gold << endl;
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

    printf("%d\n", sizeof(long));

    list[0].deserializeClass();
    list[0].printClass();

    list[0].deserializeStats();
    list[0].printStats();

    list[0].deserializeExpOrGold('E');
    list[0].printExpOrGold('E');

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
