#include "commdef_test.h"
#include "../../src/commdef.h"

commDef_Test::commDef_Test()
{}

commDef_Test::~commDef_Test()
{}

void commDef_Test::packageSqlStr()
{
    QString str = "test";
    QCOMPARE("'test'", CommDef::PackStrToSqlStr(str));
    QCOMPARE("'test'", CommDef::PackStrToSqlStr(str.toStdString()));
    QCOMPARE("'test'", CommDef::PackStrToSqlStr(str.toStdString().c_str()));
    QCOMPARE("''", CommDef::PackStrToSqlStr(nullptr));
}

void commDef_Test::getCurTime()
{
    QDateTime time = QDateTime::currentDateTime();
    QString timeStr = CommDef::GetCurrentDateTimeStr();
    qDebug() << timeStr;
    QDateTime time2 = QDateTime::fromString(timeStr, "yyyy-MM-dd HH:mm:ss");
    QVERIFY(time2.date() == time.date() && time.time().secsTo(time2.time()) <= 1);
}
