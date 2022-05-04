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

#include "fonttestframe.h"

#include "colorbutton.h"

FontTestFrame::FontTestFrame(QWidget *parent) :
    QFrame(parent),
    m_font_test(nullptr)
{
  resize(474, 361);

  QSizePolicy sp(QSizePolicy::Minimum, QSizePolicy::Minimum);
  sp.setHorizontalStretch(0);
  sp.setVerticalStretch(0);
  sp.setHeightForWidth(sizePolicy().hasHeightForWidth());

  setSizePolicy(sp);

  QPlainTextEdit* test_text = nullptr;

  auto vb = new QVBoxLayout(this);
  vb->setContentsMargins(0, 0, 0, 0);

  {
    auto scroll = new QScrollArea(this);
    sp.setHorizontalPolicy(QSizePolicy::Expanding);
    sp.setVerticalPolicy(QSizePolicy::Expanding);
    sp.setVerticalStretch(2);
    scroll->setSizePolicy(sp);

    scroll->setWidgetResizable(true);
    scroll->setWidget(new QWidget);
    scroll->widget()->setGeometry(0, 0, 470, 144);

    {
      auto gl = new QGridLayout(scroll->widget());

      gl->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding), 4, 1, 1, 1);
      gl->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding), 0, 1, 1, 1);
      gl->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum), 2, 2, 1, 1);
      gl->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum), 2, 0, 1, 1);

      m_font_test = new FontTestWidget;
      gl->addWidget(m_font_test, 2, 1, 1, 1);
    }

    vb->addWidget(scroll);
  }

  {
    auto gl = new QGridLayout;
    {
      auto gb = new QGroupBox;
      gb->setMinimumSize(90, 0);
      gb->setLayout(new QVBoxLayout);

      auto rb0 = new QRadioButton;
      auto rb1 = new QRadioButton;
      auto rb2 = new QRadioButton;

      rb1->setChecked(true);

      connect(rb0, &QCheckBox::toggled, m_font_test,
              [this](bool checked)
              {
                if(checked)
                  m_font_test->setAlign(FontTestWidget::ALIGN_LEFT);
              });
      connect(rb1, &QCheckBox::toggled, m_font_test,
              [this](bool checked)
              {
                if(checked)
                  m_font_test->setAlign(FontTestWidget::ALIGN_CENTER);
              });
      connect(rb2, &QCheckBox::toggled, m_font_test,
              [this](bool checked)
              {
                if(checked)
                  m_font_test->setAlign(FontTestWidget::ALIGN_RIGHT);
              });
      gl->addWidget(gb, 0, 0, 1, 2);
    }

    {
      test_text = new QPlainTextEdit;
      sp.setHeightForWidth(test_text->sizePolicy().hasHeightForWidth());
      sp.setHorizontalPolicy(QSizePolicy::Minimum);
      sp.setVerticalStretch(0);
      test_text->setSizePolicy(sp);
      test_text->setMaximumSize(16777215, 16777215);

      connect(test_text, &QPlainTextEdit::textChanged, m_font_test,
              [this, test_text]() { m_font_test->setText(test_text->document()->toPlainText()); });

      gl->addWidget(test_text, 0, 2, 4, 1);
    }

    {
      auto label = new QLabel(this);
      gl->addWidget(label, 1, 0, 1, 1);
    }

    {
      auto background_color = new ColorButton(this);
      background_color->setMinimumSize(QSize(32, 20));
      gl->addWidget(background_color, 1, 1, 1, 1);

      connect(background_color, &ColorButton::colorChanged, m_font_test, &FontTestWidget::setBGColor);
    }

    {
      auto useKerningCheckBox = new QCheckBox(this);
      gl->addWidget(useKerningCheckBox, 2, 0, 1, 2);

      connect(useKerningCheckBox, &QCheckBox::toggled, m_font_test,
              [this](bool checked) { m_font_test->setUseKerning(checked); });
    }

    gl->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding), 3, 0, 1, 2);
    gl->setColumnStretch(2, 1);

    vb->addLayout(gl);
  }
  vb->setStretch(0, 1);

  m_font_test->setText(test_text->document()->toPlainText());
}

FontTestFrame::~FontTestFrame()
{
}

void FontTestFrame::changeEvent(QEvent *e)
{
    QFrame::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
//        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
