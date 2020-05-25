// Antonio-Alexandru Ganea // Grupa 135 // Tema LFA3

#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <algorithm>
using namespace std;

#define EPSILON "eps"

class Grammar{

    public:
    std::map<std::string, std::vector<std::string>> products;
    std::set<std::string> states;
    bool wasEpsAProductionOfS = false;
    void setStateUsed(std::string state) {
        //auto search = states.find(state);
        //if (search == states.end()) {
            states.insert(state);
        //}
    }
    bool stateExist(std::string state){
        auto search = states.find(state);
        if (search != states.end()) {
            return true;
        } else {
            return false;
        }
    }
    void printStates(){
        std::for_each(states.cbegin(), states.cend(), [](std::string x) {
            cout << x << ' ';
        });
        cout << endl;
    }
    void addProduct(std::string state, std::string product){
        products[state.c_str()].push_back(product);
        // cout << "Size " << state << " " << products[state.c_str()].size() << endl;
        setStateUsed(state);
    }

    void prettyDisplay(){
        std::for_each(states.cbegin(), states.cend(), [this](std::string state) {
            cout << state << " -> ";
            std::for_each(products[state].cbegin(), products[state].cend(), [](std::string product) {
                cout << product << " ";
            });
            cout << endl;
        });
        cout << endl;
    }

    bool cutEpsilon(std::string & outState) {
        bool out = false;
        std::for_each(states.cbegin(), states.cend(), [this, &outState, &out](std::string state) {
            //cout << state << " -> ";

            if ( out ){
                return;
            }

            for ( auto it = products[state].begin(); it != products[state].end(); ){
                if ( *it == EPSILON )
                {
                    outState = state;
                    it = products[state].erase(it);
                    cout << "removed" << endl;
                    out = true;
                    return;
                } else {
                    ++it;
                }
            }


/*
            std::for_each(products[state].cbegin(), products[state].cend(), [this](std::string product) {
                if ( product == EPSILON )
                {
                    products[state].remove
                }
            });
*/
/*
            products[state].erase(std::remove_if(products[state].begin(), products[state].end(),
                       [](std::string product) { return product == EPSILON; }), products[state].end());
*/
            //cout << endl;
        });
        return out;
    }

    void insertLiteralProduct(std::string state, char literal){
        cout << "HEY"<<endl << flush;
        bool exists = false;
        std::string lit(1,literal);
        for ( auto it = products[state].begin(); it != products[state].end(); it++ ){
            if ( *it == lit ){
                exists = true;
                cout << "Literal " << lit << " already exists in " << state << endl;
                break;
            }
        }

        if ( exists == false ){
            cout << "Pushed " << lit << " into " << state << endl;
            products[state].push_back(lit);
        }
        cout << "HEY2"<<endl << flush;
    }

    void createS1fromS(){
        //cout << "parsing S1" << endl;
        std::string Sstate = "S";
        std::string S1state = "S1";
        std::string eps = EPSILON;

        for ( auto it = products[Sstate].begin(); it != products[Sstate].end(); it++ ){
            //cout << *it << endl;
            addProduct(S1state, *it);
        }

        if ( wasEpsAProductionOfS ){
            addProduct(S1state,eps);
        }
    }

    void reduceEpsilons(){

        while(true){
            std::string cutState;
            bool worked = cutEpsilon(cutState);
            if ( !worked ){
                break;
            }

            cout << "Cut an epsilon " << cutState << endl;

            std::vector<std::pair<std::string,char>> toPush;


            std::for_each(states.cbegin(), states.cend(), [this, &cutState, &toPush](std::string state) {
                for ( auto it = products[state].begin(); it != products[state].end(); it++ ){
                    //cout << "Comparing " << (*it)[1] << " " << state[0] << endl;
                    if ( (*it)[1] == cutState[0] ) {
                        cout << state << " found match " << *it << " " << cutState << endl;
                        //insertLiteralProduct(state, (*it)[0]);
                        toPush.push_back(std::pair<std::string,char>(state,(*it)[0]));
                    }
                }
            });


            std::for_each(toPush.cbegin(), toPush.cend(), [this, &cutState, &toPush](std::pair<std::string,char> mypair) {
                cout << mypair.first << " ----  " << mypair.second << endl;
                insertLiteralProduct( mypair.first, mypair.second);
            });


        }

        cout << endl;
        createS1fromS();


    }
};


struct Node{
    std::map< char, std::vector<int> > m;
    bool finalState;
    Node(){
        finalState = false;
    }
};

struct Automat{
    int initialState;
    Node* nodes;
    int node_number;
    char* characters;

    Automat(int states){
        nodes = new Node[states+1];
        node_number = states;

        characters = new char[256];
    }

    /*
    ~Automat() {
        //delete nodes;
        //delete characters;
    }
    */
};

Automat createNFAfromGrammar( Grammar & grammar ){

    // count grammar states
    int stateNo = grammar.states.size() + 1;

    cout << "Creating automata with " << stateNo << "( an extra one ) states" << endl;

    Automat automat(stateNo);

    std::map<std::string,int> codes;
    int idx = 0;
    int S1_id;
    std::for_each(grammar.states.cbegin(), grammar.states.cend(), [&codes, &idx, &S1_id](std::string state) {
        codes[state] = idx;
        cout << state << " has code " << idx << endl;
        if (state=="S1"){
            S1_id = idx;
        }
        idx++;
    });

    codes["D0"] = idx;
    cout << "D0 has code " << idx << endl;

    int D0_id = idx;

    for ( auto sit = grammar.states.begin(); sit != grammar.states.end(); sit++ ){
        std::string state = *sit;
        for ( auto it = grammar.products[state].begin(); it != grammar.products[state].end(); it++ ){
            //cout << *it << endl;
            //addProduct(S1state, *it);
                                        //cout << *sit << " ----> " << *it << " "  /*<<static_cast<int> ( (*it)[1] ) */<< endl;
            char destinationStateChar = (*it)[1];
            if ( (*it)[1] != 0 && destinationStateChar != EPSILON[1] ){
                cout << *sit << " links to " << destinationStateChar << " by " << (*it)[0] << endl;
                // a -> b through c;
                std::string destination(1, destinationStateChar);
                int aCode = codes[*sit];
                int bCode = codes[destination];
                char cChar = (*it)[0];
                automat.nodes[aCode].m[cChar].push_back(bCode);
            }
            if ( (*it)[1] == 0 ){
                cout << *sit << " links to end (D0)" << " by " << (*it)[0] << endl;
                int aCode = codes[*sit];
                //int bCode = codes[destination];
                char cChar = (*it)[0];
                automat.nodes[aCode].m[cChar].push_back(D0_id);
            }
        }
    }

    automat.initialState = S1_id;
    if ( grammar.wasEpsAProductionOfS ){
        automat.nodes[S1_id].finalState = true;
    }
    automat.nodes[D0_id].finalState = true;


    return automat;

    /*
        int a,b;
        char temp[100];
        fscanf(fin,"%d%d%s",&a,&b,&temp);
        char c = temp[0];
    */
    //automat.nodes[a].m[c].push_back(b);


}


FILE * fin;

int main(){
    Grammar grammar;
    fin = fopen("input.txt", "r");
    int temp;
    fscanf(fin, "%d", &temp);

    char buffer[256];
    for ( int i = 0; i < temp; i++ ){
        fscanf(fin,"%s",&buffer);
        std::string state = buffer;
        fscanf(fin,"%s",&buffer);
        std::string product = buffer;

        cout << state << " -> " << product << endl;
        grammar.addProduct(state, product);
        if ( state == "S" && product == "eps" ){
            grammar.wasEpsAProductionOfS = true;
            cout << ">>>wasEpsAProductionOfS set to true" << endl;
        }
    }

    cout << endl;

    grammar.printStates();

        cout << endl;

    grammar.prettyDisplay();

    grammar.reduceEpsilons();

    cout << endl;
    grammar.prettyDisplay();

    Automat automat = createNFAfromGrammar(grammar);



    fclose(fin);

}