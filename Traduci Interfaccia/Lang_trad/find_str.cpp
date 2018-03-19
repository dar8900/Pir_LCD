#include "find_str.h"

void SearchStr()
{
        vector<string> strings;
        vector<string> stringhe_da_tradurre;
        string nome_file = "setup.c";
        scrivi("Inserire il nome con le stringhe da tradurre");
        //cin >> nome_file;
        mem_file_in_vector(strings, nome_file);
        for(uint vec_line = 0; vec_line < strings.size(); vec_line++)
        {
                string linea = strings[vec_line];

                for(uint char_idx = 0; char_idx < linea.size(); char_idx++)
                {
                        int char4trad = char_idx;
                        if(linea[char_idx] == '"')
                        {
                                char4trad++;
                                string stringa_da_tradurre;
                                while(linea[char4trad] != '"')
                                {
                                        stringa_da_tradurre += linea[char4trad++];
                                }
                                stringhe_da_tradurre.push_back(stringa_da_tradurre);
                                break;

                        }


                }

        }
        WriteVector_inFile(stringhe_da_tradurre, "Stringhe da tradurre.txt");

}
