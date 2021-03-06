#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QSqlDatabase>
#include <QSqlQuery>

#include <medAbstractDbController.h>
#include <medCoreLegacyExport.h>

#define EXEC_QUERY(q) medDatabaseController::instance()->execQuery(q, __FILE__ , __LINE__ )

class medAbstractData;
class medDatabaseControllerPrivate;
class medJobItemL;

/**
 * Concrete dbController implementation adhering to abstract base class
 */
class MEDCORELEGACY_EXPORT medDatabaseController: public medAbstractDbController
{
    Q_OBJECT

public:
    static medDatabaseController* instance();
    ~medDatabaseController();

    const QSqlDatabase& database() const;

    bool createConnection();
    bool  closeConnection();

    medDataIndex indexForPatient(int id);
    medDataIndex indexForStudy  (int id);
    medDataIndex indexForSeries (int id);

    medDataIndex indexForPatient (const QString &patientName);
    medDataIndex indexForStudy   (const QString &patientName, const QString &studyName);
    medDataIndex indexForSeries  (const QString &patientName, const QString &studyName,
                                  const QString &seriesName);

    QString stringForPath(const QString & name) const;

    bool moveDatabase(QString newLocation);

    bool isConnected() const;

    virtual QList<medDataIndex> patients() const;
    virtual QList<medDataIndex> studies(const medDataIndex& index ) const;
    virtual QList<medDataIndex> series(const medDataIndex& index) const;

    virtual QPixmap thumbnail(const medDataIndex& index) const;

    virtual QString metaData(const medDataIndex& index,const QString& key) const;
    virtual bool setMetaData(const medDataIndex& index, const QString& key, const QString& value);

    virtual bool isPersistent() const;

    bool execQuery(QSqlQuery& query, const char* file = nullptr, int line = -1) const;

    void addTextColumnToSeriesTableIfNeeded(QSqlQuery query, QString columnName);

public slots:

    medAbstractData *retrieve(const medDataIndex &index) const;

    void importPath(const QString& file, const QUuid& importUuid, bool indexWithoutCopying = false);
    void importData(medAbstractData *data, const QUuid & importUuid);

    virtual void remove(const medDataIndex& index);

    QList<medDataIndex> moveStudy(const medDataIndex& indexStudy, const medDataIndex& toPatient);
    medDataIndex moveSeries(const medDataIndex& indexSeries, const medDataIndex& toStudy);

    virtual int dataSourceId() const;

     bool contains(const medDataIndex &index) const;

     virtual void removeAll();

protected slots:
    void showOpeningError(QObject *sender);

private:
    medDatabaseController();

    bool createPatientTable();
    bool   createStudyTable();
    bool  createSeriesTable();

    bool updateFromNoVersionToVersion1();

    QSqlDatabase m_database;

    medDatabaseControllerPrivate * d;
    static medDatabaseController * s_instance;
};
