#pragma once

#include <sese/db/PreparedStatement.h>
#include <maria/MariaStmtResultSetImpl.h>

namespace sese::db::impl {

    class SESE_DB_API MariaPreparedStatementImpl final : public PreparedStatement {
    public:
        explicit MariaPreparedStatementImpl(MYSQL_STMT *stmt, MYSQL_RES *meta, size_t count) noexcept;
        ~MariaPreparedStatementImpl() noexcept override;

        ResultSet::Ptr executeQuery() noexcept override;
        int64_t executeUpdate() noexcept override;

        bool setDouble(uint32_t index, double &value) noexcept override;
        bool setFloat(uint32_t index, float &value) noexcept override;
        bool setInteger(uint32_t index, int32_t &value) noexcept override;
        bool setLong(uint32_t index, int64_t &value) noexcept override;
        bool setText(uint32_t index, const char *value) noexcept override;
        bool setNull(uint32_t index) noexcept override;

        bool getColumnType(uint32_t index, MetadataType &type) noexcept override;
        int64_t getColumnSize(uint32_t index) noexcept override;

        [[nodiscard]] int getLastError() const noexcept override;
        [[nodiscard]] const char *getLastErrorMessage() const noexcept override;

        static bool mallocBindStruct(MYSQL_RES *res, MYSQL_BIND **bind) noexcept;
        static void freeBindStruct(MYSQL_BIND *bind, size_t count) noexcept;

    protected:
        MYSQL_STMT *stmt;
        size_t count = 0;
        MYSQL_BIND *param;
        MYSQL_RES *meta;
    };

}// namespace sese::db