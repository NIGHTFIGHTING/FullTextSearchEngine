#include "utils.h"
#include <iostream>

using namespace ftse;
using namespace std;
void Utils::print_invert_index_info(InvertIndex& index, int token_id) {
    cout << "-----------begin-----------" << endl;
    cout << "token_id: " << token_id << endl;
    cout << "docs_count: " << index[token_id].documents_count << endl; 
    cout << "token: " << index[token_id].token << endl; 
    cout << "positions_count: " << index[token_id].positions_count << endl; 
    for(auto& pl : index[token_id].postings_list) {
        cout << "document_id: " << pl.document_id <<
                " positions_count: " << pl.positions_count << endl; 
        for(auto& pos : pl.positions) {
            cout << "pos: " << pos << " ";
        }
        cout << endl;
    }
    cout << endl << "-----------end-----------" << endl;
}

void Utils::print_invert_index_info(InvertIndex& index) {
    cout << "-----------doucument begin-----------" << endl;
    for(auto& ii : index) { 
        int token_id = ii.first;
        cout << "token_id: " << token_id << endl;
        cout << "docs_count: " << index[token_id].documents_count << endl; 
        cout << "token: " << index[token_id].token << endl; 
        cout << "positions_count: " << index[token_id].positions_count << endl; 
        for(auto& pl : index[token_id].postings_list) {
            cout << "document_id: " << pl.document_id <<
                    " positions_count: " << pl.positions_count << endl; 
            for(auto& pos : pl.positions) {
                cout << "pos: " << pos << " ";
            }
            cout << endl;
        }
    }
    cout << endl << "-----------document end-----------" << endl;
}
