﻿#include "toptoolbar.h"

#include <QLabel>
#include <QPushButton>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QSlider>
#include <QFileDialog>
#include <QStandardPaths>
#include <QApplication>
#include <QMenu>
#include <QDebug>

#include <DApplication>

#include "utils/global.h"
#include "widgets/pushbutton.h"
#include "widgets/seperatorline.h"
#include "widgets/bigcolorbutton.h"
#include "widgets/bordercolorbutton.h"
#include "widgets/toolbutton.h"

#include "textfontlabel.h"

DWIDGET_USE_NAMESPACE

TopToolbar::TopToolbar(QWidget* parent)
: QFrame(parent),
  m_shapesWidgetExist(false)
{
    DRAW_THEME_INIT_WIDGET("TopToolbar");
    setObjectName("TopToolbar");
    QLabel* logoLabel = new QLabel(this);
    logoLabel->setFixedSize(24, 25);
    logoLabel->setObjectName("LogoLabel");

    QButtonGroup* shapesBtnGroup = new QButtonGroup(this);
    shapesBtnGroup->setExclusive(true);

    ToolButton* artBoardBtn = new ToolButton(this);
    artBoardBtn->setObjectName("ArtBoardBtn");
    shapesBtnGroup->addButton(artBoardBtn);

    ToolButton* picBtn = new ToolButton(this);
    picBtn->setObjectName("PictureBtn");
    shapesBtnGroup->addButton(picBtn);

    ToolButton* rectBtn = new ToolButton(this);
    rectBtn->setObjectName("RectBtn");
    shapesBtnGroup->addButton(rectBtn);

    ToolButton* ovalBtn = new ToolButton(this);
    ovalBtn->setObjectName("OvalBtn");
    shapesBtnGroup->addButton(ovalBtn);

    ToolButton* lineBtn = new ToolButton(this);
    lineBtn->setObjectName("LineBtn");
    shapesBtnGroup->addButton(lineBtn);

    ToolButton* textBtn = new ToolButton(this);
    textBtn->setObjectName("TextBtn");
    shapesBtnGroup->addButton(textBtn);

    ToolButton* blurBtn = new ToolButton(this);
    blurBtn->setObjectName("BlurBtn");
    shapesBtnGroup->addButton(blurBtn);

    initStackWidget();

    ToolButton* exportBtn = new ToolButton(this);
    exportBtn->setObjectName("ExportBtn");

    QHBoxLayout* mLayout = new QHBoxLayout(this);
    mLayout->setMargin(0);
    mLayout->setSpacing(10);
    mLayout->addWidget(logoLabel);
    mLayout->addSpacing(15);
    mLayout->addWidget(artBoardBtn);
    mLayout->addSpacing(20);
    mLayout->addWidget(picBtn);
    mLayout->addWidget(rectBtn);
    mLayout->addWidget(ovalBtn);
    mLayout->addWidget(lineBtn);
    mLayout->addWidget(textBtn);
    mLayout->addWidget(blurBtn);
    mLayout->addStretch();
    mLayout->addWidget(m_stackWidget);
    mLayout->addStretch();
    mLayout->addWidget(exportBtn);
    mLayout->addSpacing(30);
    setLayout(mLayout);

    connect(picBtn, &ToolButton::clicked, this, [=]{
        setMiddleStackWidget(Status::Cut);
    });
    connect(rectBtn, &ToolButton::clicked, this, [=]{
         setMiddleStackWidget(Status::FillShape);
         drawShapes("rectangle");
    });
    connect(ovalBtn, &ToolButton::clicked, this, [=]{
        setMiddleStackWidget(Status::FillShape);
        drawShapes("oval");
    });
    connect(lineBtn, &ToolButton::clicked, this, [=]{
        setMiddleStackWidget(Status::DrawLine);
        drawShapes("arbitraryCurve");
    });
    connect(textBtn, &ToolButton::clicked, this, [=]{
        setMiddleStackWidget(Status::DrawText);
        drawShapes("text");
    });
    connect(blurBtn, &ToolButton::clicked, this, [=]{
        setMiddleStackWidget(Status::DrawBlur);
        drawShapes("blur");
    });
    connect(artBoardBtn, &ToolButton::clicked, this, [=]{
        setMiddleStackWidget(Status::AdjustSize);
    });
}

void TopToolbar::importImage() {
    QFileDialog *dialog = new QFileDialog(this);
    dialog->setWindowTitle(tr("Import Image"));
    dialog->setAcceptMode(QFileDialog::AcceptOpen);


    m_path = QFileDialog::getOpenFileName(this, tr("Open Image"),
             QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
             tr("Image Files (*.png *.jpg *.bmp, *svg)"));

    emit openImage(m_path);
}

void TopToolbar::initStackWidget() {
    m_stackWidget = new QStackedWidget(this);
    //empty widget
    m_emptyWidget = new QWidget(this);
    m_stackWidget->addWidget(m_emptyWidget);
    //cutwidget.
    m_cutWidget = new QWidget(this);
    QMap<int, QString> btnInfoMap;
    btnInfoMap.insert(0, "LeftRotate");
    btnInfoMap.insert(1, "RightRotate");
    btnInfoMap.insert(2, "CutButton");
    btnInfoMap.insert(3, "FlipHorizontalBtn");
    btnInfoMap.insert(4, "FlipVerticalBtn");
    QStringList btnTextList;
    btnTextList << tr("Rotate 90° CCW") << tr("Rotate 90° CW") << tr("Clip")
                          << tr("Flip horizontally") << tr("FlipVertically");
    QList<PushButton*> btnList;
    QMap<int, QString>::const_iterator i = btnInfoMap.constBegin();
    while (i != btnInfoMap.constEnd()) {
        PushButton* btn = new PushButton();
        btnList.append(btn);
        btn->setObjectName(i.value());
        btn->setText(btnTextList[i.key()]);

        if (i.key() == 0) {
            connect(btn, &PushButton::clicked, this, [=]{
                emit rotateImage(m_path, -90);
                qDebug() << "topToolbar rotateImage:" << m_path;
            });
        }
        if (i.key() == 1) {
            connect(btn, &PushButton::clicked, this, [=]{
                emit rotateImage(m_path, 90);
            });
        }
        if (i.key() == 3) {
            connect(btn, &PushButton::clicked, this, [=]{
                emit mirroredImage(true, false);
            });
        }
        if (i.key() == 4) {
            connect(btn, &PushButton::clicked, this, [=]{
                emit mirroredImage(false, true);
            });
        }
        ++i;
    }

    for(int k = 0; k < btnList.length(); k++) {
        connect(btnList[k], &PushButton::clicked, this, [=]{
            if (btnList[k]->getChecked()) {
                 for (int j = 0; j < k; j++) {
                     btnList[j]->setChecked(false);
                 }

                 for(int p = k + 1; p < btnList.length(); p++) {
                     btnList[p]->setChecked(false);
                 }
            } else {
                qDebug() << "Btn exclusive failed" << k;
            }
        });
    }

    QHBoxLayout* cutHbLayout = new QHBoxLayout(m_cutWidget);
    cutHbLayout->setMargin(0);
    cutHbLayout->setSpacing(0);
    for(int j = 0; j < btnList.length(); j++) {
        cutHbLayout->addWidget(btnList[j]);
    }
    m_cutWidget->setLayout(cutHbLayout);
    m_stackWidget->addWidget(m_cutWidget);

    //colorPanel.
    m_colorPanel = new ColorPanel();
    m_strokeARect = new DArrowRectangle(DArrowRectangle::ArrowTop, this);
    m_strokeARect->setArrowX(25);
    m_strokeARect->setArrowWidth(30);
    m_strokeARect->setContent(m_colorPanel);
    m_strokeARect->setBackgroundColor(QColor(255, 255, 255, 0.5));
    connect(m_colorPanel, &ColorPanel::colorChanged, this, &TopToolbar::setShapesColor);

    //draw line.
    m_drawLineWidget = new QWidget(this);
    QLabel* borderColLabel = new QLabel(this);
    borderColLabel->setObjectName("BorderStrokeLabel");
    borderColLabel->setText(tr("Stroke"));
    BorderColorButton* borderCButton = new BorderColorButton(this);
    connect(borderCButton, &BorderColorButton::clicked, this, [=]{
        QPoint curPos = this->cursor().pos();
        if (m_strokeARect->isHidden()) {
            m_strokeARect->show(curPos.x(), curPos.y() + 20);
        } else {
            m_strokeARect->hide();
        }
        setDrawStatus(DrawStatus::Stroke);
    });

    SeperatorLine* sep1Line = new SeperatorLine(this);
    QLabel* borderStyleLabel = new QLabel(this);
    borderStyleLabel->setObjectName("BorderStyleLabel");
    borderStyleLabel->setText(tr("Style"));

    QList<ToolButton*> lineBtnList;
    QStringList lineBtnNameList;
    lineBtnNameList << "StraightLineBtn" << "ArbiLineBtn" << "ArrowBtn";
    QButtonGroup* btnGroup = new QButtonGroup(this);
    btnGroup->setExclusive(true);
    for(int k = 0; k < lineBtnNameList.length(); k++) {
        ToolButton* lineBtn = new ToolButton(this);
        lineBtn->setObjectName(lineBtnNameList[k]);
        lineBtnList.append(lineBtn);
        btnGroup->addButton(lineBtn);
        if (k == 1) {
            lineBtn->setChecked(true);
        }
        connect(lineBtn, &ToolButton::clicked, this, [=]{
            setLineShape(k);
        });
    }
    SeperatorLine* sep2Line = new SeperatorLine(this);

    QLabel* borderLWLabel = new QLabel(this);
    borderLWLabel->setObjectName("BorderLWLabel");
    borderLWLabel->setText(tr("Width"));
    QList<ToolButton*> lwBtnList;
    QStringList lwBtnNameList;
    lwBtnNameList << "FinerLineBtn" << "FineLineBtn"
                                 << "MediumLineBtn" << "BoldLineBtn";
    QButtonGroup* lwBtnGroup = new QButtonGroup(this);
    lwBtnGroup->setExclusive(true);
    for (int i = 0; i < lwBtnNameList.length(); i++) {
        ToolButton* lwBtn = new ToolButton(this);
        lwBtn->setObjectName(lwBtnNameList[i]);
        lwBtnList.append(lwBtn);
        lwBtnGroup->addButton(lwBtn);
        connect(lwBtn, &ToolButton::clicked, this, [=]{
            emit shapesLineWidthChanged((i+1)*2);
        });
        if (i == 0) {
            lwBtn->setChecked(true);
        }
    }

    QHBoxLayout* drawHbLayout = new QHBoxLayout(m_drawLineWidget);
    drawHbLayout->setMargin(0);
    drawHbLayout->setSpacing(10);
    drawHbLayout->addWidget(borderColLabel);
    drawHbLayout->addWidget(borderCButton);
    drawHbLayout->addWidget(sep1Line, 0, Qt::AlignCenter);
    drawHbLayout->addWidget(borderStyleLabel);

    for(int h = 0; h < lineBtnList.length(); h++) {
        drawHbLayout->addWidget(lineBtnList[h]);
    }
    drawHbLayout->addWidget(sep2Line, 0, Qt::AlignCenter);
    drawHbLayout->addWidget(borderLWLabel);
    for(int j = 0; j < lwBtnList.length(); j++) {
        drawHbLayout->addWidget(lwBtnList[j]);
    }
    m_drawLineWidget->setLayout(drawHbLayout);
    m_stackWidget->addWidget(m_drawLineWidget);

    //fill rectangle, and oval.
    m_fillShapeWidget = new QWidget(this);
    BigColorButton* fillColorBtn = new BigColorButton(this);
    QLabel* fillColLabel = new QLabel(this);
    fillColLabel->setText(tr("Fill"));

    BorderColorButton* fillShapeStrokeBtn = new BorderColorButton(this);
    fillShapeStrokeBtn->setObjectName("FillStrokeButton");

    connect(fillColorBtn, &BigColorButton::clicked, this, [=]{
        fillShapeStrokeBtn->setChecked(false);
        QPoint curPos = this->cursor().pos();
        if (m_strokeARect->isHidden()) {
            m_strokeARect->show(curPos.x(), curPos.y() + 20);
        } else {
            m_strokeARect->hide();
        }
        qDebug() << "BigColorButton:" << DrawStatus::Fill;
        setDrawStatus(DrawStatus::Fill);
    });

    connect(fillShapeStrokeBtn, &BorderColorButton::clicked, this, [=]{
        fillColorBtn->setChecked(false);
        QPoint curPos = this->cursor().pos();
        if (m_strokeARect->isHidden()) {
            m_strokeARect->show(curPos.x(), curPos.y() + 20);
        } else {
            m_strokeARect->hide();
        }
        qDebug() << "BorderColorButton:" << DrawStatus::Stroke;
        setDrawStatus(DrawStatus::Stroke);
    });

    QLabel* strokeLabel = new QLabel(this);
    strokeLabel->setText(tr("Stroke"));
    SeperatorLine* fillShapeSepLine = new SeperatorLine();
    QLabel* fillShapeLWLabel = new QLabel(this);
    fillShapeLWLabel->setObjectName("BorderLabel");
    fillShapeLWLabel->setText(tr("Width"));

    QList<ToolButton*> fillShapeLwBtnList;
    QButtonGroup* fillShapeBtnGroup = new QButtonGroup(this);
    fillShapeBtnGroup->setExclusive(true);
    for (int k = 0; k < lwBtnNameList.length(); k++) {
        ToolButton* fillShapeBtn = new ToolButton(this);
        fillShapeBtn->setObjectName(lwBtnNameList[k]);
        fillShapeLwBtnList.append(fillShapeBtn);
        fillShapeBtnGroup->addButton(fillShapeBtn);
        connect(fillShapeBtn, &ToolButton::clicked, this, [=]{
            emit shapesLineWidthChanged((k+1)*2);
        });
        if (k == 0) {
            fillShapeBtn->setChecked(true);
        }
    }
    QHBoxLayout* fillHLayout = new QHBoxLayout(this);
    fillHLayout->setMargin(0);
    fillHLayout->setSpacing(6);
    fillHLayout->addStretch();
    fillHLayout->addWidget(fillColorBtn);
    fillHLayout->addWidget(fillColLabel);
    fillHLayout->addWidget(fillShapeStrokeBtn);
    fillHLayout->addWidget(strokeLabel);
    fillHLayout->addWidget(fillShapeSepLine);
    fillHLayout->addWidget(fillShapeLWLabel);
    for(int j = 0; j < fillShapeLwBtnList.length(); j++) {
        fillHLayout->addWidget(fillShapeLwBtnList[j]);
    }
    fillHLayout->addStretch();
    m_fillShapeWidget->setLayout(fillHLayout);
    m_stackWidget->addWidget(m_fillShapeWidget);

    //draw text.
    m_drawTextWidget = new QWidget(this);
    BigColorButton* fillBtn = new BigColorButton(this);
    connect(fillBtn, &BigColorButton::clicked, this, [=]{
        QPoint curPos = this->cursor().pos();
        if (m_strokeARect->isHidden()) {
            m_strokeARect->show(curPos.x(), curPos.y() + 20);
        } else {
            m_strokeARect->hide();
        }
        qDebug() << "BorderColorButton:" << DrawStatus::Stroke;
        setDrawStatus(DrawStatus::Fill);
    });

    QLabel* colBtnLabel = new QLabel(this);
    colBtnLabel->setText(tr("Fill"));

    SeperatorLine* textSeperatorLine = new SeperatorLine(this);

    QLabel* fontsizeLabel = new QLabel(this);
    fontsizeLabel->setText(tr("Font size"));
    TextFontLabel* fontLabel = new TextFontLabel(this);
    connect(fontLabel, &TextFontLabel::textFontsizeChanged, this,
            &TopToolbar::textFontsizeChanged);

    QHBoxLayout* textHbLayout = new QHBoxLayout(m_drawTextWidget);
    textHbLayout->setMargin(0);
    textHbLayout->setSpacing(6);
    textHbLayout->addStretch();
    textHbLayout->addWidget(fillBtn);
    textHbLayout->addWidget(colBtnLabel);
    textHbLayout->addWidget(textSeperatorLine);
    textHbLayout->addWidget(fontsizeLabel);
    textHbLayout->addWidget(fontLabel);
    textHbLayout->addStretch();
    m_drawTextWidget->setLayout(textHbLayout);
    m_stackWidget->addWidget(m_drawTextWidget);

    //draw blur widget.
    m_drawBlurWidget = new QWidget(this);
    QLabel* penLabel = new QLabel(this);
    penLabel->setText(tr("Width"));
    ToolButton* fineBtn = new ToolButton(this);
    fineBtn->setObjectName("LineMostThinBtn");

    QSlider* lineWidthSlider = new QSlider(Qt::Horizontal, this);
    lineWidthSlider->setMinimum(20);
    lineWidthSlider->setMaximum(160);
    connect(lineWidthSlider, &QSlider::valueChanged, this, [=]{
        qDebug() << "lineWidth Slider:" << lineWidthSlider->value();
        emit blurLineWidthChanged(lineWidthSlider->value());
    });

    ToolButton* boldBtn = new ToolButton(this);
    boldBtn->setObjectName("LineThickLineBtn");
    QHBoxLayout* blurHbLayout = new QHBoxLayout(m_drawBlurWidget);
    blurHbLayout->addWidget(penLabel);
    blurHbLayout->addSpacing(4);
    blurHbLayout->addWidget(fineBtn);
    blurHbLayout->addWidget(lineWidthSlider);
    blurHbLayout->addSpacing(4);
    blurHbLayout->addWidget(boldBtn);
    m_drawBlurWidget->setLayout(blurHbLayout);
    m_stackWidget->addWidget(m_drawBlurWidget);

    //process  artboard's size.
    m_adjustsizeWidget = new QWidget(this);
    QLabel* casWidthLabel = new QLabel(this);
    casWidthLabel->setObjectName("CasWidthLabel");
    casWidthLabel->setText(tr("Canvas width"));

    QLineEdit* lengthLEdit = new QLineEdit(this);
    lengthLEdit->setObjectName("LengthLineEdit");
    lengthLEdit->setStyleSheet("background-color: red;");

    QLabel* casHeightLabel = new QLabel(this);
    casHeightLabel->setObjectName("CasHeightLabel");
    casHeightLabel->setText(tr("Canvas height"));
    QLineEdit* widthLEdit = new QLineEdit(this);
    widthLEdit->setObjectName("WidthLineEdit");

    ToolButton* cutTransAreaBtn = new ToolButton(this);
    cutTransAreaBtn->setFixedWidth(100);
    cutTransAreaBtn->setObjectName("CutAreaLineEdit");
    cutTransAreaBtn->setText(tr("裁剪透明区域"));
    QHBoxLayout* picHbLayout = new QHBoxLayout(m_adjustsizeWidget);
    picHbLayout->addWidget(casWidthLabel);
    picHbLayout->addWidget(lengthLEdit);
    picHbLayout->addWidget(casHeightLabel);
    picHbLayout->addWidget(widthLEdit);
    picHbLayout->addWidget(cutTransAreaBtn);
    m_adjustsizeWidget->setLayout(picHbLayout);
    m_stackWidget->addWidget(m_adjustsizeWidget);

    m_stackWidget->setCurrentWidget(m_emptyWidget);
}

void TopToolbar::setMiddleStackWidget(Status status) {
    switch (status) {
    case Empty: m_stackWidget->setCurrentWidget(m_emptyWidget); break;
    case Cut: m_stackWidget->setCurrentWidget(m_cutWidget); break;
    case DrawLine: m_stackWidget->setCurrentWidget(m_fillShapeWidget); break;
    case FillShape: m_stackWidget->setCurrentWidget(m_fillShapeWidget); break;
    case DrawText: m_stackWidget->setCurrentWidget(m_drawTextWidget); break;
    case DrawBlur: m_stackWidget->setCurrentWidget(m_drawBlurWidget); break;
    case AdjustSize: m_stackWidget->setCurrentWidget(m_adjustsizeWidget); break;
    default: break;
    }
}

void TopToolbar::setDrawStatus(DrawStatus drawstatus) {
    m_drawStatus = drawstatus;
}

void TopToolbar::drawShapes(QString shape) {
    if (!m_shapesWidgetExist) {
        emit initShapeWidgetAction(shape);
        m_shapesWidgetExist = true;
    } else {
        emit initShapeWidgetAction(shape);
    }
}

void TopToolbar::setShapesColor(QColor color) {
    qDebug() << "TopToolbar:" << color;
    if (m_drawStatus == DrawStatus::Fill) {
        emit shapesColorChanged(DrawStatus::Fill, color);
    } else {
        emit shapesColorChanged(DrawStatus::Stroke, color);
    }
}

void TopToolbar::setLineShape(int lineIndex) {
    qDebug() << "TopToolbar: setLineShape lineIndex:" << lineIndex;
    switch (lineIndex) {
    case 0: { emit lineShapeChanged("straightLine"); break;}
    case 1: { emit lineShapeChanged("arbitraryCurve"); break;}
    case 2: { emit lineShapeChanged("arrow"); break;}
    default: break;
    }
}

bool TopToolbar::shapesWidgetExist() {
    return m_shapesWidgetExist;
}

QMenu* TopToolbar::mainMenu() {
    QMenu* menu = new QMenu(this);
    QAction* importAc = menu->addAction(tr("Import"));
    QAction* importFScannerAc = menu->addAction(tr("Import from scanner"));
    QAction* saveAc = menu->addAction(tr("Save"));
    QAction* saveAsAc = menu->addAction(tr("Save as"));
    QAction* printAc = menu->addAction(tr("Print"));
    QAction* themeAc = menu->addAction(tr("Dark theme"));
    QAction* helpAc = menu->addAction(tr("Help"));

    Q_UNUSED(importFScannerAc);
    Q_UNUSED(saveAc);
    Q_UNUSED(saveAsAc);
    Q_UNUSED(printAc);
    Q_UNUSED(themeAc);
    Q_UNUSED(helpAc);
   qApp->setProductIcon(QPixmap(":/theme/common/images/deepin-draw.png"));
   qApp->setApplicationDescription(tr("Deepin Draw is a lightweight drawing tool."
                " You can freely draw on the layer or simplely edit images. "
                "Deepin Draw is released under GPL v3."));
   connect(importAc, &QAction::triggered, this, &TopToolbar::importImage);

    return menu;
}

void TopToolbar::resizeEvent(QResizeEvent *event) {
    this->updateGeometry();
    Q_UNUSED(event);
}

TopToolbar::~TopToolbar() {
}
