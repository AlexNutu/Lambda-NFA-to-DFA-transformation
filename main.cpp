#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
using namespace std;

struct tranzitie{
    char sim;
    int st;
};

int nrStari,nrCaractere;
char* caractere;
int nrStariFinale,*stariFinale;
int *stare,*nrTr;
int nrStariCuTranzitii;
vector <vector <tranzitie> > myVector;

struct setMarker{
    vector<int> StateOfDFA;
    bool isMarked;
};

struct DFAtransition{
    vector<int> initialStatesOfDFA;
    char symbol;
    vector<int> finalStatesOfDFA;
};

struct DFA{
    int noOfState;
    int noOfSymbols;
    int noOfTransition;
    setMarker allStatesOfDFA[100];
    DFAtransition dfaTransition[100];
} DFAinit;


int existInSet(vector<int> setOfStates, int state){
    for(unsigned int j=0; j< setOfStates.size();j++)
        if(setOfStates.at(j) == state)
            return 0;
    return 1;
}

void e_closure(vector<int> &setOfStates,int initialState){
    if(existInSet(setOfStates,initialState))
        setOfStates.push_back(initialState);
    for(unsigned int j=0; j< myVector[initialState].size();j++)
        if(myVector[initialState][j].sim == 'e' && existInSet(setOfStates,myVector[initialState][j].st)){
            e_closure(setOfStates,myVector[initialState][j].st);
    }
}

void e_closure(vector<int> &setOfStates,vector<int> setOfStatesNFA){
    for(unsigned int j=0; j< setOfStatesNFA.size();j++)
        e_closure(setOfStates,setOfStatesNFA.at(j));
}

vector<int> MoveNFA(vector<int> setOfStatesNFA, char symbol){
    vector<int> setOfStates;
    for(unsigned int j=0; j< setOfStatesNFA.size();j++)
        for(unsigned int i=0; i< myVector[setOfStatesNFA.at(j)].size();i++)
            if(myVector[setOfStatesNFA.at(j)][i].sim == symbol && existInSet(setOfStates,myVector[setOfStatesNFA.at(j)][i].st))
                setOfStates.push_back(myVector[setOfStatesNFA.at(j)][i].st);
    return setOfStates;
}

vector<int> MoveDFA(vector<int> setOfStatesNFA, char symbol){
    vector<int> setOfStates;
    e_closure(setOfStates,MoveNFA(setOfStatesNFA,symbol));
    return setOfStates;
}

void addSetToDFA(vector<int> Set){
    DFAinit.allStatesOfDFA[DFAinit.noOfState].StateOfDFA = Set;
    DFAinit.allStatesOfDFA[DFAinit.noOfState].isMarked = false;
    DFAinit.noOfState++;

}

int DFAcontainsUnmarkedInitStates(){
    for(int i=0;i<DFAinit.noOfState;i++)
        if(DFAinit.allStatesOfDFA[i].isMarked == false)
            return 1;
    return 0;
}
int getIndexOfASetUnmark(){
    for(int i=0;i<DFAinit.noOfState;i++)
        if(DFAinit.allStatesOfDFA[i].isMarked == false)
            return i;
    return -1;
}

void sorteaza(vector<int> &Set, int n){
    for(int i=0;i<n-1;i++)
        for(int j=i+1;j<n;j++)
            if(Set.at(i) > Set.at(j))
                swap(Set.at(i),Set.at(j));
}

void Mark(vector<int> Set){
    sorteaza(Set,Set.size());
    for(int i=0;i<DFAinit.noOfState;i++){
        if(DFAinit.allStatesOfDFA[i].StateOfDFA.size() == Set.size()){
            sorteaza(DFAinit.allStatesOfDFA[i].StateOfDFA,DFAinit.allStatesOfDFA[i].StateOfDFA.size());
            int sem=1;
            for(int j=0;j<DFAinit.allStatesOfDFA[i].StateOfDFA.size();j++)
                if(DFAinit.allStatesOfDFA[i].StateOfDFA.at(j) != Set.at(j))
                    sem=0;
            if(sem==1)
                DFAinit.allStatesOfDFA[i].isMarked = true;
        }
    }
}


int isInStatesOfDFA(vector<int> Set){
    sorteaza(Set,Set.size());
    for(int i=0;i<DFAinit.noOfState;i++){
        if(DFAinit.allStatesOfDFA[i].StateOfDFA.size() == Set.size()){
            sorteaza(DFAinit.allStatesOfDFA[i].StateOfDFA,DFAinit.allStatesOfDFA[i].StateOfDFA.size());
            int sem=1;
            for(int j=0;j<DFAinit.allStatesOfDFA[i].StateOfDFA.size();j++)
                if(DFAinit.allStatesOfDFA[i].StateOfDFA.at(j) != Set.at(j)){
                    sem=0;
                }
            if(sem==1)
                return 1;
        }
    }
    return 0;
}

int isEmpty(vector <int> temp){
    if(temp.size()){
        return 1;
    }
    return 0;
}

void conversion_from_NFA_to_DFA(){
    vector <int> temp;
    e_closure(temp, 0);
    addSetToDFA(temp);
    while(DFAcontainsUnmarkedInitStates()){
        int index = getIndexOfASetUnmark();
        Mark(DFAinit.allStatesOfDFA[index].StateOfDFA);
        vector<int> T = DFAinit.allStatesOfDFA[index].StateOfDFA;
        for (int i=1;i<=nrCaractere;i++){
            vector<int> S;
            e_closure(S,MoveNFA(T,caractere[i]));
            if(isInStatesOfDFA(S)==0)
                addSetToDFA(S);

            //S = MoveDFA(T,caractere[i]);
            if(isEmpty(S) && isEmpty(T)){
                sorteaza(S, S.size());
                sorteaza(T, T.size());
                DFAinit.noOfTransition++;
                DFAinit.dfaTransition[DFAinit.noOfTransition].initialStatesOfDFA = T;
                DFAinit.dfaTransition[DFAinit.noOfTransition].symbol = caractere[i];
                DFAinit.dfaTransition[DFAinit.noOfTransition].finalStatesOfDFA = S;
            }
        }
    }
}
int main (){

    ifstream fin("NFA2.txt");
    fin>>nrStari;

    fin>>nrCaractere;
    caractere = new char[nrCaractere];
    for(int i=1; i<=nrCaractere; i++)
        fin>>caractere[i];

    fin>>nrStariFinale;
    stariFinale = new int[nrStariFinale];
    for(int i=1; i<=nrStariFinale; i++)
        fin>>stariFinale[i];

    fin>>nrStariCuTranzitii;
    stare = new int[nrStariCuTranzitii];
    nrTr = new int[nrStariCuTranzitii];

    char c;
    int s;
    myVector.resize(nrStariCuTranzitii+1);
    for(int i=0; i<nrStariCuTranzitii; i++)
    {
        fin>>stare[i]>>nrTr[i];
        for(int j=1; j<=nrTr[i]; j++)
        {
            fin>>c>>s;
            tranzitie tr;
            tr.sim=c;
            tr.st=s;
            myVector[stare[i]].push_back(tr);

        }
    }

    conversion_from_NFA_to_DFA();

    ofstream fout("DFA.txt");
    fout<<DFAinit.noOfState<<" "<<DFAinit.noOfTransition<<'\n';

    for(int i=1;i<=DFAinit.noOfTransition;i++){
        fout<<"{ ";
        for(int j=0;j<DFAinit.dfaTransition[i].initialStatesOfDFA.size();j++)
            fout<<DFAinit.dfaTransition[i].initialStatesOfDFA.at(j)<<" ";
        fout<<" }";

        fout<<" - "<<DFAinit.dfaTransition[i].symbol<<" - ";

        fout<<"{ ";
        for(int j=0;j<DFAinit.dfaTransition[i].finalStatesOfDFA.size();j++)
            fout<<DFAinit.dfaTransition[i].finalStatesOfDFA.at(j)<<" ";
        fout<<" }";
        fout<<'\n';
    }

}
