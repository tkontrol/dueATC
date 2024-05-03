#include "datamanager.h"

DataManager::DataManager(QObject *parent) : QObject(parent)
{

}

void DataManager::readFile(const QString &path)
{
    char tag = '#';
    char lineEnd = ';';
    char separator = ',';

    maps_.clear();
    params_.clear();

    //Opens a file and checks that it exists
    QFile file(path);
    bool fileFound = file.open(QIODevice::ReadOnly | QIODevice::Text);
    if(!fileFound)
    {
        qDebug() << "File not found!";
        qDebug() << path;
        return;
    }

    QString filedata = file.readAll();
    file.close();

    std::string datastr = filedata.toStdString();

    while (datastr != "EOF") // read until reach EOF at end of file
    {
        std::string id = datastr.substr(datastr.find(tag) + 1, datastr.find("\n") - 1);
        std::string data = datastr.substr(datastr.find("\n") + 1, datastr.length() - 1);
        data = data.substr(0, data.find(lineEnd) +1);

        datastr = datastr.substr(datastr.find(lineEnd) + 3, datastr.length() -1);

        std::vector <int> datavect;

        for (std::size_t i = 0; i < data.size(); i++)  // insert data points to datavect
        {
            if (data.at(i) == separator || data.at(i) == lineEnd)
            {
                datavect.push_back(stoi((data.substr(0, i))));
                data = data.substr(i+1, data.length() -1);
                i = 0;
            }
        }

        if (datavect.size() == 1) // if param
        {
            params_.push_back(std::make_pair(id, datavect.at(0)));
        }
        else // if map
        {
            maps_.push_back(std::make_pair(id, datavect));
        }
    }

   validateMapSizes();
}

void DataManager::writeFile(const QString &path)
{
    QFile file(path);
    std::string datastr;

    for (std::size_t i = 0; i < params_.size(); i++) // write first params
    {
        datastr.append("#");
        datastr.append(params_.at(i).first);
        datastr.append("\n");
        datastr.append(std::to_string(params_.at(i).second));
        datastr.append(";\n\n");
    }

    for (std::size_t i = 0; i < maps_.size(); i++) // and then maps
    {
        datastr.append("#");
        datastr.append(maps_.at(i).first);
        datastr.append("\n");

        for (std::size_t j = 0; j < maps_.at(i).second.size(); j++)
        {
           datastr.append(std::to_string(maps_.at(i).second.at(j)));

           if (j != maps_.at(i).second.size()-1)
           {
              datastr.append(","); // do not add dot at end of line
           }
        }
        datastr.append(";\n\n");
    }
    datastr.append("EOF");

    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream outStream(&file);
    outStream << QString::fromStdString(datastr);
    file.close();
}

QStringList DataManager::giveMapList()
{
    QStringList list;

    for (std::size_t i = 0; i < maps_.size(); i++)
    {
        list.push_back(QString::fromStdString(maps_.at(i).first));
    }
    list.sort();
    return list;
}

QStringList DataManager::giveParamList()
{
    QStringList list;

    for (std::size_t i = 0; i < params_.size(); i++)
    {
        list.push_back(QString::fromStdString(params_.at(i).first));
    }
    list.sort();
    return list;
}

void DataManager::setActiveMap(QString name)
{
    for (std::size_t i = 0; i < maps_.size(); i++)
    {
        if (QString::fromStdString(maps_.at(i).first) == name)
        {
            activeMap_.first = name;
            activeMap_.second.clear();

            for (std::size_t j = 0; j < maps_.at(i).second.size(); j++)
            {
                QTableWidgetItem t;
                t.setData(Qt::EditRole, maps_.at(i).second.at(j));
                activeMap_.second.push_back(t);
            }
            return;
        }
    }
}

void DataManager::setActiveParam(QString name)
{
    for (std::size_t i = 0; i < params_.size(); i++)
    {
        if (QString::fromStdString(params_.at(i).first) == name)
        {
            activeParam_.first = name;
            activeParam_.second = params_.at(i).second;
        }
    }
}

std::pair<QString, std::vector<QTableWidgetItem> > DataManager::giveActiveMap()
{
    return activeMap_;
}

std::pair<QString, int> DataManager::giveActiveParam()
{
    return activeParam_;
}

int DataManager::giveActiveRows()
{
    QVariant r = activeMap_.second.at(0).data(Qt::DisplayRole);
    return r.toInt();
}

int DataManager::giveActiveCols()
{
    QVariant c = activeMap_.second.at(1).data(Qt::DisplayRole);
    return c.toInt();
}

void DataManager::setMapChanges(std::pair<QString, std::vector<QTableWidgetItem> > map)
{
    for (std::size_t i = 0; i < maps_.size(); i++)
    {
        if (QString::fromStdString(maps_.at(i).first) == map.first)
        {
            for (std::size_t j = 0; j < map.second.size(); j++)
            {
                QVariant v = map.second.at(j).data(Qt::DisplayRole);
                maps_.at(i).second.at(j) = v.toInt();
            }
        }
    }
}

void DataManager::setParamChange(std::pair<QString, int> param)
{
    for (std::size_t i = 0; i < params_.size(); i++)
    {
        if (QString::fromStdString(params_.at(i).first) == param.first)
        {
            params_.at(i).second = param.second;
        }
    }
}

void DataManager::addNewMap(std::pair<std::string, std::vector<int> > map)
{
    maps_.push_back(map);
    validateMapSizes();

}

void DataManager::addNewParam(std::pair<std::string, int> param)
{
    params_.push_back(param);
}

void DataManager::deleteMap(QString mapname)
{
    for (size_t i = 0; i < maps_.size(); i++)
    {
        if (QString::fromStdString(maps_.at(i).first) == mapname)
        {
            maps_.erase(maps_.begin()+i);
        }
    }
}

void DataManager::deleteParam(QString paramname)
{
   for (size_t i = 0; i < params_.size(); i++)
   {
       if (QString::fromStdString(params_.at(i).first) == paramname)
       {
           params_.erase(params_.begin()+i);
       }
   }
}

void DataManager::validateMapSizes()
{
    for (std::size_t i = 0; i < maps_.size(); i++)
    {
        int rows = maps_.at(i).second.at(0);
        int cols = maps_.at(i).second.at(1);
        int properMapSize = 0;
        int offset = 0;

        if (rows != 1) // if dual axis map
        {
            offset = 1;
        }

        properMapSize = 1 + offset + rows + cols + rows*cols;
        int mapSize = static_cast<int>(maps_.at(i).second.size());

        if (mapSize < properMapSize) // if map data area is smaller than it should be by axis
        {
            int diff = properMapSize - mapSize;
            for (int j = 0; j < diff; j++)
            {
                maps_.at(i).second.push_back(0);
            }
        }
    }
}
