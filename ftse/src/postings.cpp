#include "postings.h"
#include "utils.h"
#include <vector>
#include <iostream>

using namespace ftse;
using namespace std;
Postings::Postings() {
}

// 将二者连接成按文档编号升序排列
std::vector<PostingsList> Postings::merge_postings(std::vector<PostingsList>& pa,
        std::vector<PostingsList>& pb) {
    cout << "pa.size: " << pa.size() << " pb.size: " << pb.size() << endl; 
    std::vector<PostingsList> result;
    result.reserve(pa.size() + pb.size());
    if(pa.size() == 0) {
        return std::move(pb);
    }
    auto pla = pa.begin();
    auto plb = pb.begin();
    while(pla != pa.end() && plb != pb.end()) {
        if(pla->document_id == plb->document_id) {
            result.push_back(std::move(*pla));
            ++pla;
            ++plb;
        } else if (pla->document_id < plb->document_id) {
            result.push_back(std::move(*pla));
            ++pla;
        } else {
            ++plb;
        }
    }
    while(pla != pa.end()) {
        result.push_back(std::move(*pla));
        ++pla;
    }
    while(plb != pb.end()) {
        result.push_back(std::move(*plb));
        ++plb;
    }
    cout << "print_merge_positions-------------begin---------" << endl;
    cout << "result.size: " << result.size() << endl; 
    for(auto& pl : result) {
        cout << "document_id: " << pl.document_id <<
                " positions_count: " << pl.positions_count << endl; 
        for(auto& pos : pl.positions) {
            cout << "pos: " << pos << " ";
        }
        cout << endl;
    }
    cout << endl << "print_merge_positions-------------end---------" << endl;
    //return std::move(result);
    return result;
}
// ii -> invert_index
void Postings::merge_invert_index(InvertIndex& base_ii, InvertIndex& to_be_added_ii) {
    for(auto& ii : to_be_added_ii) {
        if(base_ii.find(ii.first) != base_ii.end()) {
            cout << "first: " << ii.first << " " << base_ii[ii.first].token << endl;
            base_ii[ii.first].postings_list = 
                    merge_postings(base_ii[ii.first].postings_list, ii.second.postings_list);
            base_ii[ii.first].documents_count += ii.second.documents_count;
            base_ii[ii.first].positions_count += ii.second.positions_count;
        } else {
            cout << "第一次出现token: " << ii.first << " " << ii.second.token <<endl;
            //base_ii[ii.first] = std::move(ii.second);
            //Utils::print_invert_index_info(to_be_added_ii, ii.first);
            base_ii[ii.first] = ii.second;
            Utils::print_invert_index_info(base_ii, ii.first);
        }
    }
}