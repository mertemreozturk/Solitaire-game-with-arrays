#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "Cards.cpp"
using namespace std;

class Controller{
    public:
        Cards pile[7];
        Cards foundation[4];
        Cards waste;
        Cards stock;
        string deckArr[52];

        void control(string deckFile,string commandFile,string outputFile){
            ifstream input(deckFile);
            int deckSize = 51;
            while ( input ){       // store initial pile cards
                input >> deckArr[deckSize--];
            }
            input.close();

            int sizeOfTable = 0;
            for(int a = 0; a < 7; a++){
                pile[a].card = new Card[a+1];
                pile[a].size = a + 1;
                pile[a].openIndex = 1;
                inital(a+1,&pile[a]);
                if ( sizeOfTable < pile[a].size)
                    sizeOfTable = pile[a].size;
            }
            for ( int b = 0; b < 4; b++){
                foundation[b].card = new Card;
                foundation[b].size = 0;
                foundation[b].openIndex = 0;
            }

            waste.card = new Card;
            waste.size = 0;
            waste.openIndex = 0;
            fillStock(&stock);

            ofstream fout;
            fout.open(outputFile);

            print(pile,sizeOfTable,foundation,waste,stock,fout);
            
            ifstream inputTwo(commandFile);
            for ( string line; getline(inputTwo, line);){    // read command each iteration
                fout << "\n" << line << endl;
                istringstream ss(line);
                string word;
                string wordArr[5];
                int counter = 0;
                while (ss >> word){
                    wordArr[counter++] = word;
                }
                if ( wordArr[0] == "move"){
                    if ( wordArr[1] == "to"){
                        if ( wordArr[3] == "pile"){   // move to foundation pile x 
                            int pileNumber = stoi(wordArr[4]);
                            int fNumber;
                            if ( pile[pileNumber].size != 0){
                                char letter = pile[pileNumber].card[0].cardArr[0];  // letter of card 
                                fNumber = foundationNumber(letter);
                                change(&pile[pileNumber],&foundation[fNumber],1,1,fout); // if destination array is empty,flag is equal 1 for this operation
                            }else{    // empty array!
                                fout << "\nInvalid Move!\n";
                            } 
                        }else{      // move to foundation waste
                            int fNumber;
                            char letter;
                            letter = waste.card[0].cardArr[0];
                            fNumber = foundationNumber(letter);
                            change(&waste,&foundation[fNumber],1,1,fout);
                        }
                    }else if ( wordArr[1] == "pile"){  // move pile x y z 
                        int from = stoi(wordArr[2]);
                        int to = stoi(wordArr[4]);
                        int amount = stoi(wordArr[3]);   
                        if ( pile[to].openIndex != 0){     
                            change(&pile[from],&pile[to],amount+1,0,fout);
                        }else if(pile[to].size == 0) {
                            change(&pile[from],&pile[to],amount+1,0,fout);
                        }else{
                            fout << "\nInvalid Move!\n";
                        }     
                    }else if (wordArr[1] == "waste"){                              // move waste x
                        int pileNumber = stoi(wordArr[2]);
                        change(&waste,&pile[pileNumber],1,0,fout);
                    }else{
                        int pileNumber = stoi(wordArr[3]);
                        int fNumber = stoi(wordArr[2]);
                        change(&foundation[fNumber],&pile[pileNumber],1,0,fout);
                    }
                }else if(wordArr[0] == "open") {
                    if ( wordArr[1] == "from"){      // open from stock
                        fromStock(&waste,&stock);
                    }else{                 // open x
                        int pileNumber = stoi(wordArr[1]);
                        if ( pile[pileNumber].openIndex == 0){
                            pile[pileNumber].openIndex++;
                        }else{
                            fout << "\nInvalid Move!\n";
                        }
                    }
                }else{     
                    // exit
                    fout << "\n****************************************\n";
                    break;
                }
                int tempCount = 0;
                for ( int count = 0; count < 7;count++){   // define table size
                    if ( tempCount < pile[count].size)
                        tempCount = pile[count].size;
                }
                sizeOfTable = tempCount;
                fout <<"\n****************************************\n";
                print(pile,sizeOfTable,foundation,waste,stock,fout);  // print table after each command
            }
            inputTwo.close();
            // check whether game winning or losing
            if ( foundation[3].size == 13 && foundation[2].size == 13 && foundation[1].size == 13 && foundation[0].size == 13)
                fout << "\n\n****************************************\n\nYou Win!\n";
            fout << "\nGame Over!";
            
            fout.close();

            clean(pile,foundation,waste,stock);  // free memory
        }


        void inital(int size,Cards *p){
            int step = 0;
            for ( int i = 0; i < size; i++){  // store piles
                p->card[i].cardArr = &deckArr[size+step-1][0];
                step +=(6-i);
            }
            
            for ( int j = 0; j < size/2; j++){    // reversing
                char* temp = p->card[j].cardArr;
                p->card[j].cardArr = p->card[size -1 -j].cardArr;
                p->card[size -1 -j].cardArr = temp;
            }
        }

        void fillStock(Cards *s){
            s->card = new Card[24];
            for ( int i = 0; i < 24; i++){    // store stock
                s->card[i].cardArr = &deckArr[28+i][0];
            }
            s->size = 24;
            s->openIndex = 0;
        }

        void fromStock(Cards *w,Cards *s){
            int count = s->openIndex, sizeOfStock = s->size, sizeOfwaste = w->size;

            if ( count == sizeOfStock){
                delete  [] s->card;
                s->openIndex = 0;
                s->size = sizeOfwaste;
                s -> card = new Card[sizeOfwaste];
                for ( int i = 0; i < sizeOfwaste; i++){
                    s->card[i].cardArr = w->card[sizeOfwaste-1-i].cardArr;
                }
                delete [] w->card;
                w->openIndex = 0;
                w->size = 0;
            }else{
                Cards temp;
                int width;
                if ( sizeOfStock - count >= 3){
                    width = 3;
                }else{
                    width = sizeOfStock - count;
                }
                temp.card = new Card[sizeOfwaste+width];
                for (int i = 0; i < width; i++){
                    temp.card[i].cardArr = s->card[count+width-1-i].cardArr;
                }
                
                for ( int j = 0; j < sizeOfwaste; j++){
                    temp.card[j+width].cardArr = w->card[j].cardArr;
                }
                if ( sizeOfwaste != 0){
                    delete [] w->card;
                }
                w->card = temp.card;
                w->size += width;
                w->openIndex = width;
                s->openIndex += width;
            }
        }

        int foundationNumber(char c){
            int foundationNumber;
            switch (c){
                case 'H':
                    foundationNumber = 0;
                    break;
                case 'D':
                    foundationNumber = 1;
                    break;
                case 'S':
                    foundationNumber = 2;
                    break;
                default:
                    foundationNumber = 3;
                    break;
            }
            return foundationNumber;
        }

        bool check(Cards *p1,Cards *p2,int x,int flag){
            if ( p1->openIndex == 0){
                return false;
            }else if ( p1->size != 0 && p2->size != 0){
                string one = &p1->card[x].cardArr[1];
                string two = &p2->card[0].cardArr[1];
                int cNumberOne = stoi(one),cNumberTwo = stoi(two);
                char cOne = p1->card[x].cardArr[0],cTwo = p2->card[0].cardArr[0];

                if ( cNumberTwo - cNumberOne == 1 && flag == 0) {  // Are they consecutive number and different color
                    if ( cOne == 'S' || cOne == 'C' && cTwo == 'D' || cTwo == 'H'){
                        return true;
                    }else if ( cOne == 'D' || cOne == 'H' && cTwo == 'S' || cTwo == 'C'){
                        return true;
                    }else{
                        return false;
                    }
                }else if ( cNumberOne - cNumberTwo == 1 && flag == 1){
                    return true;
                }else{
                    return false;
                }
            }else if ( p1->size == 0){
                return false;
            }else{
                string temp = &p1->card[x].cardArr[1];
                int tempNum = stoi(temp);
                if ( flag == 0){
                    if ( tempNum == 13){  // if destination is a pile and it is empty, value must be 13
                        return true;
                    }else{
                        return false;
                    }
                }else{
                    if ( tempNum == 1){   // source is a foundation and it is empty, value must be 1
                        return true;
                    }else{
                        return false;
                    }
                }
            }
        }

        void change(Cards *p1,Cards *p2,int howMany,int flag,ofstream &fout){
            if ( check(p1,p2,howMany-1,flag)){
                Cards tempForP2;
                int sizeOfP2 = p2->size;
                tempForP2.card = new Card[sizeOfP2+howMany];   // new size destination area
                for ( int i = 0; i < howMany ; i++){
                    tempForP2.card[i].cardArr = p1->card[i].cardArr;
                }
                for ( int j = 0; j < sizeOfP2; j++){
                    tempForP2.card[howMany+j].cardArr = p2->card[j].cardArr;
                }
                
                delete [] p2->card;
                p2->card = tempForP2.card;   // new elements
                p2->size += howMany;
                p2->openIndex += howMany;

                int sizeOfp1 = p1->size;
                Cards temp;
                temp.card = new Card[sizeOfp1-howMany];    // new size source area
                for ( int k = 0; k < sizeOfp1 - howMany; k++){
                    temp.card[k].cardArr = p1->card[howMany+k].cardArr;
                }
                
                delete [] p1->card;   
                p1->card = temp.card;   // new elements
                p1->size -= howMany;
                p1->openIndex -= howMany;
            }else{
                fout << "\nInvalid Move!\n";
            }
        }

        void print(Cards *p, int sizeOfTable,Cards *f,Cards w,Cards s,ofstream &fout){
            fout << endl;
            if ( s.openIndex != s.size){
            fout << "@@@" ; 
            }else{
                fout << "___";
            }
            for ( int a = 0; a < 3; a++){
                if ( w.size != 0 && a <= w.openIndex - 1 ){
                    fout << " " << w.card[w.openIndex - 1 -a].cardArr;
                }else if (w.size != 0 && a == 0){
                    fout << " " << w.card[0].cardArr;
                }else{
                    fout << " ___";
                }
            }
            fout << "        ";
            for ( int b = 0; b < 4;b++){
                if ( f[b].size != 0){
                    fout << " " << f[b].card[0].cardArr;
                }else{
                    fout << " ___";
                }
            }
            fout << "\n\n";
            for ( int i = 0; i < sizeOfTable; i++){
                for ( int j = 0; j < 7; j++){   
                    if ( i < p[j].size && i >= p[j].size - p[j].openIndex ){
                        fout << p[j].card[p[j].size - i -1].cardArr << "   ";
                    }else if ( i < p[j].size){
                        fout << "@@@   ";
                    }else{
                        fout << "      ";
                    }
                }   
                fout << endl;      
            }
        }

        void clean(Cards *p,Cards *f,Cards w,Cards s){
            for ( int i = 0; i < 7; i++){
                if ( p[i].size != 0){
                    delete [] p[i].card;
                }
            }
            for ( int j = 0; j < 4; j++){
                if ( f[j].size != 0){
                    delete [] f[j].card;
                }
            }
            if ( w.size != 0){
                delete [] w.card;
            }
            if ( s.size != 0){
                delete [] s.card;
            }
        }

};