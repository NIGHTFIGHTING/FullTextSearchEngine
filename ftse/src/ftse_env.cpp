#include "ftse_env.h"
#include "encoding.h"
#include "token.h"

using namespace ftse;
FullTextSearchEngineEnv::FullTextSearchEngineEnv(int token_len):db_("./ftse.db"),
        token_len_(token_len) {
}

Database& FullTextSearchEngineEnv::db() {
    return db_;
}
InvertIndex& FullTextSearchEngineEnv::get_invert_index() {
    return invert_index_;
}
void FullTextSearchEngineEnv::add_document(const char *title,
        const char *body) {
    if(!title || !body) {
        return;
    }
    unsigned int title_len = strlen(title);
    unsigned int body_len = strlen(body);
    /* 将文档存储到数据库中并获取该文档对应的文档编号 */
    db_.db_add_document(title, title_len, body, body_len);
    int document_id = db_.db_get_document_id(title, title_len); 
    UTF32Char* body32; 
    int body32_len;
    if(1 == Encoding::utf8toutf32(body, body_len, &body32, &body32_len)) {
        return;
    }
    Token::text_to_postings_lists(*this, document_id, body32, body32_len, token_len_);
}