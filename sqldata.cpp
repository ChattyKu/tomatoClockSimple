#include "sqldata.h"
#include <QtSql/qsqlquery.h>
#include <QDateTime>
#include <QVariant>
#include <QtDebug>

const QString g_sDbFileName = "tomatoClockDB";


class sqlDataImpl : public sqlData
{
public:
    sqlDataImpl() : m_dbMgr(QSqlDatabase::addDatabase("QSQLITE")){
        m_dbMgr.setDatabaseName(g_sDbFileName);
        m_dbMgr.open();
        QSqlQuery query(m_dbMgr);
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
    QString getDataTimeStr();
private:
    QSqlDatabase m_dbMgr;
};

int sqlDataImpl::createTomatoRecord(const QString &detail)
{
    QSqlQuery query(m_dbMgr);
    QString com = QString("INSERT INTO clockTable(id, detail, cloStart) values(NULL, %1,%2)")
            .arg(QString("'").append(detail).append("'"))
            .arg(QString("'").append(getDataTimeStr()).append("'"));
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
    QSqlQuery query(m_dbMgr);
    col1 = col1 + "='" + begTime + "'";
    col2 = col2 + "='" + getDataTimeStr() + "'";
    col3 = col3 + "='" + detail + "'";
    QString comm = QString("UPDATE clockTable SET " + col1 + ", " + col2 + ", " + col3 + " WHERE id=%1").arg(id);
    qDebug() << comm;
    query.exec(comm);
}

void sqlDataImpl::updateTomatoState(int id, bool isfinish)
{
    QString comm = QString("UPDATE clockTable SET cloEnd='" + getDataTimeStr() + "', normalFin=%2 WHERE id=%1")
                        .arg(id)
                        .arg(isfinish);
    QSqlQuery query(m_dbMgr);
    query.exec(comm);
}

QString sqlDataImpl::getDataTimeStr()
{
    QDateTime dtime = QDateTime::currentDateTime();
    return dtime.toString("yyyy-MM-dd HH:mm:ss");
}



std::unique_ptr<sqlData> createDbMgr()
{
    return std::unique_ptr<sqlData>(new sqlDataImpl());
}
