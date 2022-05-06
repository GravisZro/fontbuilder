#include "layoutconfigframe.h"
#include "ui_layoutconfigframe.h"
#include "layoutconfig.h"

LayoutConfigFrame::LayoutConfigFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::LayoutConfigFrame)
{
    ui->setupUi(this);
    m_config = 0;
}

LayoutConfigFrame::~LayoutConfigFrame()
{
    delete ui;
}

void LayoutConfigFrame::setConfig(LayoutConfig* config) {
    m_config = config;
    if (config) {
        ui->checkBoxOnePixelOffset->setChecked(config->onePixelOffset());
        ui->checkBoxPOT->setChecked(config->potImage());
        ui->spinBoxSizeIncrement->setValue(config->sizeIncrement());
        ui->spinBoxLeftOffset->setValue(config->offset().left());
        ui->spinBoxTopOffset->setValue(config->offset().top());
        ui->spinBoxRightOffset->setValue(config->offset().x() + config->offset().width());
        ui->spinBoxBottomOffset->setValue(config->offset().y() + config->offset().height());
    }
}

void LayoutConfigFrame::changeEvent(QEvent *e)
{
    QFrame::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void LayoutConfigFrame::on_checkBoxOnePixelOffset_toggled(bool checked)
{
    if (m_config) m_config->setOnePixelOffset(checked);
}

void LayoutConfigFrame::on_checkBoxPOT_toggled(bool checked)
{
    if (m_config) m_config->setPotImage(checked);
}

void LayoutConfigFrame::on_spinBoxSizeIncrement_valueChanged(int value)
{
    if (m_config) m_config->setSizeIncrement(value);
}

void LayoutConfigFrame::on_spinBoxTopOffset_valueChanged(int value)
{
  Q_ASSERT(m_config);
  QRect offset = m_config->offset();
  offset.setTop(value);
  m_config->setOffset(offset);
}

void LayoutConfigFrame::on_spinBoxLeftOffset_valueChanged(int value)
{
  Q_ASSERT(m_config);
  QRect offset = m_config->offset();
  offset.setLeft(value);
  m_config->setOffset(offset);
}

void LayoutConfigFrame::on_spinBoxBottomOffset_valueChanged(int value)
{
  Q_ASSERT(m_config);
  QRect offset = m_config->offset();
  offset.setBottom(value);
  m_config->setOffset(offset);
}

void LayoutConfigFrame::on_spinBoxRightOffset_valueChanged(int value)
{
  Q_ASSERT(m_config);
  QRect offset = m_config->offset();
  offset.setRight(value);
  m_config->setOffset(offset);
}


