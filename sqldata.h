#ifndef SQLDATA_H
#define SQLDATA_H

#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <memory>
class sqlData
{
public:
    virtual ~sqlData() = default;
public:
    // 建立一个番茄，返回这个番茄的ID值
    virtual int createTomatoRecord(const QString &detail) = 0;
    // 为番茄增加一个打扰
    virtual void updateTomatoPause(int id, int t, const QString &begTime, const QString detail) = 0;
    // 修改番茄的状态，是否完成
    virtual void updateTomatoState(int id, bool isfinish) = 0;
};

std::unique_ptr<sqlData> createDbMgr();

#endif // SQLDATA_H
