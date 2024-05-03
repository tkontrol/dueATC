#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QDebug>
#include <QFile>
#include <QString>
#include <QTableWidget>
#include <iostream>

class DataManager : public QObject
{
    Q_OBJECT
public:
    explicit DataManager(QObject *parent = nullptr);

    void readFile(const QString &path);
    void writeFile(const QString &path);
    QStringList giveMapList();
    QStringList giveParamList();

    void setActiveMap(QString name);
    void setActiveParam(QString name);

    std::pair <QString, std::vector <QTableWidgetItem>> giveActiveMap();
    std::pair <QString, int> giveActiveParam();

    int giveActiveRows();
    int giveActiveCols();
    void setMapChanges(std::pair <QString, std::vector <QTableWidgetItem>> map);
    void setParamChange(std::pair <QString, int> param);
    void addNewMap(std::pair<std::string, std::vector<int>> map);
    void addNewParam(std::pair <std::string, int> param);
    void deleteMap(QString mapname);
    void deleteParam(QString paramname);

signals:

private:

    void validateMapSizes();

    std::vector< std::pair <std::string, std::vector <int>>> maps_;
    std::vector< std::pair <std::string, int>> params_;

    std::pair <QString, std::vector <QTableWidgetItem>> activeMap_;
    std::pair <QString, int> activeParam_;

};

#endif // DATAMANAGER_H
