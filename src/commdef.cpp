#include "commdef.h"

#include <QDateTime>

QString CommDef::PackStrToSqlStr(const QString &str)
{
    return PackStrToSqlStr(str.toStdString().c_str());
}

QString CommDef::PackStrToSqlStr(const std::string &str)
{
    return PackStrToSqlStr(str.c_str());
}

QString CommDef::PackStrToSqlStr(const char *str)
{
    if (str == nullptr) {
        return "''";
    }
    return QString("%1%2%3").arg("'").arg(str).arg("'");
}

QString CommDef::GetCurrentDateTimeStr()
{
    QDateTime dtime = QDateTime::currentDateTime();
    return dtime.toString("yyyy-MM-dd HH:mm:ss");
}

QString CommDef::GetCurrentDateTimeSqlStr()
{
    return PackStrToSqlStr(GetCurrentDateTimeStr());
}

CommDef::CommDef()
{

}
