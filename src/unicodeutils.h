#ifndef UNICODEUTILS_H
#define UNICODEUTILS_H

#include <string>
#include <QString>
#include <QVector>

inline QString convert(const std::u32string& text)
{
  return QString::fromUcs4(text.data(), text.size());
}

inline std::u32string convert(const QString& text)
{
  auto tmp = text.toUcs4();
  return std::u32string(tmp.begin(), tmp.end());
}

#endif // UNICODEUTILS_H
