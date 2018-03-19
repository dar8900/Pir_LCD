#include "func_lib.h"

//#define VERBOSE


typedef unsigned int uint;


/*************** FUNZIONI E VARIABILI STATICHE ************/
static ifstream testo;
void rename(char nomefile[]);


/*************** FUNZIONI DI CONVERSIONE ********************/

string ToHex(int num_da_convertire)
{
    stringstream ss;
    string hex_num;
    ss<<uppercase<<hex<<num_da_convertire;
    hex_num = ss.str();
    return hex_num;

}


int HextoInt(string Hex)
{
    int int_conv;
    bool chek_hex = true;
    do
    {
        label:
        if(chek_hex == false)
        {
            scrivi('\n'<<"Re-inserire l'esadecimale"<<'\n');
            cin>>Hex;
        }
        if(Hex.size() <= 4)
        {
            for(uint i = 0; i < Hex.size(); i++)
            {
                if(Hex[i] != '0' && Hex[i] != '1' && Hex[i] != '2' && Hex[i] != '3' && Hex[i] != '4' && Hex[i] != '5' && Hex[i] != '6' && Hex[i] != '7' && Hex[i] != '8' && Hex[i] != '9' && Hex[i] != 'A' && Hex[i] != 'B' && Hex[i] != 'C' && Hex[i] != 'D' && Hex[i] != 'E' && Hex[i] != 'F'&& Hex[i] != 'a' && Hex[i] != 'b' && Hex[i] != 'c' && Hex[i] != 'd' && Hex[i] != 'e' && Hex[i] != 'f')
                {
                    scrivi('\n'<<"Hai inserito un valore incorretto"<<'\n');
                    chek_hex = false;
                    goto label;
                }
            }
            stringstream convert;
            convert<<hex<<Hex;
            convert>>int_conv;
            chek_hex = true;
        }
        else
        {
            scrivi('\n'<<"L'esadecimale e' troppo lungo"<<'\n');
            chek_hex = false;
            continue;
        }
    }while(chek_hex == false);
    return int_conv;
}



string BintoHex(string bin, bool chek_size)
{
    bool chek_ok_bin = true;
    string Hex;

    do
    {
        if(chek_ok_bin == false)
        {
           scrivi('\n'<<"Re-inserire il numero binario"<<'\n');
           cin>>bin;
        }


        if(bin == "0000")
        {
            Hex = "0";
            chek_ok_bin = true;
        }

        else if(bin == "0001")
        {
            Hex = "1";
            chek_ok_bin = true;
        }

        else if(bin == "0010")
        {
            Hex = "2";
            chek_ok_bin = true;
        }

        else if(bin == "0011")
        {
            Hex = "3";
            chek_ok_bin = true;
        }

        else if(bin == "0100")
        {
            Hex = "4";
            chek_ok_bin = true;
        }

        else if(bin == "0101")
        {
            Hex = "5";
            chek_ok_bin = true;
        }

        else if(bin == "0110")
        {
            Hex = "6";
            chek_ok_bin = true;
        }

        else if(bin == "0111")
        {
            Hex = "7";
            chek_ok_bin = true;
        }

        else if(bin == "1000")
        {
             Hex = "8";
            chek_ok_bin = true;
        }

        else if(bin == "1001")
        {
            Hex = "9";
            chek_ok_bin = true;
        }

        else if(bin == "1010")
        {
            Hex = "A";
            chek_ok_bin = true;
        }

        else if(bin == "1011")
        {
            Hex = "B";
            chek_ok_bin = true;
        }

        else if(bin == "1100")
        {
            Hex = "C";
            chek_ok_bin = true;
        }

        else if(bin == "1101")
        {
            Hex = "D";
            chek_ok_bin = true;
        }

        else if(bin == "1110")
        {
            Hex = "E";
            chek_ok_bin = true;
        }

        else if(bin == "1111")
        {
            Hex = "F";
            chek_ok_bin = true;
        }

        else
        {
           if(chek_size == false)
           {
               if(bin.size() != 4)
               {
                   scrivi('\n'<<"Hai inserito troppe cifre"<<'\n');
                   chek_ok_bin= false;
                   continue;
               }
           }

           for(uint i = 0; i < bin.size(); i++)
           {
               if(bin[i] != '0' && bin[i] != '1')
               {
                   scrivi('\n'<<"Hai inserito cifre sbagliate"<<'\n');
                   chek_ok_bin = false;
                   break;
               }


           }

        }

    }while(chek_ok_bin == false);



    return Hex;
}

int String2Int(string numero){

        stringstream conv;
        conv.str(numero);
        int numConv;
        conv >> numConv;
        return numConv;

}

string Int2String(int numero){

        stringstream conv;
        conv << numero;
        string numConv;
        numConv = conv.str();
        return numConv;

}

/*************** FUNZIONI PER ARRAY **************/

void newl(int num_nl)
{
    for(int i=0;i<=num_nl;i++)
    {
        printf("\n");
    }

}

void ArrayInit0(int Array[],int dim)
{
    for(int i=0;i<dim;i++)
    {
        Array[i]=0;
    }
}

void ArrayBig2Smll(int Array[],int dim)
{
    srand(time(0));
    for(int i=dim-1;i>=0;i--)
    {
        Array[i]=i;
        printf("%d\n",Array[i]);
    }

}

void BubbleSort(int Array[], int dim)
{
    int tmp=0;

    for(int i=0;i<dim;i++)
    {
        for(int j=0;j<dim-i;j++)
        {
            if(Array[j]>Array[j+1])
            {
                tmp=Array[j+1];
                Array[j+1]=Array[j];
                Array[j]=tmp;
            }


        }

    printf("%d\n",Array[i]);
    }
}

void MintoMax(int Array[], int dim)
{
    for(int i = 0; i < dim; i++)
    {
        int minim = i;

        for(int j = i+1; j < dim; j++)
        {
            if(Array[j] < Array[minim])
                minim = j;
        }

        int temp = Array[minim];
        Array[minim] = Array[i];
        Array[i] = temp;
    }
}




/******** FUNZIONI PER LETTURA TESTO **************/

void rename(char nomefile[])
{
    cout<<"Inserire il nome del file"<<endl;
    cin>>nomefile;
    strcat(nomefile,".txt");
}



void graphx(double perc,char vocali)
{
#define larg 10

    cout<<"[";

    for(int i=0;i<(int)perc;i++)
    {
        cout<<"-";
    }

    cout<<"]"<<setw(larg+10)<<perc<<"% di lettera '"<<vocali<<"'"<<'\n'<<endl;

}



bool open_file_check(string nomefile)
{
    ifstream aprifile;
    bool file_open;

    aprifile.open(nomefile);

    if(aprifile.is_open())
        file_open = true;
    else
        file_open = false;

    return file_open;

}


void open_file_2read(ifstream& testo)
{
    //ifstream testo;
    string nome_file;


    do
    {
    cout<<'\n'<<"Inserisci il nome del file, con estesione"<<endl;
    cin>>nome_file;
    delay(0.5);
    testo.open(nome_file);
#ifdef VERBOSE
    if(testo.is_open())
        cout<<'\n'<<"Il file \""<<nome_file<<"\" e' aperto"<<'\n'<<endl;
    else
        cout<<'\n'<<"Il file "<<nome_file<<" non aperto, ritenta!"<<'\n'<<endl;
#endif // VERBOSE
    }while(!testo.is_open());
}

void open_file_2read(ifstream& testo, string nome_file)
{
    //ifstream testo;
    do
    {
    //cout<<'\n'<<"Inserisci il nome del file, con estesione"<<endl;
    //cin>>nome_file;
    delay(0.5);
    testo.open(nome_file);

#ifdef VERBOSE
    if(testo.is_open())
        cout<<'\n'<<"Il file \""<<nome_file<<"\" e' aperto"<<'\n'<<endl;
    else
        cout<<'\n'<<"Il file \""<<nome_file<<"\" non aperto, ritenta!"<<'\n'<<endl;
#endif // VERBOSE
    }while(!testo.is_open());
    return;
}


void open_file_2read(ifstream& testo, string nome_file, bool log)
{
    //ifstream testo;
    do
    {
    //cout<<'\n'<<"Inserisci il nome del file, con estesione"<<endl;
    //cin>>nome_file;
    delay(0.5);
    testo.open(nome_file);

    if(log == true)
    {
         if(testo.is_open())
            cout<<'\n'<<"Il file \""<<nome_file<<"\" e' aperto"<<'\n'<<endl;
        else
            cout<<'\n'<<"Il file \""<<nome_file<<"\" non aperto, ritenta!"<<'\n'<<endl;
    }


    }while(!testo.is_open());
}



void open_file_2write(ofstream& testo)
{
    string nome_file;
    cout<<'\n'<<"Inserisci il nome del file, con estesione"<<endl;
    cin>>nome_file;
    testo.open(nome_file);
    delay(0.5);
    #ifdef VERBOSE
    cout<<'\n'<<"Il file \""<<nome_file<<"\" e' stato creato"<<'\n'<<endl;
    #endif // VERBOSE
}

void open_file_2write(ofstream& testo, string nome_file)
{
    testo.open(nome_file);
    delay(0.5);
    #ifdef VERBOSE
    cout<<'\n'<<"Il file \""<<nome_file<<"\" e' stato creato"<<'\n'<<endl;
    #endif // VERBOSE
}


void open_file_2write(ofstream& testo, string nome_file, bool log)
{
    testo.open(nome_file);
    if(log == true)
        cout<<'\n'<<"Il file \""<<nome_file<<"\" e' stato creato"<<'\n'<<endl;
}

void WriteVector_inFile(vector<string>& vettore, string nome_file)
{
     ofstream file_da_scrivere;
     open_file_2write(file_da_scrivere, nome_file);
     for(uint i = 0; i < vettore.size(); i++)
     {
             file_da_scrivere << vettore[i] << '\n';
     }
     close_file(file_da_scrivere);
}

void WriteVector_inFile(vector<int>& vettore, string nome_file)
{
     ofstream file_da_scrivere;
     open_file_2write(file_da_scrivere, nome_file);
     for(uint i = 0; i < vettore.size(); i++)
     {
             file_da_scrivere << vettore[i] << '\n';
     }
     close_file(file_da_scrivere);
}

void WriteVector_inFile(vector<string>& vettore, string nome_file, bool chekEmpty)
{
     ofstream file_da_scrivere;
     open_file_2write(file_da_scrivere, nome_file);
     if(!chekEmpty){
        for(uint i = 0; i < vettore.size(); i++)
        {
                file_da_scrivere << vettore[i] << '\n';
        }
     }
     else{
        for(uint i = 0; i < vettore.size(); i++){
                if(vettore[i].empty() || vettore[i] == " ")
                        continue;
                else
                       file_da_scrivere << vettore[i] << '\n';

        }
     }


     close_file(file_da_scrivere);
}

void WriteSet_inFile(set<string>& insieme, string nome_file)
{
     ofstream file_da_scrivere;
     open_file_2write(file_da_scrivere, nome_file);
     for (set<string>::iterator it=insieme.begin(); it!=insieme.end(); ++it)
     {
             file_da_scrivere << *it <<'\n';
     }
     close_file(file_da_scrivere);
}


void write_in_file(ofstream& testo, vector<string>& vettore)
{
    for(uint i = 0; i < vettore.size(); i++)
    {
        testo<<vettore[i]<<'\n';
    }

    delay(0.4);
    #ifdef VERBOSE
    scrivi('\n'<<"File scritto"<<'\n');
    #endif // VERBOSE
    close_file(testo);

}

void write_in_file(ofstream& testo, vector<string>& vettore, bool Log)
{
    for(uint i = 0; i < vettore.size(); i++)
    {
        testo<<vettore[i]<<'\n';
    }

    if(Log)
    {
        delay(0.4);
        scrivi('\n'<<"File scritto"<<'\n');
    }


    close_file(testo);

}

void write_in_file(ofstream& testo, vector<string>& vettore, int index)
{
    testo<<vettore[index]<<'\n';
}

void write_in_file(ofstream& testo, vector<string>& vettore, uint index)
{
    testo<<vettore[index]<<'\n';
}


void write_in_file(ofstream& testo, string riga)
{
    testo<<riga<<'\n';
}

void write_in_file(ofstream& testo, set<string>& insieme)
{
    for (set<string>::iterator it=insieme.begin(); it!=insieme.end(); ++it)
    {
        testo<<*it<<'\n';
    }
    delay(0.4);
    #ifdef VERBOSE
    scrivi('\n'<<"File scritto"<<'\n');
    #endif // VERBOSE
    close_file(testo);
}

void LineText_in_Vector(vector<string>& vettore, ifstream& testo)
{
    string riga_tmp;
    while(!testo.eof())
    {
        getline(testo, riga_tmp);
        vettore.push_back(riga_tmp);
    }
}


void LineText_in_Set(set<string>& insieme, ifstream& testo)
{
    string riga_tmp;
    while(!testo.eof())
    {
        getline(testo, riga_tmp);
        insieme.insert(riga_tmp);
    }
}


void mem_file_in_vector(vector<string>& lista, string nome_file){
        ifstream apri_file;
        open_file_2read(apri_file, nome_file);
        LineText_in_Vector(lista, apri_file);
        close_file(apri_file);
}

void mem_file_in_set(set<string>& lista, string nome_file){
        ifstream apri_file;
        open_file_2read(apri_file, nome_file);
        LineText_in_Set(lista, apri_file);
        close_file(apri_file);
}

void remove_file(string file_2erase)
{
    if(remove(file_2erase.c_str()) == 0)
    {
        delay(0.3);
        #ifdef VERBOSE
        cout<<'\n'<<"Il file \""<<file_2erase<<"\" e' stato eliminato"<<'\n'<<endl;
        #endif // VERBOSE
    }
    #ifdef VERBOSE
    else
        scrivi("Il file non e' stato trovato ed eliminato");
    #endif // VERBOSE

}

void remove_file(string file_2erase, bool Log)
{
    if(remove(file_2erase.c_str()) == 0)
    {
        if(Log)
        {
            delay(0.3);
            cout<<'\n'<<"Il file \""<<file_2erase<<"\" e' stato eliminato"<<'\n'<<endl;
        }

    }

    else
    {
        if(Log)
            scrivi("Il file non e' stato trovato ed eliminato");
    }


}


void close_file(ifstream& testo)
{
   testo.close();
   #ifdef VERBOSE
   scrivi('\n'<<"File chiuso"<<'\n');
   #endif // VERBOSE
}

void close_file(ofstream& testo)
{
   testo.close();
   #ifdef VERBOSE
   scrivi('\n'<<"File chiuso"<<'\n');
   #endif // VERBOSE
}

void close_file(ifstream& testo, bool log)
{
   testo.close();
   if(log)
    scrivi('\n'<<"File chiuso"<<'\n');
}

void close_file(ofstream& testo, bool log)
{
   testo.close();
   if(log)
    scrivi('\n'<<"File chiuso"<<'\n');
}


int apri_testo(char nomefile[])
{

    char *div, char_temp;
    int word_tot=0;

    do
{
    #ifndef NOMEFILE
    rename(nomefile);
    #endif // NOMEFILE

    delay(1);
    testo.open(nomefile);

    if(testo.is_open())
        cout<<'\n'<<nomefile<<" aperto"<<endl;
    else
    {
        div=strtok(nomefile,".txt");
        cout<<'\n'<<div<<" non aperto, ritenta!"<<'\n'<<endl;
    }

} while(!testo.is_open());

    while(!testo.eof())
    {
        testo.get(char_temp);
        word_tot++;

    }
    testo.close();

    return word_tot;

}

void mem_testo (char nomefile[], char words[])
{

    testo.open(nomefile);
    char char_temp;
    int cnt=0;

    while(!testo.eof())
    {
        testo.get(char_temp);
        words[cnt]=char_temp;
        cnt++;
    }
    testo.close();

}


void searc_word(char nomefile[])
{

    vector <string> words;
    string word, what_word;
    int cnt[]={0,0,0};

    bool again;

    testo.open(nomefile);

    while(testo>>word)
    {
        words.push_back(word);
        cnt[0]++;

    }

    testo.close();

    int cnt_array[cnt[0]];

    do
    {
        cnt[1]=0;

        cout<<'\n'<<"Inserisci la parola da cercare"<<'\n'<<endl;
        cin>>what_word;

        for(int i=0; i<(int)words.size(); i++)
        {
            if(words[i] == what_word)
            {
                cnt[1]++;
                cnt_array[i]=i;
            }
            else
            {
                cnt_array[i]=0;

            }
        }

        if(cnt[1]==0)
            {
                char risp;
                do
                {
                    cout<<'\n'<<"La parola cercata non e' stata trovata nel testo, digitare 's' per effettuare un altra ricerca o 'n' per chiudere"<<'\n'<<endl;
                    cin>>risp;
                    if(risp == 's')
                        again=true;
                    else if(risp =='n')
                        again=false;
                    else
                        cout<<"Ridigita!"<<endl;
                }while(risp != 's' && risp != 'n' );

            }

        else
        {
            cout<<'\n'<<"La parola '"<<what_word<<"' compare "<<cnt[1]<<" volte"<<'\n'<<endl;

            for(int i=0; i<(int)words.size(); i++)
            {
                if(cnt_array[i]>0)
                cout<<'\n'<<"La la tua parola occupa la posizione "<<cnt_array[i]<<'\n'<<endl;
            }

            char risp;
            do
            {

                cout<<'\n'<<"La ricerca e' terminata, digitare 's' per effettuare un altra ricerca o 'n' per chiudere"<<'\n'<<endl;
                cin>>risp;
                if(risp== 's')
                    again=true;
                else if(risp=='n')
                    again=false;
                else
                    cout<<"Ridigita!"<<endl;
            }while(risp !='s' && risp != 'n' );

        }

    }while(again);

    cout<<"Programma terminato"<<endl;
}

void getWordsInSet(string nome_file, set<string>& listaWords){

        ifstream testo;
        vector<string> vettore_temp;
        open_file_2read(testo, nome_file);
        mem_file_in_vector(vettore_temp, nome_file);
        close_file(testo);
        for(uint curr_word = 0; curr_word < vettore_temp.size(); curr_word++){

                string word, phrase = vettore_temp[curr_word];
                if(vettore_temp[curr_word].empty())
                        continue;
                for(uint curr_lett = 0; curr_lett < phrase.size()+1; curr_lett++){
                        char letter = phrase[curr_lett];
                        if((letter < (int) 'A' && letter > (int) 'Z') || (letter < (int) 'a' && letter > (int) 'z') || letter == ' ' || letter == '\0'){
                                listaWords.insert(word);
                                word  = ' ';
                                continue;
                        }
                        else{
                                if(word[0] == ' ')
                                        word.erase(word.begin());
                                word += letter;
                        }
                }
        }
        return;
}


