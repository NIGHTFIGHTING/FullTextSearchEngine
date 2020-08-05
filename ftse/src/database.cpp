#include "database.h"
#include <string>
#include "encoding.h" 

using namespace ftse;
/**
 * 初始化数据库
 * @param[in] env 存储着应用程序运行环境的结构体
 * @param[in] db_path 待初始化的数据库文件的名字
 * @return sqlite3的错误代码
 * @retval 0 成功
 */

Database::Database(const std::string& database_path) {
    init_database(database_path);
}

int Database::init_database(const std::string& db_path)
{
    int rc;
    if ((rc = sqlite3_open(db_path.c_str(), &db_)))
    {
        Encoding::print_error("cannot open databases.");
        return rc;
    }

    sqlite3_exec(db_,
                 "CREATE TABLE settings (" \
               "  key   TEXT PRIMARY KEY," \
               "  value TEXT" \
               ");",
                 NULL, NULL, NULL);

    sqlite3_exec(db_,
                 "CREATE TABLE documents (" \
               "  id      INTEGER PRIMARY KEY," /* auto increment */ \
               "  title   TEXT NOT NULL," \
               "  body    TEXT NOT NULL" \
               ");",
                 NULL, NULL, NULL);

    sqlite3_exec(db_,
                 "CREATE TABLE tokens (" \
               "  id         INTEGER PRIMARY KEY," \
               "  token      TEXT NOT NULL," \
               "  docs_count INT NOT NULL," \
               "  postings   BLOB NOT NULL" \
               ");",
                 NULL, NULL, NULL);

    sqlite3_exec(db_,
                 "CREATE UNIQUE INDEX token_index ON tokens(token);",
                 NULL, NULL, NULL);

    sqlite3_exec(db_,
                 "CREATE UNIQUE INDEX title_index ON documents(title);",
                 NULL, NULL, NULL);

    sqlite3_prepare(db_,
                    "SELECT id FROM documents WHERE title = ?;",
                    -1, &get_document_id_st_, NULL);
    sqlite3_prepare(db_,
                    "SELECT title FROM documents WHERE id = ?;",
                    -1, &get_document_title_st_, NULL);
    sqlite3_prepare(db_,
                    "INSERT INTO documents (title, body) VALUES (?, ?);",
                    -1, &insert_document_st_, NULL);
    sqlite3_prepare(db_,
                    "UPDATE documents set body = ? WHERE id = ?;",
                    -1, &update_document_st_, NULL);
    sqlite3_prepare(db_,
                    "SELECT id, docs_count FROM tokens WHERE token = ?;",
                    -1, &get_token_id_st_, NULL);
    sqlite3_prepare(db_,
                    "SELECT token FROM tokens WHERE id = ?;",
                    -1, &get_token_st_, NULL);
    sqlite3_prepare(db_,
                    "INSERT OR IGNORE INTO tokens (token, docs_count, postings)"
                            " VALUES (?, 0, ?);",
                    -1, &store_token_st_, NULL);
    sqlite3_prepare(db_,
                    "SELECT docs_count, postings FROM tokens WHERE id = ?;",
                    -1, &get_postings_st_, NULL);
    sqlite3_prepare(db_,
                    "UPDATE tokens SET docs_count = ?, postings = ? WHERE id = ?;",
                    -1, &update_postings_st_, NULL);
    sqlite3_prepare(db_,
                    "SELECT value FROM settings WHERE key = ?;",
                    -1, &get_settings_st_, NULL);
    sqlite3_prepare(db_,
                    "INSERT OR REPLACE INTO settings (key, value) VALUES (?, ?);",
                    -1, &replace_settings_st_, NULL);
    sqlite3_prepare(db_,
                    "SELECT COUNT(*) FROM documents;",
                    -1, &get_document_count_st_, NULL);
    sqlite3_prepare(db_,
                    "BEGIN;",
                    -1, &begin_st_, NULL);
    sqlite3_prepare(db_,
                    "COMMIT;",
                    -1, &commit_st_, NULL);
    sqlite3_prepare(db_,
                    "ROLLBACK;",
                    -1, &rollback_st_, NULL);
    return 0;
}

/**
 * 关闭数据库
 * @param[in] env 存储着应用程序运行环境的结构体
 */
void Database::fin_database()
{
    sqlite3_finalize(get_document_id_st_);
    sqlite3_finalize(get_document_title_st_);
    sqlite3_finalize(insert_document_st_);
    sqlite3_finalize(update_document_st_);
    sqlite3_finalize(get_token_id_st_);
    sqlite3_finalize(get_token_st_);
    sqlite3_finalize(store_token_st_);
    sqlite3_finalize(get_postings_st_);
    sqlite3_finalize(update_postings_st_);
    sqlite3_finalize(get_settings_st_);
    sqlite3_finalize(replace_settings_st_);
    sqlite3_finalize(get_document_count_st_);
    sqlite3_finalize(begin_st_);
    sqlite3_finalize(commit_st_);
    sqlite3_finalize(rollback_st_);
    sqlite3_close(db_);
}

/**
 * 根据指定的文档标题获取文档编号
 * @param[in] env 存储着应用程序运行环境的结构体
 * @param[in] title 文档标题
 * @param[in] title_size 文档标题的字节数
 * @return 文档编号
 */
int Database::db_get_document_id(
                   const char *title, unsigned int title_size)
{
    int rc;
    sqlite3_reset(get_document_id_st_);
    sqlite3_bind_text(get_document_id_st_, 1,
                      title, title_size, SQLITE_STATIC);
    rc = sqlite3_step(get_document_id_st_);
    if (rc == SQLITE_ROW)
    {
        return sqlite3_column_int(get_document_id_st_, 0);
    }
    else
    {
        return 0;
    }
}

/**
 * 根据指定的文档编号获取文档标题
 * @param[in] env 存储着应用程序运行环境的结构体
 * @param[in] document_id 文档编号
 * @param[out] title 文档标题
 * @param[out] title_size 文档标题的字节数
 */
int Database::db_get_document_title(int document_id,
                      const char **title, int *title_size)
{
    int rc;

    sqlite3_reset(get_document_title_st_);
    sqlite3_bind_int(get_document_title_st_, 1, document_id);

    rc = sqlite3_step(get_document_title_st_);
    if (rc == SQLITE_ROW)
    {
        if (title)
        {
            *title = (const char *) sqlite3_column_text(get_document_title_st_,
                                                        0);
        }
        if (title_size)
        {
            *title_size = (int) sqlite3_column_bytes(get_document_title_st_,
                                                     0);
        }
    }
    return 0;
}

/**
 * 将文档添加到documents表中
 * @param[in] env 存储着应用程序运行环境的结构体
 * @param[in] title 文档标题
 * @param[in] title_size 文档标题的字节数
 * @param[in] body 文档正文
 * @param[in] body_size 文档正文的字节数
 */
int Database::db_add_document(
                const char *title, unsigned int title_size,
                const char *body, unsigned int body_size)
{
    sqlite3_stmt *st;
    int rc, document_id;

    if ((document_id = db_get_document_id(title, title_size)))
    {
        st = update_document_st_;
        sqlite3_reset(st);
        sqlite3_bind_text(st, 1, body, body_size, SQLITE_STATIC);
        sqlite3_bind_int(st, 2, document_id);
    }
    else
    {
        st = insert_document_st_;
        sqlite3_reset(st);
        sqlite3_bind_text(st, 1, title, title_size, SQLITE_STATIC);
        sqlite3_bind_text(st, 2, body, body_size, SQLITE_STATIC);
    }
    query:
    rc = sqlite3_step(st);
    switch (rc)
    {
        case SQLITE_BUSY:
            goto query;
        case SQLITE_ERROR:
            Encoding::print_error("ERROR: %s", sqlite3_errmsg(db_));
            break;
        case SQLITE_MISUSE:
            Encoding::print_error("MISUSE: %s", sqlite3_errmsg(db_));
            break;
    }
    return rc;
}

/**
 * 从tokens表中获取指定词元的编号
 * @param[in] env 存储着应用程序运行环境的结构体
 * @param[in] str 词元（UTF-8）
 * @param[in] str_size 词元的字节数
 * @param[in] insert 当找不到指定词元时，是否要将该词元添加到表中
 * @param[out] docs_count 出现过指定词元的文档数
 */
int Database::db_get_token_id(
                const char *str, unsigned int str_size, int insert,
                int *docs_count)
{
    int rc;
    if (insert)
    {
        sqlite3_reset(store_token_st_);
        sqlite3_bind_text(store_token_st_, 1, str, str_size,
                          SQLITE_STATIC);
        sqlite3_bind_blob(store_token_st_, 2, "", 0, SQLITE_STATIC);
        rc = sqlite3_step(store_token_st_);
    }
    sqlite3_reset(get_token_id_st_);
    sqlite3_bind_text(get_token_id_st_, 1, str, str_size,
                      SQLITE_STATIC);
    rc = sqlite3_step(get_token_id_st_);
    if (rc == SQLITE_ROW)
    {
        if (docs_count)
        {
            *docs_count = sqlite3_column_int(get_token_id_st_, 1);
        }
        return sqlite3_column_int(get_token_id_st_, 0);
    }
    else
    {
        if (docs_count)
        {
            *docs_count = 0;
        }
        return 0;
    }
}

/**
 * 根据词元编号从tokens表获取词元
 * @param[in] env 存储着应用程序运行环境的结构体
 * @param[in] token_id 词元编号
 * @param[out] token 词元（UTF-8）
 * @param[out] token_size 词元的字节数
 */
int Database::db_get_token(
             const int token_id,
             const char **const token, int *token_size)
{
    int rc;
    sqlite3_reset(get_token_st_);
    sqlite3_bind_int(get_token_st_, 1, token_id);
    rc = sqlite3_step(get_token_st_);
    if (rc == SQLITE_ROW)
    {
        if (token)
        {
            *token = (const char *) sqlite3_column_text(get_token_st_, 0);
        }
        if (token_size)
        {
            *token_size = (int) sqlite3_column_bytes(get_token_st_, 0);
        }
    }
    return 0;
}

/**
 * 根据词元编号从数据库中获取倒排列表
 * @param[in] env 存储着应用程序运行环境的结构体
 * @param[in] token_id 词元编号
 * @param[out] docs_count 倒排列表中的文档数
 * @param[out] postings 获取到的倒排列表
 * @param[out] postings_size 获取到的倒排列表的字节数
 */
int Database::db_get_postings(int token_id,
                int *docs_count, void **postings, int *postings_size)
{
    int rc;
    sqlite3_reset(get_postings_st_);
    sqlite3_bind_int(get_postings_st_, 1, token_id);
    rc = sqlite3_step(get_postings_st_);
    if (rc == SQLITE_ROW)
    {
        if (docs_count)
        {
            *docs_count = sqlite3_column_int(get_postings_st_, 0);
        }
        if (postings)
        {
            *postings = (void *) sqlite3_column_blob(get_postings_st_, 1);
        }
        if (postings_size)
        {
            *postings_size = (int) sqlite3_column_bytes(get_postings_st_, 1);
        }
        rc = 0;
    }
    else
    {
        if (docs_count) { *docs_count = 0; }
        if (postings) { *postings = NULL; }
        if (postings_size) { *postings_size = 0; }
        if (rc == SQLITE_DONE) { rc = 0; } /* no record found */
    }
    return rc;
}

/**
 * 将倒排列表存储到数据库中
 * @param[in] env 存储着应用程序运行环境的结构体
 * @param[in] token_id 词元编号
 * @param[in] docs_count 倒排列表中的文档数
 * @param[in] postings 待存储的倒排列表
 * @param[in] postings_size 倒排列表的字节数
 */
int Database::db_update_postings(int token_id, int docs_count,
                   void *postings, int postings_size)
{
    int rc;
    sqlite3_reset(update_postings_st_);
    sqlite3_bind_int(update_postings_st_, 1, docs_count);
    sqlite3_bind_blob(update_postings_st_, 2, postings,
                      (unsigned int) postings_size, SQLITE_STATIC);
    sqlite3_bind_int(update_postings_st_, 3, token_id);
    query:
    rc = sqlite3_step(update_postings_st_);

    switch (rc)
    {
        case SQLITE_BUSY:
            goto query;
        case SQLITE_ERROR:
            Encoding::print_error("ERROR: %s", sqlite3_errmsg(db_));
            break;
        case SQLITE_MISUSE:
            Encoding::print_error("MISUSE: %s", sqlite3_errmsg(db_));
            break;
    }
    return rc;
}

/**
 * 从数据库中获取配置信息
 * @param[in] env 存储着应用程序运行环境的结构体
 * @param[in] key 配置项的名称
 * @param[in] key_size 配置项名称的字节数
 * @param[out] value 配置项的取值
 * @param[out] value_size 配置项取值的字节数
 */
int Database::db_get_settings(const char *key, int key_size,
                const char **value, int *value_size)
{
    int rc;

    sqlite3_reset(get_settings_st_);
    sqlite3_bind_text(get_settings_st_, 1,
                      key, key_size, SQLITE_STATIC);
    rc = sqlite3_step(get_settings_st_);
    if (rc == SQLITE_ROW)
    {
        if (value)
        {
            *value = (const char *) sqlite3_column_text(get_settings_st_, 0);
        }
        if (value_size)
        {
            *value_size = (int) sqlite3_column_bytes(get_settings_st_, 0);
        }
    }
    return 0;
}

/**
 * 更新存储在数据库中的配置信息
 * @param[in] env 存储着应用程序运行环境的结构体
 * @param[in] key 配置项的名称
 * @param[in] key_size 配置项名称的字节数
 * @param[in] value 配置项的取值
 * @param[in] value_size 配置项取值的字节数
 */
int Database::db_replace_settings(const char *key,
                    int key_size,
                    const char *value, int value_size)
{
    int rc;
    sqlite3_reset(replace_settings_st_);
    sqlite3_bind_text(replace_settings_st_, 1,
                      key, key_size, SQLITE_STATIC);
    sqlite3_bind_text(replace_settings_st_, 2,
                      value, value_size, SQLITE_STATIC);
    query:
    rc = sqlite3_step(replace_settings_st_);

    switch (rc)
    {
        case SQLITE_BUSY:
            goto query;
        case SQLITE_ERROR:
            Encoding::print_error("ERROR: %s", sqlite3_errmsg(db_));
            break;
        case SQLITE_MISUSE:
            Encoding::print_error("MISUSE: %s", sqlite3_errmsg(db_));
            break;
    }
    return rc;
}

/**
 * 获取已添加到数据库中的文档数
 * @param[in] env 存储着应用程序运行环境的结构体
 */
int Database::db_get_document_count()
{
    int rc;

    sqlite3_reset(get_document_count_st_);
    rc = sqlite3_step(get_document_count_st_);
    if (rc == SQLITE_ROW)
    {
        return sqlite3_column_int(get_document_count_st_, 0);
    }
    else
    {
        return -1;
    }
}

/**
 * 开启事务
 * @param[in] env 存储着应用程序运行环境的结构体
 */
int Database::begin()
{
    return sqlite3_step(begin_st_);
}

/**
 * 提交事务
 * @param[in] env 存储着应用程序运行环境的结构体
 */
int Database::commit()
{
    return sqlite3_step(commit_st_);
}

/**
 * 回滚事务
 * @param[in] env 存储着应用程序运行环境的结构体
 */
int Database::rollback()
{
    return sqlite3_step(rollback_st_);
}
