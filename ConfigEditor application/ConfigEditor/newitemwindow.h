#ifndef NewItemWindow_H
#define NewItemWindow_H

#include <QDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QString>
#include <QComboBox>
#include <QDebug>

namespace Ui {
class NewItemWindow;
}

class NewItemWindow : public QDialog
{
    Q_OBJECT

public:
    explicit NewItemWindow(QWidget *parent = nullptr);
    ~NewItemWindow();

signals:
    void newMap(std::pair <std::string, std::vector <int>> map);
    void newParam(std::pair <std::string, int> param);



private slots:
    void rowTitlesChanged(const QString &text);
    void columnTitlesChanged(const QString &text);
    void setName(const QString &text);
    void selectionChanged();

private:
    void initWindow();
    void closeWindow();
    void checkRowTitles();
    void checkColumnTitles();
    void addNew();
    void clearVars();

    std::string name_;
    int rows_;
    int cols_;
    std::string rowTitlesStr_;
    std::string colTitlesStr_;
    bool rowTitlesStrStatus_;
    bool colTitlesStrStatus_;
    std::vector<int> rowTitles_;
    std::vector<int> colTitles_;

    QSharedPointer<QPushButton> addButton_;
    QSharedPointer<QPushButton> cancelButton_;
    QSharedPointer<QComboBox> selectionBox_;
    QSharedPointer<QLineEdit> nameEdit_;
    QSharedPointer<QLabel> rowLabel_;
    QSharedPointer<QLabel> columnLabel_;
    QSharedPointer<QLabel> rowStatusLabel_;
    QSharedPointer<QLabel> columnStatusLabel_;
    QSharedPointer<QLineEdit> rowTitlesEdit_;
    QSharedPointer<QLineEdit> columnTitlesEdit_;
    QSharedPointer<QLabel> nameInfoLabel_;
    QSharedPointer<QLabel> rowInfoLabel_;
    QSharedPointer<QLabel> columnInfoLabel_;
    QSharedPointer<QLabel> rowTitlesInfoLabel_;
    QSharedPointer<QLabel> columnTitlesInfoLabel_;
    QSharedPointer<QLabel> additionalInfoLabel_;

    Ui::NewItemWindow *ui;
};

#endif // NewItemWindow_H
