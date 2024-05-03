#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , datamgr_(new DataManager)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initWindow();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFile()
{
    dataUpdating_ = true;
    QString homePath = QDir::homePath();
    filePath_ = QFileDialog::getOpenFileName(this, tr("Open conf..."), homePath, tr("CFG files (*.cfg)"));
    pathLabel_->setText(filePath_);
    datamgr_.readFile(filePath_);
    QStringList list = datamgr_.giveMapList();
    mapBox_->addItems(datamgr_.giveMapList());
    paramBox_->addItems(datamgr_.giveParamList());
    writeFileButton_->show();
    mapBox_->show();
    paramBox_->show();
    changeLog_->show();
    paramEdit_->show();
    addNewMapButton_->show();
    deleteMapButton_->show();
    deleteParamButton_->show();
    openFileButton_->hide();
    paramEdit_->setReadOnly(false);
    paramEdit_->setText(QString::fromStdString(std::to_string(displayParam_.second)));
    paramEdited_ = false;

    dataUpdating_ = false;
}

void MainWindow::writeFile()
{
    QMessageBox::StandardButton reply;

    std::string msg = ("Overwrite file " + filePath_.toStdString() + "?");
    const char* question = msg.c_str();
    reply = QMessageBox::question(this, "Confirm", question, QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        datamgr_.writeFile(filePath_);
        changeLog_->clear();
        changeLog_->append("Change log:\n");
    }
}

void MainWindow::reloadFile()
{
    if (filePath_ != "")
    {
        dataUpdating_ = true;
        datamgr_.readFile(filePath_);
        displayMap_ = datamgr_.giveActiveMap();
        originalMap_ = displayMap_;
        updateMapView();
        dataUpdating_ = false;
    }
}

void MainWindow::mapSelectionChanged()
{
   dataUpdating_ = true;
   datamgr_.setActiveMap(mapBox_->currentText());
   displayMap_ = datamgr_.giveActiveMap();
   originalMap_ = displayMap_;
   updateMapView();
   dataUpdating_ = false;
}

void MainWindow::mapCellChanged(int row, int col)
{
    QPoint p = QPoint(row, col);
    series3Ddata_->setSelectedPoint(p);
}

void MainWindow::paramSelectionChanged()
{
    dataUpdating_ = true;
    datamgr_.setActiveParam(paramBox_->currentText());
    displayParam_ = datamgr_.giveActiveParam();
    std::string par = std::to_string(displayParam_.second);
    paramEdit_->setText(QString::fromStdString(par));
    paramEdited_ = false;
    dataUpdating_ = false;
}

void MainWindow::colorifyCells()
{
    int rows = mapTable_->rowCount();
    int cols = mapTable_->columnCount();

    dataUpdating_ = true;

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            int RR = 0;
            int GG = 0;
            int BB = 0;
            QVariant data = mapTable_->item(i,j)->data(Qt::EditRole);
            RR = data.toUInt() * 2 + 35;
            GG = data.toUInt() * 2 + 35;
            if (RR > 255)
            {
                RR = 255;
            }
            if (GG > 255)
            {
                GG = 255;
            }
            QBrush color (QColor(RR,GG,BB));

            mapTable_->item(i,j)->setBackground(color);
        }
    }

    dataUpdating_ = false;
}

void MainWindow::visualizeIn3D()
{
    series3Ddata_->dataProxy()->resetArray(createSurfaceDataArray());

    mapSurf_.addSeries(series3Ddata_);
    mapSurf_.setHeight(600);
    mapSurf_.setWidth(1000);
    mapSurf_.setTitle(displayMap_.first);
    mapSurf_.axisX()->setLabelFormat("%.0f");
    mapSurf_.axisY()->setLabelFormat("%.0f");
    mapSurf_.axisZ()->setLabelFormat("%.0f");
    mapSurf_.axisX()->setTitle(QStringLiteral("row"));
    mapSurf_.axisY()->setTitle(QStringLiteral("cell value"));
    mapSurf_.axisZ()->setTitle(QStringLiteral("column"));
    mapSurf_.show();
}

void MainWindow::paramChanged(const QString &text = 0)
{
    if (!dataUpdating_)
    {
        acceptParamButton_->show();
        paramEdit_->setStyleSheet("QLineEdit { background: rgb(255, 0, 0); }");
        paramEdited_ = true;
    }
}

void MainWindow::update3DValue()
{
    if (!dataUpdating_)
    {
        series3Ddata_->dataProxy()->resetArray(createSurfaceDataArray());
        mapSurf_.addSeries(series3Ddata_);
    }
}

void MainWindow::minusSelection()
{
    QList<QTableWidgetItem*> selected = mapTable_->selectedItems();

    for (int i = 0; i < selected.size(); i++)
    {
        QVariant oldVal = selected.at(i)->data(Qt::EditRole);
        int y = oldVal.toInt(); y--;
        QVariant newVal = QVariant::fromValue(y);
        selected.at(i)->setData(Qt::EditRole, newVal);
    }
}

void MainWindow::plusSelection()
{
    QList<QTableWidgetItem*> selected = mapTable_->selectedItems();

    for (int i = 0; i < selected.size(); i++)
    {
        QVariant oldVal = selected.at(i)->data(Qt::EditRole);
        int y = oldVal.toInt(); y++;
        QVariant newVal = QVariant::fromValue(y);
        selected.at(i)->setData(Qt::EditRole, newVal);
    }
}

void MainWindow::selectionMadeIn3D()
{
    QPoint p = series3Ddata_->selectedPoint();
    int row = p.x();
    int col = p.y();

    dataUpdating_ = true;
    removeMapHighlights();

    if (row != -1 && col != -1)
    {
        mapTable_->setCurrentCell(row, col);
    }
    dataUpdating_ = false;
}

void MainWindow::addNewMap()
{
    newItemWindow_->show();
}

void MainWindow::deleteCurrentMap()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm", "Remove map?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        QString d = mapBox_->currentText();
        datamgr_.deleteMap(mapBox_->currentText());
        mapBox_->clear();
        mapBox_->addItems(datamgr_.giveMapList());
        changeLog_->append("DELETED: " + d);
    }
}

void MainWindow::deleteCurrentParam()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm", "Remove parameter?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        QString d = paramBox_->currentText();
        datamgr_.deleteParam(paramBox_->currentText());
        paramBox_->clear();
        paramBox_->addItems(datamgr_.giveParamList());
        changeLog_->append("DELETED: " + d);
    }
}

void MainWindow::newMapSlot(std::pair <std::string, std::vector <int>> map)
{
    datamgr_.addNewMap(map);
    mapBox_->clear();
    mapBox_->addItems(datamgr_.giveMapList());
    changeLog_->append("ADDED: " + QString::fromStdString(map.first));
}

void MainWindow::newParamSlot(std::pair <std::string, int> param)
{
    datamgr_.addNewParam(param);
    paramBox_->clear();
    paramBox_->addItems(datamgr_.giveParamList());
    changeLog_->append("ADDED: " + QString::fromStdString(param.first));
}

void MainWindow::initWindow()
{
    this->setFixedSize(1100, 600);
    this->setWindowTitle("Config Editor");

    openFileButton_ = QSharedPointer<QPushButton>(new QPushButton(this));
    openFileButton_->setGeometry(5, 5, 100, 40);
    openFileButton_->setText("Open conf...");

    mapBox_ = QSharedPointer<QComboBox>(new QComboBox(this));
    mapBox_->setGeometry(5, 5, 150, 40);
    mapBox_->hide();

    undoButton_ = QSharedPointer<QPushButton>(new QPushButton(this));
    undoButton_->setGeometry(400, 5, 110, 40);
    undoButton_->setText("Undo map changes");
    undoButton_->hide();

    acceptButton_ = QSharedPointer<QPushButton>(new QPushButton(this));
    acceptButton_->setGeometry(520, 5, 130, 40);
    acceptButton_->setText("Accept map changes");
    acceptButton_->hide();

    writeFileButton_ = QSharedPointer<QPushButton>(new QPushButton(this));
    writeFileButton_->setGeometry(990, 555, 100, 40);
    writeFileButton_->setText("Write to file");
    writeFileButton_->hide();

    colorifyButton_ = QSharedPointer<QPushButton>(new QPushButton(this));
    colorifyButton_->setGeometry(5, 555, 120, 40);
    colorifyButton_->setText("Colorify map");

    visualizeButton_ = QSharedPointer<QPushButton>(new QPushButton(this));
    visualizeButton_->setGeometry(150, 555, 120, 40);
    visualizeButton_->setText("View in 3D");

    minusButton_ = QSharedPointer<QPushButton>(new QPushButton(this));
    minusButton_->setGeometry(360, 555, 80, 40);
    minusButton_->setText("-1 to sel");

    plusButton_ = QSharedPointer<QPushButton>(new QPushButton(this));
    plusButton_->setGeometry(440, 555, 80, 40);
    plusButton_->setText("+1 to sel");

    addNewMapButton_ = QSharedPointer<QPushButton>(new QPushButton(this));
    addNewMapButton_->setGeometry(800, 555, 80, 40);
    addNewMapButton_->setText("Add new...");
    addNewMapButton_->hide();

    deleteMapButton_ = QSharedPointer<QPushButton>(new QPushButton(this));
    deleteMapButton_->setGeometry(640, 555, 80, 40);
    deleteMapButton_->setText("Delete map");
    deleteMapButton_->hide();

    pathLabel_ = QSharedPointer<QLabel>(new QLabel(this));
    pathLabel_->setGeometry(680,5,400,40);

    changeLog_ = QSharedPointer<QTextEdit>(new QTextEdit(this));
    changeLog_->setGeometry(900,50,180,200);
    changeLog_->setAlignment((Qt::AlignLeft));
    changeLog_->setReadOnly(true);
    changeLog_->append("Change log:\n");
    changeLog_->hide();

    acceptParamButton_ = QSharedPointer<QPushButton>(new QPushButton(this));
    acceptParamButton_->setGeometry(900, 260, 150, 40);
    acceptParamButton_->setText("Accept param change");
    acceptParamButton_->hide();

    paramBox_ = QSharedPointer<QComboBox>(new QComboBox(this));
    paramBox_->setGeometry(900, 320, 150, 40);
    paramBox_->hide();

    paramEdit_ = QSharedPointer<QLineEdit>(new QLineEdit(this));
    paramEdit_->setGeometry(920,370,100,30);
    paramEdit_->setAlignment(Qt::AlignCenter);
    paramEdit_->setReadOnly(true);
    paramEdit_->hide();

    deleteParamButton_ = QSharedPointer<QPushButton>(new QPushButton(this));
    deleteParamButton_->setGeometry(900, 420, 150, 40);
    deleteParamButton_->setText("Delete parameter");
    deleteParamButton_->hide();

    mapTable_ = QSharedPointer<QTableWidget>(new QTableWidget(0,0,this));
    mapTable_->setGeometry(5, 50, 870, 500);

    mapTable_->setStyleSheet("QTableWidget {"
                             "background-color: #F0F0F0;"
                             "alternate-background-color: #808080;"
                             "selection-background-color: #393939"
                             "}"
                             "QHeaderView::section {"
                             "border-bottom: 1px solid 4181C0;"
                             "border-right: 1px solid 4181C0;"
                             "background: #F7F7F7;"
                             "color: #000000;}");

    mapSurf_.setFlags(mapSurf_.flags() ^ Qt::FramelessWindowHint);

    series3Ddata_ = new QSurface3DSeries;

    QKeySequence p = QKeySequence(Qt::CTRL + Qt::Key_Plus);
    QShortcut *ctrlPlus = new QShortcut(QKeySequence(p),this);

    QKeySequence m = QKeySequence(Qt::CTRL + Qt::Key_Minus);
    QShortcut *ctrlMinus = new QShortcut(QKeySequence(m),this);

    newItemWindow_ = QSharedPointer<NewItemWindow>(new NewItemWindow(this));

    connect(&*mapBox_, &QComboBox::currentTextChanged, this, &MainWindow::mapSelectionChanged);
    connect(&*mapBox_, QOverload<int>::of(&QComboBox::highlighted), this, &MainWindow::mapBoxOpened);
    connect(&*paramBox_, &QComboBox::currentTextChanged, this, &MainWindow::paramSelectionChanged);
    connect(&*openFileButton_, &QPushButton::pressed, this, &MainWindow::openFile);
    connect(&*undoButton_, &QPushButton::pressed, this, &MainWindow::undoMapChanges);
    connect(&*acceptButton_, &QPushButton::pressed, this, &MainWindow::acceptMapChanges);
    connect(&*acceptParamButton_, &QPushButton::pressed, this, &MainWindow::acceptParamChange);
    connect(&*mapTable_, &QTableWidget::itemChanged, this, &MainWindow::mapItemChanged);
    connect(&*mapTable_, &QTableWidget::itemChanged, this, &MainWindow::update3DValue);
    connect(&*mapTable_, &QTableWidget::currentCellChanged, this, &MainWindow::mapCellChanged);
    connect(&*writeFileButton_, &QPushButton::pressed, this, &MainWindow::writeFile);
    connect(&*colorifyButton_, &QPushButton::pressed, this, &MainWindow::colorifyCells);
    connect(&*visualizeButton_, &QPushButton::pressed, this, &MainWindow::visualizeIn3D);
    connect(&*minusButton_, &QPushButton::pressed, this, &MainWindow::minusSelection);
    connect(&*plusButton_, &QPushButton::pressed, this, &MainWindow::plusSelection);
    connect(&*addNewMapButton_, &QPushButton::pressed, this, &MainWindow::addNewMap);
    connect(&*deleteMapButton_, &QPushButton::pressed, this, &MainWindow::deleteCurrentMap);
    connect(&*deleteParamButton_, &QPushButton::pressed, this, &MainWindow::deleteCurrentParam);
    connect(&*paramEdit_, &QLineEdit::textChanged, this, &MainWindow::paramChanged);
    connect(&*paramEdit_, &QLineEdit::returnPressed, this, &MainWindow::acceptParamChange);
    connect(&*ctrlPlus, &QShortcut::activated, this, &MainWindow::plusSelection);
    connect(&*ctrlMinus, &QShortcut::activated, this, &MainWindow::minusSelection);
    connect(&mapSurf_, &Q3DSurface::selectedElementChanged, this, &MainWindow::selectionMadeIn3D);
    connect(&*newItemWindow_, &NewItemWindow::newParam, this, &MainWindow::newParamSlot);
    connect(&*newItemWindow_, &NewItemWindow::newMap, this, &MainWindow::newMapSlot);

    openFile(); //open File dialog at startup
}

void MainWindow::undoMapChanges()
{
    if (mapEdited_)
    {
        dataUpdating_ = true;        
        mapTable_->blockSignals(true);
        displayMap_ = originalMap_;

        int rows = mapTable_->rowCount();
        int cols = mapTable_->columnCount();

        int startPos = 0;
        int offset = 0;

        if (rows != 1) // if dual axis map
        {
            offset = 1;
        }
        startPos = 1 + offset + rows + cols; // start index for every map

        int row = 0;
        int col = 0;

        for (size_t i = startPos; i < displayMap_.second.size(); i++) // this for-loop actually not needed...
        {
            for (size_t j = 0; j < alteredCells_.size(); j++)
            {
                if (alteredCells_.at(j).first == row && alteredCells_.at(j).second == col)
                {
                    mapTable_->item(alteredCells_.at(j).first, alteredCells_.at(j).second)->setSelected(false); // this returns the old values for some reason?
                }
            }

            mapTable_->item(row, col)->setTextAlignment(Qt::AlignCenter);

            if (col == cols-1)
            {
                col = 0;
                row++;
            }
            else
            {
                col++;
            }
        }
        mapTable_->blockSignals(false);
        alteredCells_.clear();
        mapEdited_ = false;
        acceptButton_->hide();
        undoButton_->hide();
        visualizeIn3D(); // update 3D view
        dataUpdating_ = false;
    }

}

void MainWindow::mapItemChanged(QTableWidgetItem* item)
{
    if (!dataUpdating_)
    {
        mapTable_->blockSignals(true);
        QBrush color (QColor(250,0,0));
        item->setBackground(color);
        acceptButton_->show();
        undoButton_->show();
        mapEdited_ = true;
        alteredCells_.push_back(std::make_pair(item->row(), item->column()));
        mapTable_->blockSignals(false);
    }
}

void MainWindow::mapBoxOpened(int index)
{
    if (mapEdited_)
    {
        acceptMapChanges();
    }
}

void MainWindow::acceptMapChanges()
{
    if (mapEdited_)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Confirm", "Accept map changes?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            dataUpdating_ = true;
            changeLog_->append("MODIFIED: " + displayMap_.first);
            datamgr_.setMapChanges(displayMap_);
            removeMapHighlights();
            alteredCells_.clear();
            mapEdited_ = false;
            acceptButton_->hide();
            undoButton_->hide();
            dataUpdating_ = false;
        }
    }
}

void MainWindow::acceptParamChange()
{
    if (paramEdited_)
    {
        acceptParamButton_->hide();
        changeLog_->append(displayParam_.first + " = " + QString::fromStdString(std::to_string(displayParam_.second)));
        displayParam_.second = stoi(paramEdit_->text().toStdString());
        changeLog_->append("            -> " + QString::fromStdString(std::to_string(displayParam_.second)) +"\n");
        datamgr_.setParamChange(displayParam_);
        paramEdit_->setStyleSheet("QLineEdit { background: rgb(255, 255, 255); }");
        paramEdited_ = false;
    }
}

void MainWindow::removeMapHighlights()
{
    int rows = mapTable_->rowCount();
    int cols = mapTable_->columnCount();

    QBrush color0 (QColor(240,240,240)); // see last lines of updateMapView() for the colors
    QBrush color1 (QColor(128,128,128));

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (i % 2 == 0) // because every other line has different color
            {
                mapTable_->item(i,j)->setBackground(color0);
            }
            else
            {
                mapTable_->item(i,j)->setBackground(color1);
            }
        }
    }
}



QSurfaceDataArray* MainWindow::createSurfaceDataArray()
{
    int rows = mapTable_->rowCount();
    int cols = mapTable_->columnCount();

    QList<QSurface3DSeries *> seriesList;
    seriesList = mapSurf_.seriesList();

    for (int i = 0; i < seriesList.size(); i++) // remove previous series'
    {
        mapSurf_.removeSeries(seriesList.at(i));
    }

    QSurfaceDataArray *dataArray = new QSurfaceDataArray;

    for(int i = 0; i < rows; i++)
    {
        QSurfaceDataRow *dataRow = new QSurfaceDataRow;
        for(int j = 0; j < cols; j++)
        {
            QVariant var = mapTable_->item(i,j)->data(Qt::DisplayRole); // read cell value
            int y = var.toInt();

            QTableWidgetItem* vItem = mapTable_->verticalHeaderItem(i); // read column title
            QVariant vTitle = vItem->data(Qt::DisplayRole);
            int z = vTitle.toInt();

            QTableWidgetItem* hItem = mapTable_->horizontalHeaderItem(j); // read row title
            QVariant hTitle = hItem->data(Qt::DisplayRole);
            int x = hTitle.toInt();

            *dataRow << QVector3D(x, y, z); // add to row
        }
        *dataArray << dataRow; // add row to data
    }
    return dataArray;
}

void MainWindow::updateMapView()
{
    int rows = datamgr_.giveActiveRows();
    int cols = datamgr_.giveActiveCols();

    mapTable_->setRowCount(rows);
    mapTable_->setColumnCount(cols);

    size_t loc = 0;
    int offset = 0;

    if (rows == 1)
    {
        loc = 2 + cols;
        offset = 1;
    }
    else
    {
        loc = 2 + cols + rows;
        offset = 2;
    }

    for (int i = 0; i < rows; i++) // write row labels
    {
        mapTable_->setVerticalHeaderItem(i, &displayMap_.second.at(i+offset));
    }

    for (int i = 0; i < cols; i++) // write column labels
    {
        mapTable_->setHorizontalHeaderItem(i, &displayMap_.second.at(i+offset+rows));
    }

    for (int i = 0; i <= rows - 1; i++) // write row data
    {
        mapTable_->setRowHeight(i, cellHeigth_);
        for (int j = 0; j <= cols - 1; j++) // write column data
        {
            mapTable_->setItem(i, j, &displayMap_.second.at(loc));
            mapTable_->setColumnWidth(j, cellWidth_);
            mapTable_->item(i,j)->setTextAlignment(Qt::AlignCenter);
            loc++;
        }
    }
    mapTable_->setAlternatingRowColors(true);
    mapTable_->setShowGrid(true);
}
