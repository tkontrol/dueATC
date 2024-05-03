#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QFileDialog>
#include <QComboBox>
#include <QTableWidget>
#include <QDir>
#include <QLabel>
#include <QMessageBox>
#include <QTextEdit>
#include <QLineEdit>
#include <Q3DSurface>
#include <QtDataVisualization/Q3DSurface>
#include <QShortcut>
#include <QKeySequence>
#include "datamanager.h"
#include "newitemwindow.h"

using namespace QtDataVisualization;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void newMapSlot(std::pair <std::string, std::vector <int>> map);
    void newParamSlot(std::pair <std::string, int> param);

signals:
    void emptyMap();


private slots:
    void openFile();
    void writeFile();
    void reloadFile();
    void acceptMapChanges();
    void acceptParamChange();
    void mapSelectionChanged();
    void mapCellChanged(int row, int col);
    void mapItemChanged(QTableWidgetItem* item);
    void mapBoxOpened(int index);
    void paramSelectionChanged();
    void colorifyCells();
    void visualizeIn3D();
    void paramChanged(const QString &text);
    void update3DValue();
    void minusSelection();
    void plusSelection();
    void selectionMadeIn3D();
    void addNewMap();
    void deleteCurrentMap();
    void deleteCurrentParam();


private:
    void initWindow();
    void updateMapView();
    void undoMapChanges();
    void removeMapHighlights();

    QSurfaceDataArray* createSurfaceDataArray();

    QSharedPointer<QPushButton> openFileButton_;
    QSharedPointer<QPushButton> loadFileButton_;
    QSharedPointer<QPushButton> undoButton_;
    QSharedPointer<QPushButton> acceptButton_;
    QSharedPointer<QPushButton> acceptParamButton_;
    QSharedPointer<QPushButton> writeFileButton_;
    QSharedPointer<QPushButton> colorifyButton_;
    QSharedPointer<QPushButton> visualizeButton_;
    QSharedPointer<QPushButton> plusButton_;
    QSharedPointer<QPushButton> minusButton_;
    QSharedPointer<QPushButton> addNewMapButton_;
    QSharedPointer<QPushButton> deleteMapButton_;
    QSharedPointer<QPushButton> deleteParamButton_;
    QSharedPointer<QComboBox> mapBox_;
    QSharedPointer<QComboBox> paramBox_;
    QSharedPointer<QLabel> pathLabel_;
    QSharedPointer<QTextEdit> changeLog_;
    QSharedPointer<QLineEdit> paramEdit_;

    int cellWidth_ = 75;
    int cellHeigth_ = 35;

    bool dataUpdating_;
    bool paramEdited_;
    bool mapEdited_ = false;

    QSharedPointer<QTableWidget> mapTable_;

    std::pair<QString, std::vector<QTableWidgetItem> > displayMap_;
    std::pair<QString, std::vector<QTableWidgetItem> > originalMap_;
    std::vector<std::pair <int, int>> alteredCells_;

    std::pair <QString, int> displayParam_;

    QString filePath_;

    DataManager datamgr_;

    Q3DSurface mapSurf_;
    QSurface3DSeries *series3Ddata_;

    QSharedPointer<NewItemWindow> newItemWindow_;

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
