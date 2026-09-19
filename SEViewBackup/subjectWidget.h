#ifndef SUBJECT_H
#define SUBJECT_H

#include <QWidget>
#include <QSet>
#include <QVector>

#define HEIGHT 375
#define WIDTH 450

class QPushButton;
class QAbstractButton;
class QButtonGroup;

class SubjectWidget : public QWidget
{
    Q_OBJECT
private:
    QVector<int> rulesToBW;
    QButtonGroup* buttonGroup;
    QPushButton* motorButton;
    QPushButton* transmisionButton;
    QPushButton* brakesButton;
    QPushButton* electricIssuesButton;
    QPushButton* suspensionButton;
    QPushButton* directionButton;

public:
    QSet<int> getRulesRange(unsigned int begin, unsigned int length)const;
    void setRules(QVector<int> v){rulesToBW=v;}
    explicit SubjectWidget(QWidget *parent = 0);

signals:
    void subjectToggled(QSet<int> subjectSet,bool checked);
public slots:
    void onButtonToggled(QAbstractButton* pButton, bool checked);
};



#endif // SUBJECT_H
