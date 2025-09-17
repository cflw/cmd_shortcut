#ifndef XML_UTILITY_H
#define XML_UTILITY_H
#include <QDomDocument>
bool openXmlWithDom(const QString &filePath, QDomDocument &doc);
bool saveXmlToFile(const QDomDocument &doc, const QString &filename, int indent = 4);
QDomElement createSimpleElement(QDomDocument& doc, const QString& tagName, const QString& text);
#endif // XML_UTILITY_H
