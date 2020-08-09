#include "postings.h"
#include "utils.h"
#include <vector>
#include <iostream>
#include "ftse_env.h"
#include "buffer.h"

using namespace ftse;
using namespace std;
Postings::Postings() {
}

void Postings::encode_postings(
        const std::vector<PostingsList>& postings, Buffer* postings_e) {
    for(int i = 0; i < postings.size(); ++i) {
        int document_id = postings[i].document_id;
        int positions_count = postings[i].positions_count; 
        Buffer::append_buffer(postings_e, (void *)&document_id, sizeof(int));
        Buffer::append_buffer(postings_e, (void *)&positions_count, sizeof(int));
        for(int j = 0; j < postings[i].positions.size(); ++j) {
            int pos = postings[i].positions[j];
            Buffer::append_buffer(postings_e, (void *)&pos, sizeof(int));
        }
    }
}

void Postings::decode_postings(
        const char* postings_extend, int postings_extend_size,
        std::vector<PostingsList>* postings, int* decode_len) {
    *decode_len = 0;
    const int* p = (const int *)postings_extend;
    const int* pend = (const int *)(postings_extend+postings_extend_size);
    for(; p < pend; ++p) {
        *(decode_len++);
        PostingsList pl;
        pl.document_id = *(p++);
        pl.positions_count = *(p++);
        pl.positions.reserve(pl.positions_count);
        for(int i = 0; i < pl.positions_count; ++i) {
            pl.positions.push_back(*p);
        }
        postings->push_back(std::move(pl));
    }
}


int Postings::fetch_postings(FullTextSearchEngineEnv& ftse_env, int token_id, std::vector<PostingsList>* postings) {
    char* postings_extend;
    int docs_count, postings_extend_size;
    // 获取这个token在db中的postings(documend_id, positions_count, postings_list)
    int ret = ftse_env.db().db_get_postings(token_id, &docs_count, (void **)&postings_extend, &postings_extend_size);
    if(ret != 0 || postings_extend_size <= 0) {
        return RET_FAIL;
    }
    int decode_len;
    decode_postings(postings_extend, postings_extend_size, postings, &decode_len);
    if(decode_len != docs_count) {
        cout << "postings list decode error: stored:" <<
                docs_count << " decoded:" << decode_len << endl;
        return RET_FAIL;
    }
    return RET_SUCC;
}

void Postings::update_postings(FullTextSearchEngineEnv& ftse_env,
        InvertIndexEntry& ii) {
    std::vector<PostingsList> old_postings;
    // 从db中获取token_id的倒排列表
    if(RET_FAIL == fetch_postings(ftse_env, ii.token_id, &old_postings)) {
        return;
    }
    // merge db的倒排列表和内存中的倒排列表
    if(old_postings.size() > 0) {
        ii.postings_list = merge_postings(old_postings, ii.postings_list);
        ii.documents_count += old_postings.size(); 
        for(int i = 0; i < old_postings.size(); ++i) {
            ii.positions_count += old_postings[i].positions.size();
        }
    }
    //
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
    return std::move(result);
    //return result;
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
