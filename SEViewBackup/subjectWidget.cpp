#include "subjectWidget.h"
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QButtonGroup>
#include <QDebug>
#include <QList>
#include <QtAlgorithms>

QSet<int> SubjectWidget::getRulesRange(unsigned int begin, unsigned int length) const
{
    QList<int> subjectRules;

    for(unsigned int i= 0; i<length; ++i)
        subjectRules.append(rulesToBW.at(begin+i));
    return QSet<int>(subjectRules.begin(), subjectRules.end());
}

SubjectWidget::SubjectWidget(QWidget *parent) : QWidget(parent)
{
    const unsigned int ButtonSize=120;
    const unsigned int IconSize=110;
    setFixedSize(HEIGHT,WIDTH);

    motorButton=new QPushButton(QIcon(":/SEResources/motor_icon.png"),
                                QString("motorButton"),this);
    transmisionButton=new QPushButton(QIcon(":/SEResources/transmision_icon.png"),
                                QString("transmisionButton"),this);
    directionButton=new QPushButton(QIcon(":/SEResources/direccion_icon.png"),
                                    QString("directionButton"),this);
    electricIssuesButton=new QPushButton(QIcon(":/SEResources/electrico_icon.png"),
                                         QString("electricIssuesButton"),this);
    brakesButton=new QPushButton(QIcon(":/SEResources/frenos_icon.png"),
                                 QString("brakesButton"),this);
    suspensionButton=new QPushButton(QIcon(":/SEResources/suspension_icon.png"),
                                     QString("suspensionButton"),this);

    motorButton->setMinimumSize(ButtonSize,ButtonSize);
    motorButton->setIconSize(QSize(IconSize,IconSize));
    motorButton->setCheckable(true);

    electricIssuesButton->setMinimumSize(ButtonSize,ButtonSize);
    electricIssuesButton->setIconSize(QSize(IconSize,IconSize));
    electricIssuesButton->setCheckable(true);

    suspensionButton->setMinimumSize(ButtonSize,ButtonSize);
    suspensionButton->setIconSize(QSize(IconSize,IconSize));
    suspensionButton->setCheckable(true);

    directionButton->setMinimumSize(ButtonSize,ButtonSize);
    directionButton->setIconSize(QSize(IconSize,IconSize));
    directionButton->setCheckable(true);

    brakesButton->setMinimumSize(ButtonSize,ButtonSize);
    brakesButton->setIconSize(QSize(IconSize,IconSize));
    brakesButton->setCheckable(true);

    transmisionButton->setMinimumSize(ButtonSize,ButtonSize);
    transmisionButton->setIconSize(QSize(IconSize,IconSize));
    transmisionButton->setCheckable(true);

    buttonGroup=new QButtonGroup(this);
    buttonGroup->addButton(motorButton,0);
    buttonGroup->addButton(electricIssuesButton,1);
    buttonGroup->addButton(brakesButton,2);
    buttonGroup->addButton(suspensionButton,3);
    buttonGroup->addButton(directionButton,4);
    buttonGroup->addButton(transmisionButton,5);
    buttonGroup->setExclusive(false);

    //QGridLayout
    QGridLayout* gridLayout=new QGridLayout(this);
    gridLayout->addWidget(motorButton,0,0,Qt::AlignCenter);
    gridLayout->addWidget(electricIssuesButton,1,0,Qt::AlignCenter);
    gridLayout->addWidget(brakesButton,2,0,Qt::AlignCenter);
    gridLayout->addWidget(suspensionButton,0,1,Qt::AlignCenter);
    gridLayout->addWidget(directionButton,1,1,Qt::AlignCenter);
    gridLayout->addWidget(transmisionButton,2,1,Qt::AlignCenter);

    setLayout(gridLayout);

    connect(buttonGroup, SIGNAL(buttonToggled(QAbstractButton*,bool)),
            this, SLOT(onButtonToggled(QAbstractButton*,bool)));
}

void SubjectWidget::onButtonToggled(QAbstractButton* pButton, bool checked)
{
    QSet<int> temp;
    const auto id = buttonGroup->id(pButton);
    switch(id){
        case 0://Engine
        temp=getRulesRange(0,95);
            break;
        case 1: //Electric issues
        temp=getRulesRange(103,9);
            break;
        case 2: //Brakes
        temp=getRulesRange(115,16);
            break;
        case 3: //Suspension
        temp=getRulesRange(113,3);
            break;
        case 4: // Direction
        temp=getRulesRange(95,5);
            break;
        case 5:
        temp=getRulesRange(131,16);
            break;
        default:
        qDebug()<<"No supported button was clicked";
            break;
    }
    emit subjectToggled(temp,checked);
}
