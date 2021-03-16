#ifndef COMMDEF_H
#define COMMDEF_H

#include <string>

#include <QString>

using std::string;
// 公共方法静态类型
class CommDef
{
public:
    static QString PackStrToSqlStr(const QString &str);
    static QString PackStrToSqlStr(const string &str);
    static QString PackStrToSqlStr(const char *str);
    static QString GetCurrentDateTimeStr();
    static QString GetCurrentDateTimeSqlStr();
private:
    CommDef();
};

#endif // COMMDEF_H
