#include "newitemwindow.h"
#include "ui_newitemwindow.h"

NewItemWindow::NewItemWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewItemWindow)
{
    initWindow();
    ui->setupUi(this);
}

NewItemWindow::~NewItemWindow()
{
    delete ui;
}

void NewItemWindow::rowTitlesChanged(const QString &text)
{
    rowTitlesStr_ = text.toStdString();
    checkRowTitles();
}

void NewItemWindow::columnTitlesChanged(const QString &text)
{
    colTitlesStr_ = text.toStdString();
    checkColumnTitles();
}

void NewItemWindow::setName(const QString &text)
{
    name_ = text.toStdString();
}

void NewItemWindow::selectionChanged()
{
    QString sel = selectionBox_->currentText();

    if (sel == "Dual axis map")
    {
        rows_ = 0;
        cols_ = 0;
        rowTitlesStrStatus_ = false;
        colTitlesStrStatus_ = false;
        rowTitlesEdit_->clear();
        columnTitlesEdit_->clear();
        nameInfoLabel_->setText("Map name:");
        rowLabel_->show();
        rowInfoLabel_->show();
        rowStatusLabel_->show();
        rowTitlesEdit_->show();
        rowTitlesInfoLabel_->show();
        columnInfoLabel_->show();
        columnLabel_->show();
        columnStatusLabel_->show();
        columnTitlesInfoLabel_->setText("Column titles:");
        additionalInfoLabel_->setText("Separate titles with , - end line with ;");

    }
    else if (sel == "Single axis map")
    {
        rows_ = 0;
        cols_ = 0;
        rowTitlesStrStatus_ = true;
        colTitlesStrStatus_ = false;
        rowTitlesEdit_->clear();
        columnTitlesEdit_->clear();
        nameInfoLabel_->setText("Map name:");
        rowLabel_->hide();
        rowInfoLabel_->hide();
        rowStatusLabel_->hide();
        rowTitlesInfoLabel_->hide();
        rowTitlesEdit_->hide();
        columnInfoLabel_->show();
        columnLabel_->show();
        columnStatusLabel_->show();
        columnTitlesInfoLabel_->setText("Column titles:");
        additionalInfoLabel_->setText("Separate titles with , - end line with ;");

    }
    else if (sel == "Parameter")
    {
        rows_ = 0;
        cols_ = 0;
        rowTitlesStrStatus_ = true;
        colTitlesStrStatus_ = false;
        rowTitlesEdit_->clear();
        columnTitlesEdit_->clear();
        nameInfoLabel_->setText("Param name:");
        rowLabel_->hide();
        rowInfoLabel_->hide();
        rowStatusLabel_->hide();
        rowTitlesEdit_->hide();
        rowTitlesInfoLabel_->hide();
        columnInfoLabel_->hide();
        columnLabel_->hide();
        columnStatusLabel_->hide();
        columnTitlesInfoLabel_->setText("Value:");
        additionalInfoLabel_->setText("Insert parameter value and end with ;");
    }
}

void NewItemWindow::initWindow()
{
    this->setFixedSize(600, 360);
    this->setWindowTitle("Add new...");

    selectionBox_ = QSharedPointer<QComboBox>(new QComboBox(this));
    selectionBox_->setGeometry(400, 20, 150, 40);
    selectionBox_->addItem(QString("Dual axis map"));
    selectionBox_->addItem(QString("Single axis map"));
    selectionBox_->addItem(QString("Parameter"));

    nameInfoLabel_ = QSharedPointer<QLabel>(new QLabel(this));
    nameInfoLabel_->setGeometry(100,18,80,30);

    rowLabel_ = QSharedPointer<QLabel>(new QLabel(this));
    rowLabel_->setAlignment(Qt::AlignCenter);
    rowLabel_->setGeometry(180,70,100,30);

    nameEdit_ = QSharedPointer<QLineEdit>(new QLineEdit(this));
    nameEdit_->setGeometry(180,20,200,30);
    nameEdit_->setAlignment(Qt::AlignCenter);
    nameEdit_->setReadOnly(false);

    rowInfoLabel_ = QSharedPointer<QLabel>(new QLabel(this));
    rowInfoLabel_->setGeometry(100,68,80,30);
    rowInfoLabel_->setText("Rows:");

    rowStatusLabel_ = QSharedPointer<QLabel>(new QLabel(this));
    rowStatusLabel_->setGeometry(300,70,80,30);

    columnInfoLabel_ = QSharedPointer<QLabel>(new QLabel(this));
    columnInfoLabel_->setGeometry(100,118,80,30);
    columnInfoLabel_->setText("Columns:");

    columnLabel_ = QSharedPointer<QLabel>(new QLabel(this));
    columnLabel_->setGeometry(180,120,100,30);
    columnLabel_->setAlignment(Qt::AlignCenter);

    columnStatusLabel_ = QSharedPointer<QLabel>(new QLabel(this));
    columnStatusLabel_->setGeometry(300,120,80,30);

    rowTitlesEdit_ = QSharedPointer<QLineEdit>(new QLineEdit(this));
    rowTitlesEdit_->setGeometry(180,170,300,30);
    rowTitlesEdit_->setAlignment(Qt::AlignCenter);
    rowTitlesEdit_->setReadOnly(false);

    rowTitlesInfoLabel_ = QSharedPointer<QLabel>(new QLabel(this));
    rowTitlesInfoLabel_->setGeometry(100,168,80,30);
    rowTitlesInfoLabel_->setText("Row titles:");

    columnTitlesEdit_ = QSharedPointer<QLineEdit>(new QLineEdit(this));
    columnTitlesEdit_->setGeometry(180,220,300,30);
    columnTitlesEdit_->setAlignment(Qt::AlignCenter);
    columnTitlesEdit_->setReadOnly(false);

    columnTitlesInfoLabel_ = QSharedPointer<QLabel>(new QLabel(this));
    columnTitlesInfoLabel_->setGeometry(100,218,80,30);
    columnTitlesInfoLabel_->setText("Column titles:");

    additionalInfoLabel_ = QSharedPointer<QLabel>(new QLabel(this));
    additionalInfoLabel_->setGeometry(180,250,300,30);
    additionalInfoLabel_->setText("Separate titles with , - end line with ;");

    addButton_ = QSharedPointer<QPushButton>(new QPushButton(this));
    addButton_->setGeometry(180, 300, 100, 40);
    addButton_->setText("Add");

    cancelButton_ = QSharedPointer<QPushButton>(new QPushButton(this));
    cancelButton_->setGeometry(320, 300, 100, 40);
    cancelButton_->setText("Cancel");

    connect(&*addButton_, &QPushButton::pressed, this, &NewItemWindow::addNew);
    connect(&*cancelButton_, &QPushButton::pressed, this, &NewItemWindow::closeWindow);
    connect(&*rowTitlesEdit_, &QLineEdit::textChanged, this, &NewItemWindow::rowTitlesChanged);
    connect(&*nameEdit_, &QLineEdit::textChanged, this, &NewItemWindow::setName);
    connect(&*columnTitlesEdit_, &QLineEdit::textChanged, this, &NewItemWindow::columnTitlesChanged);
    connect(&*selectionBox_, &QComboBox::currentTextChanged, this, &NewItemWindow::selectionChanged);
}

void NewItemWindow::closeWindow()
{
    this->close();
}

void NewItemWindow::checkRowTitles()
{
    char lineEnd = ';';
    char separator = ',';

    std::string data = rowTitlesStr_;

    std::vector <int> datavect;

    if (data.find(",,") != std::string::npos || data.find(",;") != std::string::npos || data.find(";,") != std::string::npos || data.find(";;") != std::string::npos)
    {
        rowTitlesStrStatus_ = false;
    }

    else
    {
        if (data.find(";") != std::string::npos)
        {
            rowStatusLabel_->setText("OK");
            rowTitlesStrStatus_ = true;
        }
        else
        {
            rowStatusLabel_->setText("");
            rowTitlesStrStatus_ = false;
        }

        for (std::size_t i = 0; i < data.size(); i++)  // insert data points to datavect
        {
            if (data.at(i) == separator || data.at(i) == lineEnd)
            {
                datavect.push_back(stoi((data.substr(0, i))));
                data = data.substr(i+1, data.length() -1);
                i = 0;
            }
        }
    }

    rows_ =  static_cast<int>(datavect.size());
    std::string rowStr = std::to_string(rows_);

    rowLabel_->setText(QString::fromStdString(rowStr));

    if (rowTitlesStrStatus_)
    {
        rowTitles_ = datavect;
    }
}

void NewItemWindow::checkColumnTitles()
{
    char lineEnd = ';';
    char separator = ',';

    std::string data = colTitlesStr_;

    std::vector <int> datavect;

    if (data.find(",,") != std::string::npos || data.find(",;") != std::string::npos || data.find(";,") != std::string::npos || data.find(";;") != std::string::npos)
    {
        colTitlesStrStatus_ = false;
    }
    else
    {

        if (data.find(";") != std::string::npos)
        {
            columnStatusLabel_->setText("OK");
            colTitlesStrStatus_ = true;
        }
        else
        {
            columnStatusLabel_->setText("");
            colTitlesStrStatus_ = false;
        }

        for (std::size_t i = 0; i < data.size(); i++)  // insert data points to datavect
        {
            if (data.at(i) == separator || data.at(i) == lineEnd)
            {
                datavect.push_back(stoi((data.substr(0, i))));
                data = data.substr(i+1, data.length() -1);
                i = 0;
            }
        }
    }

    cols_ =  static_cast<int>(datavect.size());
    std::string colStr = std::to_string(cols_);
    columnLabel_->setText(QString::fromStdString(colStr));

    if (colTitlesStrStatus_)
    {
        colTitles_ = datavect;
    }
}

void NewItemWindow::addNew()
{
    std::pair <std::string, std::vector <int>> map;
    std::pair <std::string, int> param;

    if (colTitlesStrStatus_ && rowTitlesStrStatus_)
    {

        if (rows_ > 1) // dual axis map
        {
            map.first = name_;
            map.second.push_back(rows_);
            map.second.push_back(cols_);

            for (size_t i = 0; i < rowTitles_.size(); i++)
            {
                map.second.push_back(rowTitles_.at(i));
            }
            for (size_t i = 0; i < colTitles_.size(); i++)
            {
                map.second.push_back(colTitles_.at(i));
            }

            emit newMap(map);
            clearVars();
        }

        else if (cols_ > 1) // single axis map
        {
            map.first = name_;
            map.second.push_back(1);
            map.second.push_back(cols_);

            for (size_t i = 0; i < colTitles_.size(); i++)
            {
                map.second.push_back(colTitles_.at(i));
            }
            emit newMap(map);
            clearVars();
        }

        else // parameter
        {
            param.first = name_;
            param.second = colTitles_.at(0);
            emit newParam(param);
            clearVars();

        }
         this->hide();
    }
}

void NewItemWindow::clearVars()
{
    rows_ = 0;
    cols_ = 0;
    rowTitlesStr_ = "";
    colTitlesStr_ = "";
    colTitles_.clear();
    rowTitles_.clear();
    rowTitlesStrStatus_ = false;
    colTitlesStrStatus_ = false;
    nameEdit_->clear();
    rowTitlesEdit_->clear();
    columnTitlesEdit_->clear();
}
