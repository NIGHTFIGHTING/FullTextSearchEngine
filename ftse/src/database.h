#ifndef _FULL_TEXT_SEARCH_ENGINE_DATABASE_H_
#define _FULL_TEXT_SEARCH_ENGINE_DATABASE_H_
#include <sqlite3.h>
#include <string>

namespace ftse {
class Database {
public:
    Database(const std::string& database_path);
    int init_database(const std::string& db_path);

    void fin_database();

    int db_get_document_id(
                       const char *title, unsigned int title_size);

    int db_get_document_title(int document_id,
                          const char **const title, int *title_size);

    int db_add_document(
                    const char *title, unsigned int title_size,
                    const char *body, unsigned int body_size);

    int db_get_token_id(
                    const char *str, unsigned int str_size, int insert,
                    int *docs_count);

    int db_get_token(
                 const int token_id,
                 const char **const token, int *token_size);

    int db_get_postings(int token_id,
                    int *docs_count, void **postings, int *postings_size);

    int db_update_postings(int token_id,
                       int docs_count,
                       void *postings, int postings_size);

    int db_get_settings(const char *key,
                    int key_size,
                    const char **value, int *value_size);

    int db_replace_settings(const char *key,
                        int key_size,
                        const char *value, int value_size);

    int db_get_document_count();

    int begin();

    int commit();

    int rollback();
private:
    std::string db_path_;            /* 数据库的路径*/
    /* 与sqlite3相关的配置 */
    sqlite3 *db_; /* sqlite3的实例 */
    /* sqlite3的准备语句 */
    sqlite3_stmt *get_document_id_st_;
    sqlite3_stmt *get_document_title_st_;
    sqlite3_stmt *insert_document_st_;
    sqlite3_stmt *update_document_st_;
    sqlite3_stmt *get_token_id_st_;
    sqlite3_stmt *get_token_st_;
    sqlite3_stmt *store_token_st_;
    sqlite3_stmt *get_postings_st_;
    sqlite3_stmt *update_postings_st_;
    sqlite3_stmt *get_settings_st_;
    sqlite3_stmt *replace_settings_st_;
    sqlite3_stmt *get_document_count_st_;
    sqlite3_stmt *begin_st_;
    sqlite3_stmt *commit_st_;
    sqlite3_stmt *rollback_st_;

};
}
#endif //_FULL_TEXT_SEARCH_ENGINE_DATABASE_H_
