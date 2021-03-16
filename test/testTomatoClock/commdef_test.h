#ifndef COMMDEF_TEST_H
#define COMMDEF_TEST_H

#include <QTest>

class commDef_Test : public QObject
{
    Q_OBJECT
public:
    commDef_Test();
    ~commDef_Test();
private slots:
    void packageSqlStr();
    void getCurTime();
};

#endif // COMMDEF_TEST_H
