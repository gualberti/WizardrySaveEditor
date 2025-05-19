#include <string>
#include <cstdint>

#define CLASSOFFSET 0x24
#define STATSOFFSET 0x2C
#define GOLDOFFSET  0x34
#define EQUIPOFFSET 0x3A
#define EXPOFFSET   0x7C

int32_t getNumberFromUser();

int8_t* disk;

class Character {
    std::string name;
    int32_t savedAddress; 
    int8_t charClass;                   //(1 byte) (0 = fighter, 1 = mage, 2 = priest, 3 = thief, 4 = bishop, 5 = samurai, 6 = lord, 7 = ninja)
    int32_t* stats = new int32_t[6];    //(4 byte) (9 e 25 = 0, str = 4-8, iq = 15-16 + 1-3, piety = 10-14, vitality = 20-24, agility = 31-32 + 17-19, luck = 26-30)
    int64_t gold;                       //(5 byte) (byte1: 1-4 *16, 5-8 *1, byte2: 1-4 *4096, 5-8 *256, byte3: 1-4 *160000, 5-8 *10000, byte4: 1-4 *40960000, 5-8 *2560000, byte5: 1-4 *1600000000, 5-8 *100000000)
    int64_t exp;                        //vedi sopra

/* TODO:
    <>
    tutta la cli:
        ottieni nome personaggio
        menù per decidere statistiche
        menù per decidere equipaggiamento (serve lista)
        menù per gold ed esperienza (uguali)

    getter e setter di:
        gold    s
        exp     s

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

        savedAddress = i - 2;
    }

//nome
    void setName(std::string toSet)
    {
        name = toSet;
    }

//classe
    void deserializeClass()
    {
        charClass = disk[savedAddress + CLASSOFFSET];
    }

    void serializeClass(int8_t toSet)
    {
        disk[savedAddress + CLASSOFFSET] = toSet;
    }

//statistiche
    void deserializeStats()
    {
        int32_t buf = disk[savedAddress + STATSOFFSET + 0] << 24 |
                  disk[savedAddress + STATSOFFSET + 1] << 16 |
                  disk[savedAddress + STATSOFFSET + 2] << 8 |
                  disk[savedAddress + STATSOFFSET + 3];
        
        static const int32_t masks[] = {
			(int32_t) 0b00011111'00000000'00000000'00000000,  //strength
			(int32_t) 0b00000000'01111100'00000000'00000000,  //piety
			(int32_t) 0b00000000'00000000'00011111'00000000,  //vitality
			(int32_t) 0b00000000'00000000'00000000'01111100,  //luck
			(int32_t) 0b11100000'00000000'00000000'00000000,  //iq pt1
			(int32_t) 0b00000000'00000011'00000000'00000000,  //iq pt2
			(int32_t) 0b00000000'00000000'11100000'00000000,  //agility pt1
			(int32_t) 0b00000000'00000000'00000000'00000011   //agility pt2
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
		int32_t buf =  stats[0] << 24 |
				  (stats[1] & 0b00000111) << 21 | (stats[1] & 0b00011000) << 13 |
				   stats[2] << 18 |
				   stats[3] << 8  |
				  (stats[4] & 0b00000111) << 13 | (stats[4] & 0b00011000) >> 3 |
				   stats[5] << 2;
		
		disk[savedAddress + STATSOFFSET + 0] = (buf & 0xFF'00'00'00) >> 24;
		disk[savedAddress + STATSOFFSET + 1] = (buf & 0x00'FF'00'00) >> 16;
		disk[savedAddress + STATSOFFSET + 2] = (buf & 0x00'00'FF'00) >> 8;
		disk[savedAddress + STATSOFFSET + 3] = (buf & 0x00'00'00'FF);
	}

	void setStr(int32_t str) {
		stats[0] = str;
	}
	
	void setIq(int32_t iq) {
		stats[1] = iq;
	}
	
	void setPiet(int32_t piety) {
		stats[2] = piety;
	}
	
	void setVit(int32_t vitality) {
		stats[3] = vitality;
	}
	
	void setAg(int32_t agility) {
		stats[4] = agility;
	}
	
	void setLuck(int32_t luck) {
		stats[5] = luck;
	}
	
	void setStats(int32_t* buf) {
		setStr(buf[0]);
		setIq(buf[1]);
		setPiet(buf[2]);
		setVit(buf[3]);
		setAg(buf[4]);
		setLuck(buf[5]);
	}

//oro ed esperienza
    void deserializeExpOrGold(int8_t which)
    {
        int64_t nibbleArray[10], buf, resultTemp;
        static const int64_t mask = 0xF; //deve poter contenere 5 byte vedi sotto
        int32_t tempOffset;

        if(which == 'E')
            tempOffset = EXPOFFSET;
        
        if(which == 'G')
            tempOffset = GOLDOFFSET;

        //la dimensione delle singole celle dell'array causa un problema con questa quantità di shifting
        buf =   disk[savedAddress + tempOffset];
        buf =   buf << 32 |
                disk[savedAddress + tempOffset + 1] << 24|
                disk[savedAddress + tempOffset + 2] << 16|
                disk[savedAddress + tempOffset + 3] << 8 |
                disk[savedAddress + tempOffset + 4];
        
        for(int32_t i = 0; i < 10; i++) {
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
            gold = resultTemp;
    }

    void serializeExpOrGold();

//sezione di stampa
    std::string printName()
    {
        return name + "\n";
    }

    std::string printClass()
    {
        switch(charClass)
        {
            case 0: return"fighter\n";
            case 1: return"mage   \n";
            case 2: return"priest \n";
            case 3: return"thief  \n";
            case 4: return"bishop \n";
            case 5: return"samurai\n";
            case 6: return"lord   \n";
            case 7: return"ninja  \n";

            default:return"error  \n";
        }
    }

    std::string printStats()
    {
        using namespace std;

        return  "strength: " + to_string(stats[0]) + "\n"
            +   "iq: "       + to_string(stats[1]) + "\n"
            +   "piety: "    + to_string(stats[2]) + "\n"
            +   "vitality: " + to_string(stats[3]) + "\n"
            +   "agility: "  + to_string(stats[4]) + "\n"
            +   "luck: "     + to_string(stats[5]) + "\n";
    }

    std::string printExpOrGold(int8_t which)
    {
        using namespace std;

        if(which == 'E')
            return "exp. points: " + to_string(exp) + "\n";
        
        if(which == 'G')
            return "gold: " + to_string(gold) + "\n";
    }

    std::string printCharacter()
    {
        return printName()         +
                        printClass()        +
                        printStats()        + 
                        printExpOrGold('E') +
                        printExpOrGold('G');
    }
};