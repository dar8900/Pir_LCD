#ifndef FUNC_LIB_H
#define FUNC_LIB_H

#include<iostream>
#include<stdio.h>
#include<fstream>
#include<time.h>
#include<stdlib.h>
#include<cstring>
#include<vector>
#include<set>
#include<iomanip>
#include<cstdio>
#include<cstdlib>

#include<string>

#include<sstream>

#define WINDOWS_SYS
//#define LINUX_SYS

#ifdef WINDOWS_SYS
#include<windows.h>
#define delay(s) Sleep(s*1000)
#endif // WINDOWS_SYS

#ifdef LINUX_SYS
#include <unistd.h>
#define delay(s)  usleep(s*1000000);
#endif // LINUX_SYS



#define scrivi(roba)    cout<<roba<<endl
#define log(ns,roba)    cout<<roba<<endl; delay(ns)
#define WriteOnFile(fileOfstream, Roba_Da_Scrivere)     fileOfstream << Roba_Da_Scrivere << '\n'


//#define NOMEFILE  "viaggio.txt"

using namespace std;

typedef unsigned int uint;
typedef int counter;

template <typename Class>
void WriteGenericArrayInFile(Class array2Write[], int sizeArray, string Nomefile){
        ofstream File2Write;
        File2Write.open(Nomefile);
        if(File2Write.is_open()){
                for(int i = 0 ; i < sizeArray; i++){
                        File2Write << array2Write[i] << '\n';
                }
                File2Write.close();
        }
        return;
}

/**************** FUNZIONI DI CONVERSIONE *************************/

string ToHex(int num_da_convertire);
string BintoHex(string bin, bool chek_size);
int HextoInt(string Hex);
int String2Int(string numero);
string Int2String(int numero);

/*************** PER ARRAY *****************************/

void newl(int num_nl);
void ArrayInit0(int Array[],int dim);
void ArrayBig2Smll(int Array[],int dim);
//void BubbleSort(int Array[], int dim);
void MintoMax(int Array[], int dim);

/*************** ELABORAZIONE TESTO ********************/

bool open_file_check(string nomefile);

void open_file_2read(ifstream& testo); // Richiede all'utente di immettere il nome del file
void open_file_2read(ifstream& testo, string nome_file); //Viene passato direttamente una stringa contenente il nome del file da aprire
void open_file_2read(ifstream& testo, string nome_file, bool log);// Non scrive nulla

void open_file_2write(ofstream& testo);
void open_file_2write(ofstream& testo, string nome_file);
void open_file_2write(ofstream& testo, string nome_file, bool log);

void write_in_file(ofstream& testo, vector<string>& vettore);
void write_in_file(ofstream& testo, vector<string>& vettore, bool Log);
void write_in_file(ofstream& testo, vector<string>& vettore, int index);
void write_in_file(ofstream& testo, vector<string>& vettore, uint index);
void write_in_file(ofstream& testo, string riga);
void write_in_file(ofstream& testo, set<string>& insieme);
void WriteVector_inFile(vector<string>& vettore, string nome_file);
void WriteVector_inFile(vector<int>& vettore, string nome_file);
void WriteVector_inFile(vector<string>& vettore, string nome_file, bool chekEmpty);
void WriteSet_inFile(set<string>& insieme, string nome_file);

void close_file(ifstream& testo);
void close_file(ofstream& testo);
void close_file(ifstream& testo, bool log);
void close_file(ofstream& testo, bool log);

void LineText_in_Vector(vector<string>& vettore, ifstream& testo);
void LineText_in_Set(set<string>& insieme, ifstream& testo);
void mem_file_in_vector(vector<string>& lista, string nome_file);
void mem_file_in_set(set<string>& lista, string nome_file);

void getWordsInSet(string nome_file, set<string>& listaWords);

void remove_file(string file_2erase); // Rimuove il file con il nome della stringa dalla cartella
void remove_file(string file_2erase, bool Log);

int apri_testo(char nomefile[]); // Ritorna la dimensione che devo fornire alla vettore char words[] di mem_testo
void mem_testo (char nomefile[], char words[]);
void searc_word(char nomefile[]);
void graphx(double perc,char vocali);

#endif // FUNC_LIB_H
