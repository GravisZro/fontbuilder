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


#include "fontbuilder.h"

#include <QSettings>

#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

//#include "ui_fontbuilder.h"
#include "fontconfig.h"
#include "fontrenderer.h"
#include "layoutconfig.h"
#include "layoutdata.h"
#include "layouterfactory.h"
#include "outputconfig.h"
#include "exporterfactory.h"
#include "imagewriterfactory.h"
#include "fontloader.h"


FontBuilder::FontBuilder(QWidget *parent) :
    QMainWindow(parent),
    m_draw_grid_checkbox(nullptr),
    m_font_test_frame(nullptr),
    m_font_preview_widget(nullptr),
    m_image_size_label(nullptr),
    m_image_writer(nullptr)
{
  setWindowTitle("FontBuilder");
  resize(612, 560);

  QComboBox* layout_engine_combobox = nullptr;
  CharactersFrame* characters_frame = nullptr;
  FontOptionsFrame* font_options_frame = nullptr;
  LayoutConfigFrame* layout_config_frame = nullptr;
  OutputFrame* output_frame = nullptr;
  FontSelectFrame* font_select_frame = nullptr;


  QSizePolicy sp(QSizePolicy::Minimum, QSizePolicy::Minimum);
  sp.setHorizontalStretch(0);
  sp.setVerticalStretch(0);

  {
    auto hb = new QHBoxLayout;
    hb->setSpacing(6);
    hb->setContentsMargins(11, 11, 11, 11);

    {
      auto tw0 = new QTabWidget(centralWidget());
      {
        auto font_tab = new QWidget;
        {
          auto gb0 = new QGroupBox("Font", font_tab);
          {
            sp.setHeightForWidth(gb0->sizePolicy().hasHeightForWidth());
            gb0->setSizePolicy(sp);
          }

          {
            auto hb = new QHBoxLayout(gb0);
            hb->setSpacing(6);
            hb->setContentsMargins(11, 11, 11, 11);

            {
              font_select_frame = new FontSelectFrame(gb0); // external
              sp.setHeightForWidth(font_select_frame->sizePolicy().hasHeightForWidth());
              font_select_frame->setSizePolicy(sp);
              hb->addWidget(font_select_frame);
            }
          }

          auto gb1 = new QGroupBox("Options", font_tab);
          {
            auto vb = new QVBoxLayout(gb1);
            vb->setSpacing(6);
            vb->setContentsMargins(11, 11, 11, 11);
            {
              font_options_frame = new FontOptionsFrame(gb1); // external
              sp.setHeightForWidth(font_options_frame->sizePolicy().hasHeightForWidth());
              font_options_frame->setSizePolicy(sp);
              vb->addWidget(font_options_frame);
            }
          }

          {
            auto vb = new QVBoxLayout(font_tab);
            vb->setSpacing(6);
            vb->setContentsMargins(11, 11, 11, 11);
            vb->addWidget(gb0);
            vb->addWidget(gb1);
            vb->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
          }
        }
        tw0->addTab(font_tab, "Font");
      }

      {
        auto characters_tab = new QWidget;
        auto vb = new QVBoxLayout(characters_tab);
        vb->setSpacing(6);
        vb->setContentsMargins(11, 11, 11, 11);
        {
          characters_frame = new CharactersFrame(characters_tab);
          vb->addWidget(characters_frame);
        }
        tw0->addTab(characters_tab, "Characters");
      }

      {
        auto layout_tab = new QWidget;
        {
          layout_engine_combobox = new QComboBox(layout_tab); // external
          layout_config_frame = new LayoutConfigFrame(layout_tab); // external

          {
            auto vb = new QVBoxLayout(layout_tab);
            vb->setSpacing(6);
            vb->setContentsMargins(11, 11, 11, 11);

            vb->addWidget(new QLabel("Layout engine"));
            vb->addWidget(layout_engine_combobox);
            vb->addWidget(layout_config_frame);

            vb->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
          }

          connect(layout_engine_combobox, &QComboBox::currentTextChanged, this, &FontBuilder::on_comboBoxLayouter_currentIndexChanged);
        }
        tw0->addTab(layout_tab, "Layout");
      }

      {
        auto output_tab = new QWidget;
        auto vb = new QVBoxLayout(output_tab);

        output_frame = new OutputFrame(output_tab); // external
        vb->addWidget(output_frame);

        {
          auto import_json = new QPushButton("Generate fonts from .json file", output_tab);
          vb->addWidget(import_json);
          connect(import_json, &QPushButton::clicked, this, &FontBuilder::on_pushButtonImportJson_clicked);
        }

        {
          auto write_font = new QPushButton("Write font", output_tab);
          vb->addWidget(write_font);
          connect(write_font, &QPushButton::clicked, this, &FontBuilder::on_pushButtonWriteFont_clicked);
        }

        tw0->addTab(output_tab, "Output");
      }

      tw0->setCurrentIndex(1);
      hb->addWidget(tw0);
    }

    {
      auto tw1 = new QTabWidget(centralWidget());
      {
        auto preview_tab = new QWidget;
        {
          auto gl = new QGridLayout(preview_tab);
          gl->setSpacing(6);
          gl->setContentsMargins(11, 11, 11, 11);

          {
            auto scrollArea = new QScrollArea(preview_tab);
            scrollArea->setWidgetResizable(true);
            scrollArea->setWidget(new QWidget);
            scrollArea->widget()->setGeometry(QRect(0, 0, 360, 425));
            {
              auto gl1 = new QGridLayout(scrollArea->widget());
              gl1->setSpacing(6);
              gl1->setContentsMargins(11, 11, 11, 11);
              gl1->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding), 4, 0, 1, 4);
              gl1->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding), 0, 0, 1, 4);
              gl1->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum), 2, 3, 1, 1);
              gl1->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum), 2, 0, 1, 1);

              {
                m_font_preview_widget = new FontDrawWidget(scrollArea->widget());
                gl1->addWidget(m_font_preview_widget, 2, 1, 1, 2);
              }
            }
            gl->addWidget(scrollArea, 3, 0, 1, 4);
          }

          {
            m_draw_grid_checkbox = new QCheckBox("Draw Grid");
            connect(m_draw_grid_checkbox, &QCheckBox::toggled, this, &FontBuilder::on_checkBoxDrawGrid_toggled);
            gl->addWidget(m_draw_grid_checkbox, 1, 0, 1, 1);
          }

          {
            m_image_size_label = new QLabel("0x0");
            gl->addWidget(m_image_size_label, 1, 1, 1, 1);
          }

          gl->addWidget(new QLabel("Scale:"), 1, 2, 1, 1);

          {
            auto layout_combobox = new QComboBox(preview_tab);
            layout_combobox->addItem("50%");
            layout_combobox->addItem("100%");
            layout_combobox->addItem("200%");
            layout_combobox->addItem("400%");
            layout_combobox->addItem("800%");
            layout_combobox->setLayoutDirection(Qt::LeftToRight);
            layout_combobox->setFrame(true);
            layout_combobox->setCurrentIndex(1);
            gl->addWidget(layout_combobox, 1, 3, 1, 1);
          }

          gl->setColumnStretch(0, 1);
          gl->setColumnStretch(1, 1);
        }
        tw1->addTab(preview_tab, "Preview");

        {
          auto test_font_tab = new QWidget();
          {
            auto gl = new QGridLayout(test_font_tab);
            gl->setSpacing(6);
            gl->setContentsMargins(11, 11, 11, 11);

            {
              m_font_test_frame = new FontTestFrame(test_font_tab); // external
              gl->addWidget(m_font_test_frame, 0, 0, 1, 1);
            }
          }
          tw1->addTab(test_font_tab, "Test Font");
        }
        tw1->setCurrentIndex(1);
      }
      hb->addWidget(tw1);
    }
    hb->setStretch(1, 1);
    setCentralWidget(new QWidget);
    centralWidget()->setLayout(hb);
  }

  setMenuBar(new QMenuBar);

  {
    auto file = new QMenu("&File", menuBar());
    menuBar()->addMenu(file);

    {
      auto open = file->addAction("&Open");
      connect(open, &QAction::triggered, this, &FontBuilder::on_action_Open_triggered);
    }
  }

  setStatusBar(new QStatusBar);




    m_font_config = new FontConfig(this);
    bool font_config_block = m_font_config->blockSignals(true);
    connect(m_font_config, &FontConfig::nameChanged, this, &FontBuilder::onFontNameChanged);
    connect(m_font_config, &FontConfig::sizeChanged, this, &FontBuilder::onFontNameChanged);

    m_font_renderer = new FontRenderer(this,m_font_config);

    connect(m_font_renderer, &FontRenderer::imagesChanged, this, &FontBuilder::onRenderedChanged);

    m_layout_config = new LayoutConfig(this);
    m_layout_data = new LayoutData(this);

    connect(m_layout_data, &LayoutData::layoutChanged, this, &FontBuilder::onLayoutChanged);

    m_layouter = nullptr;
    m_layouter_factory = new LayouterFactory(this);

    bool b = layout_engine_combobox->blockSignals(true);
    layout_engine_combobox->clear();
    layout_engine_combobox->addItems(m_layouter_factory->names());

    m_output_config = new OutputConfig(this);

    QSettings settings;
    restoreGeometry(settings.value("geometry").toByteArray());
    readConfig(settings,"fontconfig",m_font_config);
    m_font_config->normalize();
    readConfig(settings,"layoutconfig",m_layout_config);
    readConfig(settings,"outputconfig",m_output_config);
    m_draw_grid_checkbox->setChecked(settings.value("draw_grid").toBool());
    m_font_preview_widget->setDrawGrid(m_draw_grid_checkbox->isChecked());

    characters_frame->setConfig(m_font_config);
    font_options_frame->setConfig(m_font_config);
    if (!m_layout_config->layouter().isEmpty()) {
        for (int32_t i=0;i<layout_engine_combobox->count();i++)
            if (layout_engine_combobox->itemText(i)==m_layout_config->layouter())
                layout_engine_combobox->setCurrentIndex(i);
    }
    layout_config_frame->setConfig(m_layout_config);


    m_exporter_factory = new ExporterFactory(this);
    output_frame->setExporters(m_exporter_factory->names());

    m_image_writer_factory = new ImageWriterFactory(this);
    output_frame->setImageWriters(m_image_writer_factory->names());

    layout_engine_combobox->blockSignals(b);
    //layout_engine_combobox->setCurrentText(layout_engine_combobox->currentText());

    output_frame->setConfig(m_output_config);
    font_select_frame->setConfig(m_font_config);

    m_font_test_frame->setLayoutData(m_layout_data);
    m_font_test_frame->setRendererData(&m_font_renderer->data());
    m_font_test_frame->setFontConfig(m_font_config);

    m_font_preview_widget->setLayoutData(m_layout_data);
    m_font_preview_widget->setRendererData(&m_font_renderer->data());
    m_font_preview_widget->setLayoutConfig(m_layout_config);

    m_font_config->blockSignals(font_config_block);
    m_font_config->emmitChange();

    connect(m_font_config, &FontConfig::spacingChanged, this, &FontBuilder::onSpacingChanged);
    m_font_test_frame->refresh();

    m_font_loader = new FontLoader(this);

}

FontBuilder::~FontBuilder()
{
}


void FontBuilder::closeEvent(QCloseEvent *event)
 {
     QSettings settings;
     settings.setValue("geometry", saveGeometry());
     saveConfig(settings,"fontconfig",m_font_config);
     saveConfig(settings,"layoutconfig",m_layout_config);
     saveConfig(settings,"outputconfig",m_output_config);
     settings.setValue("draw_grid", m_draw_grid_checkbox->isChecked());
     QMainWindow::closeEvent(event);
 }

void FontBuilder::saveConfig(QSettings& settings,
                             const QString& name,
                             const QObject* object) {
    settings.beginGroup(name);
    const QMetaObject *metaobject = object->metaObject();
     int32_t count = metaobject->propertyCount();
     for (int32_t i=0; i<count; ++i) {
         QMetaProperty metaproperty = metaobject->property(i);
         const char *name = metaproperty.name();
         QVariant value = object->property(name);
         settings.setValue(name,value);
     }
    settings.endGroup();
}

void FontBuilder::readConfig(QSettings& settings,
                             const QString& name,
                             QObject* object) {
    settings.beginGroup(name);
    const QMetaObject *metaobject = object->metaObject();
     int32_t count = metaobject->propertyCount();
     for (int32_t i=0; i<count; ++i) {
         QMetaProperty metaproperty = metaobject->property(i);
         const char *name = metaproperty.name();
         if (settings.contains(name))
             object->setProperty(name,settings.value(name));
      }
    settings.endGroup();
}

void FontBuilder::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
//        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void FontBuilder::fontParametersChanged() {
    qDebug("font parameters changed");
}

void FontBuilder::on_comboBoxLayouter_currentIndexChanged(QString name)
{
    if (name.isEmpty()) return;
    if (m_layouter) {
        delete m_layouter;
        m_layouter = nullptr;
    }
    m_layouter = m_layouter_factory->build(name,this);
    if (m_layouter) {
        m_layouter->setConfig(m_layout_config);
        m_layouter->setData(m_layout_data);
        connect(m_font_renderer, &FontRenderer::imagesChangedWithData,
                m_layouter, &AbstractLayouter::on_ReplaceImages);
        m_layouter->on_ReplaceImages(m_font_renderer->rendered());
        m_layout_config->setLayouter(name);
    }
}

void FontBuilder::onRenderedChanged() {
    m_font_test_frame->refresh();
    if (m_image_writer)
        m_image_writer->forget();
}


void FontBuilder::setLayoutImage(const QImage& image) {
    m_font_preview_widget->setImage(image);
    m_image_size_label->setText(QString("Image size: %1x%2")
                              .arg(m_layout_data->width())
                              .arg(m_layout_data->height())
                              );
}

void FontBuilder::onLayoutChanged() {
    QImage image (m_layout_data->width(),m_layout_data->height(),QImage::Format_ARGB32);
    image.fill(0);
    {
        QPainter painter(&image);
        foreach (const LayoutChar& c,m_layout_data->placed()) {
          QPoint pos = c.bounding.topLeft() + m_layout_config->offset().topLeft();
          m_font_renderer->placeImage(painter,c.symbol, pos.x(), pos.y());
        }
    }
    qDebug() << "set layout image from rendered";
    m_layout_data->setImage(image);
    setLayoutImage(image);
    m_font_test_frame->refresh();
    if (m_image_writer)
        m_image_writer->forget();
}

void FontBuilder::on_checkBoxDrawGrid_toggled(bool dg) {
    m_font_preview_widget->setDrawGrid(dg);
}

void FontBuilder::onFontNameChanged() {
    QString name = QString("%1_%2_%3")
                   .arg(m_font_config->family())
                   .arg(m_font_config->style())
                   .arg(m_font_config->size())
                   .toLower().replace(" ","_");

    m_output_config->setImageName(name);
    m_output_config->setDescriptionName(name);
    if (m_image_writer)
        m_image_writer->forget();
}

void FontBuilder::doExport(bool x2) {
    QDir dir(m_output_config->path());
    QString texture_filename;
    setLayoutImage(m_layout_data->image());  

    if (m_output_config->writeImage()) {
        delete m_image_writer;
        m_image_writer = nullptr;

        //mudita pure must be selected in both image format and description format
        if( m_output_config->imageFormat().compare("Mudita Pure") == 0 &&
            m_output_config->descriptionFormat().compare("Mudita Pure") != 0 )
        {
            QMessageBox msgBox;
            msgBox.setText(tr("Error ")+m_output_config->descriptionFormat()+ " selected. Must be "+m_output_config->imageFormat());
            msgBox.exec();
            return;
        }
        if( m_output_config->imageFormat().compare("Mudita Pure") != 0 &&
            m_output_config->descriptionFormat().compare("Mudita Pure") == 0 )
        {
            QMessageBox msgBox;
            msgBox.setText(tr("Error ")+m_output_config->imageFormat()+ " selected. Must be "+m_output_config->descriptionFormat());
            msgBox.exec();
            return;
        }
        //both description and image formats are Mudita Pure
        if( m_output_config->imageFormat().compare("Mudita Pure") == 0 &&
            m_output_config->descriptionFormat().compare("Mudita Pure") == 0)
        {
            QString filename;
            QFile file;

            AbstractExporter* descExporter = m_exporter_factory->build(m_output_config->descriptionFormat(),this);
            if (!descExporter) {
                QMessageBox msgBox;
                msgBox.setText(tr("Unknown exporter :")+m_output_config->descriptionFormat());
                msgBox.exec();
                return;
            }
            descExporter->setFace(m_font_renderer->face());
            descExporter->setFontConfig(m_font_config,m_layout_config);
            descExporter->setData(m_layout_data,m_font_renderer->data());
            descExporter->setTextureFilename(texture_filename);
            descExporter->setScale(m_font_renderer->scale());

            QByteArray data;
            if (!descExporter->Write(data)) {
                 QMessageBox msgBox;
                 msgBox.setText(tr("Error on save description :\n")+descExporter->getErrorString()+"\nFile not writed.");
                 msgBox.exec();
             } else {
                filename = m_output_config->imageName();
                filename+="."+descExporter->getExtension();
                filename = dir.filePath(filename);
                 file.setFileName(filename);
                 if (file.open(QIODevice::ReadWrite)) {
                     file.write(data);
                 } else {
                     QMessageBox msgBox;
                     msgBox.setText(tr("Failed to open file:\n")+filename);
                     msgBox.exec();
                 }
             }

            AbstractImageWriter* imgExporter = m_image_writer_factory->build(m_output_config->imageFormat(),this);
            if (!imgExporter) {
                QMessageBox msgBox;
                msgBox.setText(tr("Unknown exporter :")+m_output_config->descriptionFormat());
                msgBox.exec();
                return;
            }

            imgExporter->setData(m_layout_data,m_layout_config,m_font_renderer->data());
            texture_filename = m_output_config->imageName();
            if (x2) {
                texture_filename += "_x2";
            }
            texture_filename+="."+imgExporter->extension();
             filename = dir.filePath(texture_filename);

            if (!imgExporter->Write(file)) {
                QMessageBox msgBox;
                msgBox.setText(tr("Error on save image :\n")+imgExporter->errorString()+"\nFile not writed.");
                msgBox.exec();
            }
            file.close();                        

            m_image_writer = imgExporter;
            m_image_writer->watch(filename);
            connect(m_image_writer, &AbstractImageWriter::imageChanged, this, &FontBuilder::onExternalImageChanged);

            delete descExporter;

            return;
        }

        AbstractImageWriter* exporter = m_image_writer_factory->build(m_output_config->imageFormat(),this);
        if (!exporter) {
            QMessageBox msgBox;
            msgBox.setText(tr("Unknown exporter :")+m_output_config->descriptionFormat());
            msgBox.exec();
            return;
        }

        exporter->setData(m_layout_data,m_layout_config,m_font_renderer->data());
        texture_filename = m_output_config->imageName();
        if (x2) {
            texture_filename += "_x2";
        }
        texture_filename+="."+exporter->extension();
        QString filename = dir.filePath(texture_filename);

        QFile file(this);
        file.setFileName(filename);
        if (!file.open(QIODevice::WriteOnly)) {
            delete exporter;
            QMessageBox msgBox;
            msgBox.setText(tr("Error opening file :")+filename);
            msgBox.exec();
            return;
        }
        if (!exporter->Write(file)) {
            QMessageBox msgBox;
            msgBox.setText(tr("Error on save image :\n")+exporter->errorString()+"\nFile not writed.");
            msgBox.exec();
        }
        file.close();
        m_image_writer = exporter;
        m_image_writer->watch(filename);
        connect(m_image_writer, &AbstractImageWriter::imageChanged, this, &FontBuilder::onExternalImageChanged);
    }
    if (m_output_config->writeDescription()) {
        AbstractExporter* exporter = m_exporter_factory->build(m_output_config->descriptionFormat(),this);
        if (!exporter) {
            QMessageBox msgBox;
            msgBox.setText(tr("Unknown exporter :")+m_output_config->descriptionFormat());
            msgBox.exec();
            return;
        }
        exporter->setFace(m_font_renderer->face());
        exporter->setFontConfig(m_font_config,m_layout_config);
        exporter->setData(m_layout_data,m_font_renderer->data());
        exporter->setTextureFilename(texture_filename);
        exporter->setScale(m_font_renderer->scale());
        QString filename = dir.filePath(m_output_config->descriptionName());
        if (x2) {
            filename += "_x2";
        }
        filename+="."+exporter->getExtension();
        QByteArray data;
        if (!exporter->Write(data)) {
             QMessageBox msgBox;
             msgBox.setText(tr("Error on save description :\n")+exporter->getErrorString()+"\nFile not writed.");
             msgBox.exec();
         } else {
             QFile file(filename);
             if (file.open(QIODevice::WriteOnly)) {
                 file.write(data);
             } else {

             }
         }
        delete exporter;
    }
}

void FontBuilder::on_pushButtonWriteFont_clicked()
{
    doExport(false);
    if (m_output_config->generateX2()) {
        m_font_renderer->render(2.0f);
        doExport(true);
        m_font_renderer->render(1.0f);
    }
}

void FontBuilder::onExternalImageChanged(const QString& fn) {
    if (!m_image_writer) return;
    qDebug() << "File changed : " << fn ;
    QFile f(this);
    f.setFileName(fn);
    if (!f.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed open : " << fn ;
        return;
    }
    QImage* image = m_image_writer->Read(f);
    if (image) {
        /*foreach (LayoutChar c, m_layout_data->placed()) {
            QImage img = image->copy(c.x+m_layout_config->offsetLeft(),c.y+m_layout_config->offsetTop(),
                                     c.w-m_layout_config->offsetLeft()-m_layout_config->offsetRight(),
                                     c.h-m_layout_config->offsetTop()-m_layout_config->offsetBottom());
            m_font_renderer->SetImage(c.symbol,img);
        }*/
        setLayoutImage(*image);
        m_layout_data->setImage(*image);
        qDebug() << "set layout image from exernal";
        m_font_test_frame->refresh();
        delete image;
    }
}

void FontBuilder::onSpacingChanged() {
    m_font_test_frame->refresh();
}

void FontBuilder::on_comboBox_currentIndexChanged(int32_t index)
{
    static const float scales[] = { 0.5,1.0,2.0,4.0,8.0 };
    m_font_preview_widget->setScale(scales[index]);
}

void FontBuilder::on_action_Open_triggered()
{

    QString file = QFileDialog::getOpenFileName(this,tr("Select file"),
                                                QString(),
                                                tr("Font file(*.xml)"));
    if (!file.isEmpty()) {
        if (m_font_loader->Load(file)) {

        }
    }


}

void FontBuilder::on_pushButtonImportJson_clicked()
{
    if( m_output_config->imageFormat().compare("Mudita Pure") != 0 ||
             m_output_config->descriptionFormat().compare("Mudita Pure") != 0 )
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Error ")+". Must be Mudita Pure both description and image format!");
        msgBox.exec();
        return;
    }
    else {
        QString file_path = QFileDialog::getOpenFileName(this, "Open a file", QDir::homePath(), "JSON File (*.json)");
        QFile inFile(file_path);
        inFile.open(QIODevice::ReadOnly|QIODevice::Text);
        QByteArray data = inFile.readAll();
        inFile.close();

        QJsonParseError errorPtr;
        QJsonDocument doc = QJsonDocument::fromJson(data, &errorPtr);
        if(doc.isNull()) {
            qDebug() << "Failed to load file";
        }
        QJsonObject obj = doc.object();
        std::vector<QPointF> listOfSettings;

        QJsonArray settingArray = obj.value("fonts").toArray();
        foreach(const QJsonValue & val, settingArray) {
            QString font_file = val.toObject().value("font-file").toString();
            QString font = val.toObject().value("font").toString();
            QString style = val.toObject().value("style").toString();
            int32_t size = val.toObject().value("size").toInt();
            int32_t dpi = val.toObject().value("DPI").toInt();
            int32_t bold = val.toObject().value("bold").toInt();
            int32_t italic = val.toObject().value("italic").toInt();
            float scale_width = val.toObject().value("scale_width").toInt();
            float scale_height = val.toObject().value("scale_height").toInt();
            int32_t spacing_char = val.toObject().value("spacing_char").toInt();
            int32_t spacing_line = val.toObject().value("spacing_line").toInt();
            bool smoothing = val.toObject().value("smoothing").toBool();
            QString smoothing_option = val.toObject().value("smoothing_option").toString();
            QString hinting_option = val.toObject().value("hinting_option").toString();
            int32_t padding_top = val.toObject().value("padding_top").toInt();
            int32_t padding_bottom = val.toObject().value("padding_bottom").toInt();
            int32_t padding_left = val.toObject().value("padding_left").toInt();
            int32_t padding_right = val.toObject().value("padding_right").toInt();
            if (m_font_config) {
                m_font_config->setFilename(font_file);
                m_font_config->setStyle(style);
                m_font_config->setSize(size);
                m_font_config->setDPI(dpi);
                m_font_config->setBold(bold);
                m_font_config->setItalic(italic);
                m_font_config->setWidth(scale_width);
                m_font_config->setHeight(scale_height);
                m_font_config->setCharSpacing(spacing_char);
                m_font_config->setLineSpacing(spacing_line);
                m_font_config->setAntialiased(smoothing);
                if(QString::compare(smoothing_option, "Light", Qt::CaseInsensitive) == 0) {
                    m_font_config->setAntiAliasing(static_cast<FontConfig::AAMethod>(1));
                }
                else if(QString::compare(smoothing_option, "LCD horizontal", Qt::CaseInsensitive) == 0) {
                    m_font_config->setAntiAliasing(static_cast<FontConfig::AAMethod>(2));
                }
                else if(QString::compare(smoothing_option, "LCD vertical", Qt::CaseInsensitive) == 0) {
                    m_font_config->setAntiAliasing(static_cast<FontConfig::AAMethod>(3));
                }
                else {
                    m_font_config->setAntiAliasing(static_cast<FontConfig::AAMethod>(0)); // standard-grayscale
                }
                if(QString::compare(hinting_option, "Default", Qt::CaseInsensitive) == 0) {
                    m_font_config->setHinting(static_cast<FontConfig::HintingMethod>(1));
                }
                else if(QString::compare(hinting_option, "ForceFreetypeAutohinting", Qt::CaseInsensitive) == 0) {
                    m_font_config->setHinting(static_cast<FontConfig::HintingMethod>(2));
                }
                else if(QString::compare(hinting_option, "DisableFreetypeAutohinting", Qt::CaseInsensitive) == 0) {
                    m_font_config->setHinting(static_cast<FontConfig::HintingMethod>(3));
                }
                else {
                    m_font_config->setHinting(static_cast<FontConfig::HintingMethod>(0)); // disabled
                }
            }
            if(m_layout_config) {

              m_layout_config->setOffset(QRect { QPoint{ padding_top, padding_left},
                                                 QPoint{ padding_bottom, padding_right} });
            }
            doExport(false);
            if (m_output_config->generateX2()) {
                m_font_renderer->render(2.0f);
                doExport(true);
                m_font_renderer->render(1.0f);
            }
        }
    }
}
