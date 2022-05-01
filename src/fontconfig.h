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

#ifndef FONTCONFIG_H
#define FONTCONFIG_H

#include <QObject>
#include <QColor>

class FontConfig : public QObject
{
Q_OBJECT
Q_ENUMS(HintingMethod)
Q_ENUMS(AAMethod)
public:
    explicit FontConfig(QObject *parent = nullptr);

    void normalize();

    enum HintingMethod {
        HintingDisable,
        HintingDefault,
        HintingForceFreetypeAuto,
        HintingDisableFreetypeAuto
    };

    enum AAMethod {
        AAliasingNormal,
        AAliasingLight,
        AAliasingLCDhor,
        AAliasingLCDvert
    };

    const QString& path() const { return m_path; }
    void setPath(const QString& path);
    Q_PROPERTY( QString path READ path WRITE setPath )

    const QString& filename() const { return m_filename; }
    void setFilename(const QString& filename);
    Q_PROPERTY( QString filename READ filename WRITE setFilename )

    const QString& family() const { return m_family; }
    void setFamily(const QString& family);
    Q_PROPERTY( QString family READ family WRITE setFamily )

    const QString& style() const { return m_style; }
    void setStyle(const QString& style);
    Q_PROPERTY( QString style READ style WRITE setStyle )

    int32_t faceIndex() const { return m_face_index;}
    void setFaceIndex(int32_t indx);
    Q_PROPERTY( int32_t faceIndex READ faceIndex WRITE setFaceIndex )

    int32_t size() const { return m_size;}
    void setSize(int32_t size);
    Q_PROPERTY( int32_t size READ size WRITE setSize )

    const QString& characters() const { return m_characters; }
    void setCharacters(const QString& characters);
    Q_PROPERTY( QString characters READ characters WRITE setCharacters )

    const QString& defaultCharacters() const;

    int32_t hinting() const { return m_hinting;}
    void setHinting(int32_t b);
    void resetHinting() { m_hinting = HintingDefault;}
    Q_PROPERTY( int32_t hinting READ hinting WRITE setHinting RESET resetHinting )

    bool renderMissing() const { return m_render_missing;}
    void setRenderMissing(bool b);
    Q_PROPERTY( bool renderMissing READ renderMissing WRITE setRenderMissing )

    bool antialiased() const { return m_antialiased;}
    void setAntialiased(bool b);
    Q_PROPERTY( bool antialiased READ antialiased WRITE setAntialiased )

    int32_t antialiasing() const { return m_aamethod;}
    void setAntiAliasing(int32_t b);
    void resetAntiAliasing() {m_aamethod = AAliasingNormal;}
    Q_PROPERTY( int32_t antialiasing READ antialiasing WRITE setAntiAliasing RESET resetAntiAliasing)

    int32_t bold() const { return m_bold;}
    void setBold(int32_t b);
    Q_PROPERTY( int32_t bold READ bold WRITE setBold )

    int32_t italic() const { return m_italic;}
    void setItalic(int32_t b);
    Q_PROPERTY( int32_t italic READ italic WRITE setItalic )

    float width() const { return m_width;}
    void setWidth(float b);
    Q_PROPERTY( float width READ width WRITE setWidth )

    float height() const { return m_height;}
    void setHeight(float b);
    Q_PROPERTY( float height READ height WRITE setHeight )

    int32_t lineSpacing() const { return m_line_spacing;}
    void setLineSpacing(int32_t s);
    Q_PROPERTY( int32_t lineSpacing READ lineSpacing WRITE setLineSpacing )

    int32_t charSpacing() const { return m_char_spacing;}
    void setCharSpacing(int32_t s);
    Q_PROPERTY( int32_t charSpacing READ charSpacing WRITE setCharSpacing )

    int32_t DPI() const { return m_dpi;}
    void setDPI(int32_t dpi);
    Q_PROPERTY( int32_t DPI READ DPI WRITE setDPI )

    static QString defaultFontsPath();
    void emmitChange();
private:
    QString m_path;
    QString m_filename;
    QString m_family;
    QString m_style;
    int32_t m_face_index;
    int32_t m_size;
    QString m_characters;
    int32_t    m_hinting;
    bool    m_render_missing;
    bool    m_antialiased;
    int32_t     m_aamethod;
    int32_t    m_bold;
    int32_t    m_italic;
    float   m_width;
    float   m_height;
    int32_t m_char_spacing;
    int32_t m_line_spacing;
    int32_t m_dpi;
signals:
    void nameChanged();
    void fileChanged();
    void faceIndexChanged();
    void sizeChanged();
    void charactersChanged();
    void renderingOptionsChanged();
    void spacingChanged();
public slots:

};


#endif // FONTCONFIG_H
