#pragma once
#include <string>
#include <cstdint>
#include "enums.hpp"

class Character {
    std::string name;
    int32_t savedAddress; 
    int8_t charClass;
    int32_t stats[6];
    int64_t gold;
    int64_t exp;


public:
    /*
    int64_t seekAddress(size_t size, int8_t* disk);
    bool isCharacterValid(int8_t* disk);
    
    void setName(std::string toSet);
    
    void deserializeClass(int8_t* disk);
    void serializeClass(int8_t* disk);
    uint8_t setClass(uint8_t toSet);
    
    void deserializeStats(int8_t* disk);
    void serializeStats(int8_t* disk);
    uint8_t setStr(uint32_t str);
    uint8_t setIq(uint32_t iq);
    uint8_t setPiet(uint32_t piety);
    uint8_t setVit(uint32_t vitality);
    uint8_t setAg(uint32_t agility);
    uint8_t setLuck(uint32_t luck);
    uint8_t setStats(uint32_t* buf);
    
    void deserializeExp(int8_t* disk);
    void deserializeGold(int8_t* disk);
    void serializeExp(int8_t* disk);
    void serializeGold(int8_t* disk);
    uint8_t setExp(uint64_t toSet);
    uint8_t setGold(uint64_t toSet);
    
    std::string printName();
    std::string printClass();
    std::string printStats();
    std::string printExp();
    std::string printGold();
    std::string printCharacter();
    */
    

    int64_t seekAddress(size_t size, int8_t* disk)
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

        if(!found || !isCharacterValid(disk))
            savedAddress = -1;
		
		return savedAddress;
    }
    
    bool isCharacterValid(int8_t* disk)
    {
		deserializeClass(disk);
		deserializeStats(disk);
		
		if(charClass > (int)MinMaxValues::maxClass) //ci sono 8 classi da 0 a 7
			return false;
		
		for(int i = 0; i < 6; i++)
			if(stats[i] > (int)MinMaxValues::maxStats || stats[i] < (int)MinMaxValues::minStats) //le statistiche possono raggiungere solo 18
				return false;
		
		return true;
	}

//nome
    void setName(std::string toSet)
    {
        name = toSet;
    }

//classe
    void deserializeClass(int8_t* disk)
    {
        charClass = disk[savedAddress + (int32_t)Offset::profession];
    }

    void serializeClass(int8_t* disk)
    {
        disk[savedAddress + (int32_t)Offset::profession] = charClass;
    }
    
    uint8_t setClass(uint8_t toSet)
    {
		if(toSet > (int)MinMaxValues::maxClass)
			return -1;
		
		charClass = toSet;
		
		return 0;
	}

//statistiche
    void deserializeStats(int8_t* disk)
    {
        int32_t buf = disk[savedAddress + (int32_t)Offset::statistics + 0] << 24 |
                  disk[savedAddress + (int32_t)Offset::statistics + 1] << 16 |
                  disk[savedAddress + (int32_t)Offset::statistics + 2] << 8 |
                  disk[savedAddress + (int32_t)Offset::statistics + 3];
        
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

        stats[(int)Statistics::strength]    = (buf & masks[0]) >> 24;
        stats[(int)Statistics::iq]          = (buf & masks[4]) >> 21 | (buf & masks[5]) >> 13;
        stats[(int)Statistics::piety]       = (buf & masks[1]) >> 18;
        stats[(int)Statistics::vitality]    = (buf & masks[2]) >> 8;
        stats[(int)Statistics::agility]     = (buf & masks[6]) >> 13 | (buf & masks[7]) << 3;
        stats[(int)Statistics::luck]        = (buf & masks[3]) >> 2;
    }
    
    void serializeStats(int8_t* disk)
    {
		int32_t buf =      stats[(int)Statistics::strength]                 << 24 |
				          (stats[(int)Statistics::iq]       & 0b00000111)   << 21
                        | (stats[(int)Statistics::iq]       & 0b00011000)   << 13 |
				           stats[(int)Statistics::piety]                    << 18 |
				           stats[(int)Statistics::vitality]                 << 8  |
				          (stats[(int)Statistics::agility]  & 0b00000111)   << 13
                        | (stats[(int)Statistics::agility]  & 0b00011000)   >> 3 |
				           stats[(int)Statistics::luck]                     << 2;
		
		disk[savedAddress + (int32_t)Offset::statistics + 0] = (buf & 0xFF'00'00'00) >> 24;
		disk[savedAddress + (int32_t)Offset::statistics + 1] = (buf & 0x00'FF'00'00) >> 16;
		disk[savedAddress + (int32_t)Offset::statistics + 2] = (buf & 0x00'00'FF'00) >> 8;
		disk[savedAddress + (int32_t)Offset::statistics + 3] = (buf & 0x00'00'00'FF);
	}

	uint8_t setStr(uint32_t str) {
		if(str > (int)MinMaxValues::maxStats)
			return -1;
		
		stats[(int)Statistics::strength] = str;
		
		return 0;
	}
	
	uint8_t setIq(uint32_t iq) {
		if(iq > (int)MinMaxValues::maxStats)
			return -1;
			
		stats[(int)Statistics::iq] = iq;
		
		return 0;
	}
	
	uint8_t setPiet(uint32_t piety) {
		if(piety > (int)MinMaxValues::maxStats)
			return -1;
			
		stats[(int)Statistics::piety] = piety;
		
		return 0;
	}
	
	uint8_t setVit(uint32_t vitality) {
		if(vitality > (int)MinMaxValues::maxStats)
			return -1;
			
		stats[(int)Statistics::vitality] = vitality;
		
		return 0;
	}
	
	uint8_t setAg(uint32_t agility) {
		if(agility > (int)MinMaxValues::maxStats)
			return -1;
			
		stats[(int)Statistics::agility] = agility;
		
		return 0;
	}
	
	uint8_t setLuck(uint32_t luck) {
		if(luck > (int)MinMaxValues::maxStats)
			return -1;
			
		stats[(int)Statistics::luck] = luck;
		
		return 0;
	}
	
	uint8_t setStats(uint32_t* buf) {
		if(setStr(buf[0]) 	== -1 ||
		   setIq(buf[1])  	== -1 ||
		   setPiet(buf[2])  == -1 ||
		   setVit(buf[3])   == -1 ||
		   setAg(buf[4])    == -1 ||
		   setLuck(buf[5])  == -1)
		{
		   return -1;
		}

        return 0;
	}

//oro ed esperienza
    void deserializeExp(int8_t* disk)
    {
        int64_t nibbleArray[10], buf;
        static const int64_t mask = 0xF; //deve poter contenere 5 byte vedi sotto

        //la dimensione delle singole celle dell'array causa un problema con questa quantità di shifting
        buf =   disk[savedAddress + (int32_t)Offset::experience];
        buf =   buf << 32 |
                disk[savedAddress + (int32_t)Offset::experience + 1] << 24|
                disk[savedAddress + (int32_t)Offset::experience + 2] << 16|
                disk[savedAddress + (int32_t)Offset::experience + 3] << 8 |
                disk[savedAddress + (int32_t)Offset::experience + 4];
        
        for(int32_t i = 0; i < 10; i++) {
            nibbleArray[9 - i] = (buf & (mask << (4 * i))) >> (4 * i);
        }

        //purtroppo non c'è una logica secondo cui moltiplicare
        exp =    nibbleArray[0] * 16         +
                 nibbleArray[1]              +
                 nibbleArray[2] * 4096       + 
                 nibbleArray[3] * 256        + 
                 nibbleArray[4] * 160000     +
                 nibbleArray[5] * 10000      +
                 nibbleArray[6] * 40960000   +
                 nibbleArray[7] * 2560000    + 
                 nibbleArray[8] * 1600000000 +
                 nibbleArray[9] * 100000000;
    }

    void deserializeGold(int8_t* disk)
    {
        int64_t nibbleArray[10], buf;
        static const int64_t mask = 0xF;

        buf =   disk[savedAddress + (int32_t)Offset::money];
        buf =   buf << 32 |
                disk[savedAddress + (int32_t)Offset::money + 1] << 24|
                disk[savedAddress + (int32_t)Offset::money + 2] << 16|
                disk[savedAddress + (int32_t)Offset::money + 3] << 8 |
                disk[savedAddress + (int32_t)Offset::money + 4];
        
        for(int32_t i = 0; i < 10; i++) {
            nibbleArray[9 - i] = (buf & (mask << (4 * i))) >> (4 * i);
        }

        gold =  nibbleArray[0] * 16         +
				nibbleArray[1]              +
                nibbleArray[2] * 4096       + 
                nibbleArray[3] * 256        + 
                nibbleArray[4] * 160000     +
                nibbleArray[5] * 10000      +
                nibbleArray[6] * 40960000   +
                nibbleArray[7] * 2560000    + 
                nibbleArray[8] * 1600000000 +
                nibbleArray[9] * 100000000;
    }

    void serializeExp(int8_t* disk)
    {
        int64_t nibbleArray[10], buf = exp, mask = 0xF;

        nibbleArray[8] = buf / 1600000000;
        buf -= nibbleArray[8] * 1600000000;
        nibbleArray[9] = buf / 100000000;
        buf -= nibbleArray[9] * 100000000;
        nibbleArray[6] = buf / 40960000;
        buf -= nibbleArray[6] * 40960000;
        nibbleArray[7] = buf / 2560000;
        buf -= nibbleArray[7] * 2560000;
        nibbleArray[4] = buf / 160000;
        buf -= nibbleArray[4] * 160000;
        nibbleArray[5] = buf / 10000;
        buf -= nibbleArray[5] * 10000;
        nibbleArray[2] = buf / 4096;
        buf -= nibbleArray[2] * 4096;
        nibbleArray[3] = buf / 256;
        buf -= nibbleArray[3] * 256;
        nibbleArray[0] = buf / 16;
        buf -= nibbleArray[0] * 16;
        nibbleArray[1] = buf;

        for (int i = 0; i < 10; i++)
            buf |= (nibbleArray[9 - i] & mask) << (4 * i);
        
        disk[savedAddress + (int)Offset::experience] = buf >> 32;
        disk[savedAddress + (int)Offset::experience + 1] = buf >> 24;
        disk[savedAddress + (int)Offset::experience + 2] = buf >> 16;
        disk[savedAddress + (int)Offset::experience + 3] = buf >> 8;
        disk[savedAddress + (int)Offset::experience + 4] = buf;
    }
    void serializeGold(int8_t* disk)
    {
        int64_t nibbleArray[10], buf = gold, mask = 0xF;

        nibbleArray[8] = buf / 1600000000;
        buf -= nibbleArray[8] * 1600000000;
        nibbleArray[9] = buf / 100000000;
        buf -= nibbleArray[9] * 100000000;
        nibbleArray[6] = buf / 40960000;
        buf -= nibbleArray[6] * 40960000;
        nibbleArray[7] = buf / 2560000;
        buf -= nibbleArray[7] * 2560000;
        nibbleArray[4] = buf / 160000;
        buf -= nibbleArray[4] * 160000;
        nibbleArray[5] = buf / 10000;
        buf -= nibbleArray[5] * 10000;
        nibbleArray[2] = buf / 4096;
        buf -= nibbleArray[2] * 4096;
        nibbleArray[3] = buf / 256;
        buf -= nibbleArray[3] * 256;
        nibbleArray[0] = buf / 16;
        buf -= nibbleArray[0] * 16;
        nibbleArray[1] = buf;

        for (int i = 0; i < 10; i++)
            buf |= (nibbleArray[9 - i] & mask) << (4 * i);
        
        disk[savedAddress + (int)Offset::money] = buf >> 32;
        disk[savedAddress + (int)Offset::money + 1] = buf >> 24;
        disk[savedAddress + (int)Offset::money + 2] = buf >> 16;
        disk[savedAddress + (int)Offset::money + 3] = buf >> 8;
        disk[savedAddress + (int)Offset::money + 4] = buf;
    }
    
    uint8_t setExp(uint64_t toSet)
    {
		if(toSet >= (int)MinMaxValues::maxMoneyExperience) //numero massimo di exp ottenibile
			return -1;
			
		exp = toSet;
		
		return 0;
	}
	
	uint8_t setGold(uint64_t toSet)
	{
		if(toSet >= (int)MinMaxValues::maxMoneyExperience)
			return -1;
			
		gold = toSet;
		
		return 0;
	}

//sezione di stampa
    std::string printName()
    {
        return name;
    }

    std::string printClass()
    {
        switch(charClass)
        {
            case (int)Professions::fighter:  return "fighter";
            case (int)Professions::mage:     return "mage";
            case (int)Professions::priest:   return "priest";
            case (int)Professions::thief:    return "thief";
            case (int)Professions::bishop:   return "bishop";
            case (int)Professions::samurai:  return "samurai";
            case (int)Professions::lord:     return "lord";
            case (int)Professions::ninja:    return "ninja";

            default:return "error";
        }
    }

    std::string printStats()
    {
        using namespace std;

        return  "strength: " + to_string(stats[(int)Statistics::strength]) + "\n"
            +   "iq: "       + to_string(stats[(int)Statistics::iq]) + "\n"
            +   "piety: "    + to_string(stats[(int)Statistics::piety]) + "\n"
            +   "vitality: " + to_string(stats[(int)Statistics::vitality]) + "\n"
            +   "agility: "  + to_string(stats[(int)Statistics::agility]) + "\n"
            +   "luck: "     + to_string(stats[(int)Statistics::luck]);
    }

    std::string printExp()
    {
        using namespace std;
        
        return to_string(exp);
    }
    
    std::string printGold()
    {
		using namespace std;
		
		return to_string(gold);
	}

    std::string printCharacter()
    {
        return printName()  + "\n" +
               printClass() + "\n" +
               printStats() + "\n" +
               printExp()	+ "\n" +
               printGold();
    }
};
