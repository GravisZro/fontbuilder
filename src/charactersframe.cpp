/**
 * Copyright (c) 2010-2010 Andrey AndryBlack Kunitsyn
 * email:support.andryblack@gmail.com
 *
 * Report bugs and download new versions at http://code.google.com/p/fontbuilder
 *
 * This software is distributed under the MIT License.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "charactersframe.h"
#include "ui_charactersframe.h"
#include <QFileDialog>
#include <QDebug>
#include "fontconfig.h"
#include "charmapdialog.h"
#include "unicodeutils.h"

CharactersFrame::CharactersFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CharactersFrame)
{
    ui->setupUi(this);
    m_config = nullptr;
}

CharactersFrame::~CharactersFrame()
{
    delete ui;
}

void CharactersFrame::changeEvent(QEvent *e)
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

void CharactersFrame::on_pushButtonImport_clicked()
{
    QString file = QFileDialog::getOpenFileName(this,tr("Select file"),
                                                QString(),
                                                tr("Text file(*.txt)"));
    if (!file.isEmpty()) {
        QFile f(file);
        if (f.open(QFile::ReadOnly | QFile::Text)) {
            QByteArray data = f.readAll();
            auto text = QString::fromUtf8(data.constData(),data.size());
            ui->plainTextEdit->setPlainText(convert(removeDuplicates(sortChars(convert(text)))));
        }
    }
}

void CharactersFrame::on_pushButtonExport_clicked()
{
    QString file = QFileDialog::getSaveFileName(this,tr("Select file"),
                                                QString(),
                                                tr("Text file(*.txt)"));
    if (!file.isEmpty()) {
        QFile f(file);
        if (f.open(QFile::WriteOnly | QFile::Text)) {
            QByteArray data =
                ui->plainTextEdit->document()->toPlainText().toUtf8();
            f.write(data);
        }
    }
}


std::u32string CharactersFrame::getCharacters() const
{
  return convert(ui->plainTextEdit->document()->toPlainText());
}

void CharactersFrame::on_plainTextEdit_textChanged()
{
  m_config->setCharacters(removeDuplicates(sortChars(getCharacters())));
}

void CharactersFrame::setConfig(FontConfig* config)
{
    m_config = config;
    ui->plainTextEdit->setPlainText(convert(config->characters()));
}

std::u32string CharactersFrame::removeDuplicates(std::u32string text) const
{
    // Remove duplicates with C++ algorithm
    auto newEnd = std::unique(text.begin(), text.end());

    // Drop NUL character(s) at the beginning
    auto newStart = text.begin();
    while (newStart != newEnd && *newStart == 0)
        ++newStart;

    return std::u32string(newStart, newEnd);
}

std::u32string CharactersFrame::sortChars(std::u32string text) const
{
  std::sort(text.begin(), text.end());
  return text;
}


void CharactersFrame::on_pushButtonDefault_clicked()
{
    ui->plainTextEdit->setPlainText(convert(m_config->defaultCharacters()));
}

void CharactersFrame::on_pushButton_SelectFromCharsMap_clicked()
{
    CharMapDialog dialog(this);
    dialog.setModal(true);
    dialog.setChars(m_config->characters());
    int32_t result = dialog.exec();
    (void)result;
    if (dialog.result()==QDialog::Accepted) {
        m_config->setCharacters(removeDuplicates(sortChars(dialog.getCharacters())));
        bool block = ui->plainTextEdit->blockSignals(true);
        ui->plainTextEdit->setPlainText(convert(m_config->characters()));
        ui->plainTextEdit->blockSignals(block);
    }
}

void CharactersFrame::on_pushButtonRefresh_clicked()
{
    bool block = ui->plainTextEdit->blockSignals(true);
    ui->plainTextEdit->setPlainText(convert(m_config->characters()));
    ui->plainTextEdit->blockSignals(block);
}
