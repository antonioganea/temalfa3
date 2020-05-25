// Antonio-Alexandru Ganea // Grupa 135 // Tema LFA3

#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <algorithm>
#include <cstring>
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
                                                            //cout << "removed" << endl;
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
        bool exists = false;
        std::string lit(1,literal);
        for ( auto it = products[state].begin(); it != products[state].end(); it++ ){
            if ( *it == lit ){
                exists = true;
                                                                        //cout << "Literal " << lit << " already exists in " << state << endl;
                break;
            }
        }

        if ( exists == false ){
                                                                        //cout << "Pushed " << lit << " into " << state << endl;
            products[state].push_back(lit);
        }
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

                                                                                    //cout << "Cut an epsilon " << cutState << endl;

            std::vector<std::pair<std::string,char>> toPush;


            std::for_each(states.cbegin(), states.cend(), [this, &cutState, &toPush](std::string state) {
                for ( auto it = products[state].begin(); it != products[state].end(); it++ ){
                    //cout << "Comparing " << (*it)[1] << " " << state[0] << endl;
                    if ( (*it)[1] == cutState[0] ) {
                                                                                //cout << state << " found match " << *it << " " << cutState << endl;
                        //insertLiteralProduct(state, (*it)[0]);
                        toPush.push_back(std::pair<std::string,char>(state,(*it)[0]));
                    }
                }
            });


            std::for_each(toPush.cbegin(), toPush.cend(), [this, &cutState, &toPush](std::pair<std::string,char> mypair) {
                                                                                    //cout << mypair.first << " ----  " << mypair.second << endl;
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

long long pow( long long b, int exp ){
    long long res = 1;
    for ( int i = 0; i < exp; i++ ){
        res *= b;
    }
    return res;
}

// Suppose you have at most 64 states
unsigned long long getStatesHash( std::vector<int> states ) {
    unsigned long long hash = 0;

    for (std::vector<int>::iterator it = states.begin() ; it != states.end(); ++it){
        hash |= 1 << *it;
    }

    return hash;
}

unsigned long long getSingleStateHash( int state ){
    unsigned long long hash = 1 << state;
    return hash;
}

struct StateNumberAllocator{
private:
    int currentIndex;
public:
    //int states[1024];
    std::map< unsigned long long, int > m;
    StateNumberAllocator(){
        currentIndex = 0;
    }

    bool isHashTaken( unsigned long long h ){
        //cout << "Checking if " << h << " is taken" << endl;
        if ( m[h] ){
            //cout << "Yep" << endl;
            return true;
        }
        else{
            //cout << "Nope" << endl;
            return false;
        }

    }

    int allocateHash(unsigned long long hash) {
        if ( m[hash] ){
            //cout << "Already there" << endl;
            return m[hash];
        } else {
            m[hash] = currentIndex;
            currentIndex++;
            //cout << "Allocated!!!!!!!!!!!!!!!!!!!!!! for " << hash << "  " << m[hash] << endl;
            return m[hash];
        }
    }

};

struct Link{
    int from;
    int to;
    char by;
    bool isFinal;
    Link( int _from, int _to, char _by, bool _isFinal){
        from = _from;
        to = _to;
        by = _by;
        isFinal = _isFinal;
    }
};

bool checkIfHashContainsFinalState( Automat & automat, unsigned long long hash ) {
    unsigned long long mask;
    for ( int i = 0; i < 32; i++ ){
        mask = 1 << i;

        if ( mask & hash ){
            if ( automat.nodes[i].finalState ){
                return true;
            }
        }
    }

    return false;
}

char letters[] = "abc";

void passLetter( Automat & automat, std::vector<int> & currentStates, char currentChar ){
    std::vector<int> nextStates;

    for (std::vector<int>::iterator it = currentStates.begin() ; it != currentStates.end(); ++it){ // for all of the current states
        int currentState = *it;

        for( std::vector<int>::iterator it2 = automat.nodes[currentState].m[currentChar].begin(); it2 != automat.nodes[currentState].m[currentChar].end(); ++it2 ){
            // from currentState -> jumpingInto by letter 'currentChar'

            int jumpingInto = *it2;
            nextStates.push_back(jumpingInto);
        }
    }

    // currentStates becomes nextStates
    currentStates.clear();
    currentStates.assign(nextStates.begin(), nextStates.end());
}

Automat NFAtoDFA( Automat & automat ){
    cout << "Transforming NFA to DFA" << endl;

    //Automat newAuto(automat.node_number);

    StateNumberAllocator allocator;

    unsigned long long matrix [automat.node_number][strlen(letters)]; // states x letters matrix of hashes

    for ( int i = 0; i < automat.node_number; i++ ){
        for ( int C = 0; C < strlen(letters); C++ ) { // for currentLetter = letters[C], for every normal letter
            char currentChar = letters[C];

            std::vector<int> currentStates;
            currentStates.push_back(i);

            //cout << "Passing letter " << currentChar << " on state " << i << endl;
            passLetter(automat, currentStates, currentChar);
            //printVector(currentStates);
            //cout << endl;

            matrix[i][C] = getStatesHash(currentStates);
        }

        //cout << automat.nodes[i].finalState << " " << endl;
    }

    std::vector<unsigned long long> q;
    q.push_back(getSingleStateHash(automat.initialState));

    StateNumberAllocator cleanAlloc;
    int maxStates = 0;

    std::vector<Link> links;

    while (!q.empty())
    {
        unsigned long long currentHash = q.back();
        q.pop_back();

        for ( int C = 0; C < strlen(letters); C++ ) {
            char currentChar = letters[C];

            unsigned long long mask;
            unsigned long long composed = 0;
            for ( int i = 0; i < 32; i++ ){
                mask = 1 << i;

                if ( mask & currentHash ){
                    composed |= matrix[i][C];
                }
            }

            if ( composed != 0 && !allocator.isHashTaken(composed) ){
                q.push_back(composed);
                int allocatedNumber = allocator.allocateHash(composed);

                /*
                if ( allocator.isHashTaken(composed)){
                    cout << "DEbug" << allocator.allocateHash(composed) << endl;
                }
                */
                //cout << "Pushed back " << composed << endl;

                bool isFinalState = checkIfHashContainsFinalState(automat, composed);
                //cout << "R::Current hash " << currentHash << " jumps -> " << composed << " by letter " << currentChar << " (isfinal? " << isFinalState << ")" << endl;

                int fromNode = cleanAlloc.allocateHash(currentHash);
                int toNode = cleanAlloc.allocateHash(composed);

                if ( fromNode > maxStates )
                    maxStates = fromNode;
                if ( toNode > maxStates )
                    maxStates = toNode;
                //cout << "T::Current hash " << fromNode << " jumps -> " << toNode << " by letter " << currentChar << endl;

                Link newLink(fromNode, toNode, currentChar, isFinalState);
                links.push_back(newLink);
            }
        }
    }

    Automat newAuto(maxStates);

    newAuto.initialState = automat.initialState;
    //newAuto.nodes[i].finalState = automat.nodes[i].finalState;

    //cout << "Constructing new automat with " << maxStates << " states" << endl;

    for (std::vector<Link>::iterator it = links.begin() ; it != links.end(); ++it){
        newAuto.nodes[it->from].m[it->by].push_back(it->to);
        cout << it->from << " -> " << it->to << " by " << it->by << endl;

        cout << it->to << " is a final state ? " << it->isFinal << endl;
        newAuto.nodes[it->to].finalState = it->isFinal;
    }

    return newAuto;
}

void prettyPrintAutomat( Automat & automat ) {
    int links = 0;
    int finalStateNodes = 0;
    for ( int i = 0; i < automat.node_number; i++ ){
        Node node = automat.nodes[i];
        if ( node.finalState )
            finalStateNodes++;
        for (auto const& x : node.m){
            for ( auto const& elem : x.second ){
                links++;
            }
        }
    }

    cout << automat.node_number << endl;
    cout << links << endl;

    for ( int i = 0; i < automat.node_number; i++ ){
        Node node = automat.nodes[i];
        //cout << "Node " << i << endl;
        for (auto const& x : node.m)
        {
            //std::cout << x.first  // string (key)
                      //<< " : ";
            for ( auto const& elem : x.second ){
                //cout << elem << " ";
                cout << i << " " << elem << " " << x.first << endl;
            }
            //cout << endl;
        }
    }

    cout << automat.initialState << endl;
    cout << finalStateNodes << endl;
    for ( int i = 0; i < automat.node_number; i++ ){
        Node node = automat.nodes[i];
        if ( node.finalState ){
            cout << i << " ";
        }
    }
    cout << endl;
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
                                                        //cout << ">>>wasEpsAProductionOfS set to true" << endl;
        }
    }

    cout << endl;

    grammar.printStates();

    cout << endl;

    grammar.prettyDisplay();

    grammar.reduceEpsilons();

    cout << endl;
    grammar.prettyDisplay();

    Automat NFAautomat = createNFAfromGrammar(grammar);

    cout << endl;

    prettyPrintAutomat( NFAautomat );

    cout << endl;

    Automat DFAAuto = NFAtoDFA(NFAautomat);



    fclose(fin);

}