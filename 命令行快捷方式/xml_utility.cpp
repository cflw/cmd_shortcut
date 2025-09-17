#include "xml_utility.h"
#include <QFile>
#include <QTextStream>
bool openXmlWithDom(const QString &filePath, QDomDocument &doc) {
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return false;
	}
	if (!doc.setContent(&file)) {
		file.close();
		return false;
	}
	file.close();
	return true;
}
bool saveXmlToFile(const QDomDocument &doc, const QString &filename, int indent) {
	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		return false;
	}
	QTextStream out(&file);
	doc.save(out, indent);
	file.close();
	return true;
}
QDomElement createSimpleElement(QDomDocument& doc, const QString& tagName, const QString& text) {
	QDomElement element = doc.createElement(tagName);
	element.appendChild(doc.createTextNode(text));
	return element;
}
