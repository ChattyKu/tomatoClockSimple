#ifndef COMMDEFTEST_HPP
#define COMMDEFTEST_HPP

#include <QtTest>

class commDefTest : public QObject
{
    Q_OBJECT

public:
    commDefTest();
    ~commDefTest();

private slots:
    void test_case1();

};
#endif // COMMDEFTEST_HPP
