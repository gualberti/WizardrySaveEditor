#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdint>
#include <vector>
#include <limits>
#include "Character.hpp"
#include "enums.hpp"

/* TODO:
    <>
    tutta la cli:
        menù per gold ed esperienza (uguali)
        menù per mostrare tutto il personaggio

    serializzatore di gold ed esperienza
    
    salva su disco

    rigoroso testing
*/

using namespace std;

unsigned int getNumberFromUser(int);
unsigned int getStatFromUser();
int getCharacterForMenu(vector<Character*>);
int writeToDisk(int8_t*, size_t);

int main()
{
    size_t size;
    int8_t* disk;
	ifstream ifs("SAVE1.DSK");
    vector<Character*> list;

    int menuChoice = 1, additionalInput;
    string stringInput;
	Character* objTmp;
	
	if(!ifs.good())
		return 69;
	
	ifs.seekg(0, ios::end);
	size = ifs.tellg();
	ifs.seekg(0, ios::beg);
	
	disk = new int8_t[size];
	
	ifs.read((char*)disk, size);
	ifs.close();

    do {
        cout << "[MENU]" << endl;
        cout << "0. Uscita dal programma" << endl;
        cout << "1. Inserisci nome di nuovo personaggio" << endl;
        cout << "2. Mostra classe" << endl;
        cout << "3. Cambia classe" << endl;
        cout << "4. Mostra statistiche" << endl;
        cout << "5. Cambia singola statistica" << endl;
        cout << "6. Cambia tutte le statistiche" << endl;
        cout << "7. Mostra oro o esperienza" << endl;
        cout << "8. Cambia oro o esperienza" << endl;
        cout << "9. Stampa tutte le informazioni del personaggio" << endl;
        cout << "10. Salvataggio." << endl;
        
        menuChoice = getNumberFromUser(10);
		cout << endl;
		
        switch (menuChoice) {
			case 0:
				cout << "Salvare prima di uscire?" << endl
					 << "0. No" << endl
					 << "1. Sì" << endl;
				menuChoice = getNumberFromUser(1);

				switch(menuChoice) {
					case 0:
						cout << "Hai deciso di non salvare. Chiusura del programma..." << endl;
						break;
					case 1:
						cout << "Salvataggio..." << endl;
						//funzione per scrivere su disco
						break;
				}
				
				menuChoice = 0;
				break;
            case 1:				
                cout << "Inserisci nome: " << endl;
                cin >> stringInput;
                
                for(int i = 0; i < stringInput.length(); i++)
					stringInput[i] = toupper(stringInput[i]); //trasforma il nome in caps
				
				list.emplace_back(new Character()); //aggiunge personaggio a vettore
                
                list.back()->setName(stringInput); //cambia nome ad oggetto
                
                if(list.back()->seekAddress(size, disk) <= 0) { //controlla se il personaggio trovato da seekAddress è valido
					cout << "Personaggio non trovato." << endl << endl;
					list.pop_back();
					break;
				}

                list.back()->deserializeClass(disk);
                list.back()->deserializeStats(disk);
                list.back()->deserializeGold(disk);
                list.back()->deserializeExp(disk);
				
				cout << "Personaggio trovato!" << endl << endl;
                
                break;
            case 2:
                if((menuChoice = getCharacterForMenu(list)) < 0)
                    break;
				
				cout << endl << list.at(menuChoice)->printClass() << endl;

                menuChoice = 1; //per evitare chiusura del programma
				
                break;
            case 3:
                if((menuChoice = getCharacterForMenu(list)) < 0)
                    break;
                
                cout << "0. Fighter" << endl;
                cout << "1. Mage" << endl;
                cout << "2. Priest" << endl;
                cout << "3. Thief" << endl;
                cout << "4. Bishop" << endl;
                cout << "5. Samurai" << endl;
                cout << "6. Lord" << endl;
                cout << "7. Ninja" << endl;

                additionalInput = getNumberFromUser((int)MinMaxValues::maxClass);

                list.at(menuChoice)->setClass(additionalInput);

                cout << "Cambiata la classe di " << list.at(menuChoice)->printName()
                     << " a " << list.at(menuChoice)->printClass() << endl;
                
                menuChoice = 1;
                
                break;
            case 4:
                if((menuChoice = getCharacterForMenu(list)) < 0)
                    break;
                
                cout << list.at(menuChoice)->printStats() << endl;

                menuChoice = 1;

                break;
            case 5:
                if((menuChoice = getCharacterForMenu(list)) < 0)
                    break;

                objTmp = list.at(menuChoice);
                
                cout << "0. Forza" << endl;
                cout << "1. Intelligenza" << endl;
                cout << "2. Fede" << endl;
                cout << "3. Costituzione" << endl;
                cout << "4. Agilità" << endl;
                cout << "5. Fortuna" << endl;

                menuChoice = getNumberFromUser((int)MinMaxValues::numOfStats);

                additionalInput = getStatFromUser();

                switch(menuChoice) {
                    case (int)Statistics::strength:
                        objTmp->setStr(additionalInput);
                        stringInput = "forza";
                        break;
                    case (int)Statistics::iq:
                        objTmp->setIq(additionalInput);
                        stringInput = "intelligenza";
                        break;
                    case (int)Statistics::piety:
                        objTmp->setPiet(additionalInput);
                        stringInput = "fede";
                        break;
                    case (int)Statistics::vitality:
                        objTmp->setVit(additionalInput);
                        stringInput = "costituzione";
                        break;
                    case (int)Statistics::agility:
                        objTmp->setAg(additionalInput);
                        stringInput = "agilità";
                        break;
                    case (int)Statistics::luck:
                        objTmp->setLuck(additionalInput);
                        stringInput = "fortuna";
                        break;
                    default:
                        cout << "come hai fatto a schizzare così?";
                        break;
                }

                cout << "Cambiata la " << stringInput << " di " << list.at(menuChoice)->printName()
                     << " a " << to_string(additionalInput) << endl;
                
                menuChoice = 1;

                break;
            case 6:
            {
                if((menuChoice = getCharacterForMenu(list)) < 0)
                    break;

                uint32_t tmpStats[(int)MinMaxValues::numOfStats];

                cout << "Inserisci tutte le statistiche in questo ordine: forza, "
                     << "intelligenza, fede, costituzione, agilità, fortuna." << endl;

                for(int i = 0; i < (int)MinMaxValues::numOfStats; i++)
                    tmpStats[i] = getStatFromUser();

                list.at(menuChoice)->setStats(tmpStats);

                cout << "Tutte le statistiche sono state cambiate.";

                break;
            }
            case 7:
                if((menuChoice = getCharacterForMenu(list)) < 0)
                    break;
                
                objTmp = list.at(menuChoice);
                
                cout << "0. Esperienza." << endl;
                cout << "1. Oro." << endl;

                menuChoice = getNumberFromUser(1);
                
                switch(menuChoice) {
                    case 0: cout << "Punti esperienza: " << objTmp->printExp() << endl;
                    case 1: cout << "Oro: " << objTmp->printGold() << endl;
                }

                menuChoice = 1;

                break;
            case 8:
                if((menuChoice = getCharacterForMenu(list)) < 0)
                    break;
                
                objTmp = list.at(menuChoice);
                
                cout << "0. Esperienza." << endl;
                cout << "1. Oro." << endl;

                menuChoice = getNumberFromUser(1);

                additionalInput = getNumberFromUser((int)MinMaxValues::maxMoneyExperience);

                switch(menuChoice) {
                    case 0: 
                        objTmp->setExp(additionalInput);
                        stringInput = "esperienza";
                        break;
                    case 1:
                        objTmp->setGold(additionalInput);
                        stringInput = "oro";
                        break;
                    default:
                        cout << "come hai fatto a schizzare così?";
                        break;
                }

                cout << "Cambiato/a l' " << stringInput << " di " << objTmp->printName()
                     << " a " << to_string(additionalInput) << endl;

                menuChoice = 1;
                
                break;
            case 9:
                if((menuChoice = getCharacterForMenu(list)) < 0)
                    break;
                
                cout << list.at(menuChoice)->printCharacter() << endl;

                menuChoice = 1;

                break;
            case 10:
                if(list.empty()){
                    cout << "Non ci sono personaggi." << endl; 
                    break;
                }

                cout << "Puoi effetuare due tipi di salvataggi:" << endl;
                cout << "0. Sovrascrivere solo la memoria RAM." << endl;
                cout << "1. Sovrascrivere il file su disco. (alto rischio!)" << endl;

                menuChoice = getNumberFromUser(1);

                switch(menuChoice) {
                    case 0:
                        for(int i = 0; i < list.size(); i++) {
                            list.at(i)->serializeClass(disk);
                            list.at(i)->serializeStats(disk);
                            list.at(i)->serializeExp(disk);
                            list.at(i)->serializeGold(disk);
                        }
                        cout << "Operazione compiuta con successo" << endl;
                        break;
                    case 1:
                        for(int i = 0; i < list.size(); i++) {
                            list.at(i)->serializeClass(disk);
                            list.at(i)->serializeStats(disk);
                            list.at(i)->serializeExp(disk);
                            list.at(i)->serializeGold(disk);
                        }
                        writeToDisk(disk, size);
                        cout << "Operazione compiuta con successo" << endl;
                        break;
                    default:
                        cout << "hai proprio schizzato." << endl;
                }

                menuChoice = 1;

                break;
            default:
                cout << "Scelta non valida. Riprova." << endl;
                break;
        }
    } while(menuChoice);

	delete[] disk;
	return 0;
}

unsigned int getNumberFromUser(int maxValid)
{
    unsigned int number;
    while (true) {
        cout << "Inserisci il numero: ";
        cin >> number;

        if (cin.fail() || number > maxValid) { //se inserito un numero non intero
            cin.clear(); //risolve flag di errore
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); //ignora input precedente

            cout << "Input non valido. Per favore, inserisci un numero positivo compreso tra 0 e "
                 << to_string(maxValid) << endl;
            
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        break;
    }
    cout << endl;
    return number;
}

unsigned int getStatFromUser()
{
    unsigned int input;

    do{
        input = getNumberFromUser((int)MinMaxValues::maxStats);
        if(input < 1)
            cout << "Le statistiche hanno un valore minimo di 1!" << endl;
    } while(input < 1);

    return input;
}

int getCharacterForMenu(vector<Character*> list)
{
    int menuChoice;
    if (list.empty()) {
        cout << "Non ci sono personaggi." << endl;
        return -1;
    }

    cout << "Lista dei personaggi: " << endl;

    for (int i = 0; i < list.size(); i++) {
        cout << i << ". " << list.at(i)->printName() << endl;
    }

    menuChoice = getNumberFromUser(list.size());

    return menuChoice;
}

int writeToDisk(int8_t* disk, size_t size)
{
    ofstream ofs("SAVE1.DSK");
    if(!ofs.good())
        return 69;

    ofs.write((char*)disk, size);

    ofs.close();

    return 0;
}