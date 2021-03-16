
#include <QTest>

#include "commdeftest.hpp"
#include "commdef_test.h"

int main(int argc, char *argv[])
{
    int status = 0;

    commDefTest tc;
    status |=  QTest::qExec(&tc, argc, argv);

    commDef_Test tc2;
    status |=  QTest::qExec(&tc2, argc, argv);

    return status;
}
