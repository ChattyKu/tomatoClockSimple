#include "sqldata.h"

#include <QtSql/qsqlquery.h>
#include <QDateTime>
#include <QVariant>
#include <QtDebug>
#include <QSqlRecord>
#include <QSqlError>

#include "commdef.h"

const QString g_sDbFileName = "tomatoClockDB";
QSqlDatabase g_dbMgr = QSqlDatabase::addDatabase("QSQLITE");

class sqlDataImpl : public sqlTomatoRecordData
{
public:
    sqlDataImpl(){
        g_dbMgr.setDatabaseName(g_sDbFileName);
        if (!g_dbMgr.isOpen()) {
            g_dbMgr.open();
        }

        QSqlQuery query(g_dbMgr);
        query.exec("CREATE TABLE clockTable (id INTEGER PRIMARY KEY, detail varchar NOT NULL, normalFin BOOLEAN, "
                   "cloStart DATETIME, cloEnd DATETIME, oneStart DATETIME, oneEnd DATETIME, oneDetail varchar,"
                   "twoStart DATETIME, twoEnd DATETIME, twoDetail varchar, thrStart DATETIME, thrEnd DATETIME, "
                   "thrDetail varchar)");
    }
    // sqlData interface
public:
    int createTomatoRecord(const QString &detail) override;
    void updateTomatoPause(int id, int t, const QString &begTime, const QString detail) override;
    void updateTomatoState(int id, bool isfinish) override;
private:
private:
    // QSqlDatabase m_dbMgr;
};

int sqlDataImpl::createTomatoRecord(const QString &detail)
{
    QSqlQuery query(g_dbMgr);
    QString com = QString("INSERT INTO clockTable(id, detail, cloStart) values(NULL, %1,%2)")
            .arg(CommDef::PackStrToSqlStr(detail))
            .arg(CommDef::GetCurrentDateTimeSqlStr());
    query.exec(com);
    return query.lastInsertId().toInt();
}

void sqlDataImpl::updateTomatoPause(int id, int t, const QString &begTime, const QString detail)
{
    QString col1,col2,col3;
    switch(t){
        case 1:
            col1 = "oneStart";
            col2 = "oneEnd";
            col3 = "oneDetail";
            break;
        case 2:
            col1 = "twoStart";
            col2 = "twoEnd";
            col3 = "twoDetail";
            break;
        case 3:
            col1 = "thrStart";
            col2 = "thrEnd";
            col3 = "thrDetail";
            break;
        default:
            return;
    }
    QSqlQuery query(g_dbMgr);
    col1 = col1 + "=" + CommDef::PackStrToSqlStr(begTime);
    col2 = col2 + "=" + CommDef::GetCurrentDateTimeSqlStr();
    col3 = col3 + "=" + CommDef::PackStrToSqlStr(detail);
    QString comm = QString("UPDATE clockTable SET " + col1 + ", " + col2 + ", " + col3 + " WHERE id=%1").arg(id);
    qDebug() << comm;
    query.exec(comm);
}

void sqlDataImpl::updateTomatoState(int id, bool isfinish)
{
    QString comm = QString("UPDATE clockTable SET cloEnd=" + CommDef::GetCurrentDateTimeSqlStr() + ", normalFin=%2 WHERE id=%1")
                        .arg(id)
                        .arg(isfinish);
    QSqlQuery query(g_dbMgr);
    query.exec(comm);
}

class sqlSystemConfigDataImpl : public sqlSystemConfigData
{
public:
    sqlSystemConfigDataImpl();
    ~sqlSystemConfigDataImpl() = default;

    // sqlSystemConfigData interface
public:
    void setStartPos(const QPoint &pos);
    void getStartPos(QPoint &pos);
private:

};

const QString xpos = "xPos";
const QString ypos = "yPos";

sqlSystemConfigDataImpl::sqlSystemConfigDataImpl()
{
    g_dbMgr.setDatabaseName(g_sDbFileName);
    if (!g_dbMgr.isOpen()) {
        g_dbMgr.open();
    }
    QSqlQuery query(g_dbMgr);
    query.exec("CREATE TABLE TomatoSystemConfig (name varchar NOT NULL, value varchar NOT NULL)");
}

void sqlSystemConfigDataImpl::setStartPos(const QPoint &pos)
{
    QSqlQuery query(g_dbMgr);
    query.exec("SELECT name,value FROM TomatoSystemConfig WHERE name='xpos' or name='ypos'");
    auto rec = query.record();
    QString sqlcom, sqlcom2;
    qDebug() << "record count:" << rec.count();
    if (rec.count() != 0) {
        while (query.next()) {
            qDebug() << "name:" << query.value(0).toString() << ",value:" << query.value(1).toString();
        }
    }
    if ( rec.count() == 0) {
        sqlcom = QString("INSERT INTO TomatoSystemConfig (name, value) VALUES(%1, %2)")
                        .arg(CommDef::PackStrToSqlStr("xpos"))
                        .arg(CommDef::PackStrToSqlStr(QString("%1").arg(pos.x())));
        sqlcom2 += QString("INSERT INTO TomatoSystemConfig (name, value) VALUES(%1, %2)")
                .arg(CommDef::PackStrToSqlStr("ypos"))
                .arg(CommDef::PackStrToSqlStr(QString("%1").arg(pos.y())));
    } else {
        sqlcom = QString("UPDATE TomatoSystemConfig SET value=%2 WHERE name=%1")
                        .arg(CommDef::PackStrToSqlStr("xpos"))
                        .arg(CommDef::PackStrToSqlStr(QString("%1").arg(pos.x())));
        sqlcom2 += QString("UPDATE TomatoSystemConfig SET value=%2 WHERE name=%1")
                .arg(CommDef::PackStrToSqlStr("ypos"))
                .arg(CommDef::PackStrToSqlStr(QString("%1").arg(pos.y())));
    }
    if (!query.exec(sqlcom)) {
        qDebug() << "sql comm run failed database:" << query.lastError().databaseText();
        qDebug() << "sql comm run failed driver:" << query.lastError().driverText();
    }
    if (!query.exec(sqlcom2)) {
        qDebug() << "sql comm run failed database:" << query.lastError().databaseText();
        qDebug() << "sql comm run failed driver:" << query.lastError().driverText();
    }
}

void sqlSystemConfigDataImpl::getStartPos(QPoint &pos)
{
    QSqlQuery query(g_dbMgr);
    query.exec("SELECT name,value FROM TomatoSystemConfig WHERE name='xpos' or name='ypos'");
    while (query.next()) {
        if (query.value(0).toString().compare("xpos") == 0) {
            pos.setX(query.value(1).toInt());
        } else if(query.value(0).toString().compare("ypos") == 0) {
            pos.setY(query.value(1).toInt());
        } else {
            qDebug() << "name:" << query.value(0).toString() << ",value:" << query.value(1).toString();
        }
    }
}



std::unique_ptr<sqlTomatoRecordData> createDbMgr()
{
    return std::unique_ptr<sqlTomatoRecordData>(new sqlDataImpl());
}

std::unique_ptr<sqlSystemConfigData> createSystemConfig()
{
    return std::unique_ptr<sqlSystemConfigData>(new sqlSystemConfigDataImpl());
}

