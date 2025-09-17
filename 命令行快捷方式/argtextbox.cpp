#include "argtextbox.h"
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
//==============================================================================
// 控件
//==============================================================================
W参数文本框::W参数文本框(QWidget *a父):
	QLineEdit(a父) {
	setAcceptDrops(true);	// 启用拖放功能
}
void W参数文本框::dragEnterEvent(QDragEnterEvent *event) {
	// 检查拖曳的内容是否包含文件
	if (event->mimeData()->hasUrls()) {
		m拖曳前样式 = styleSheet();
		f文本框_s颜色(*this, E颜色::e绿);    // 拖曳进入时改变背景色
		event->acceptProposedAction();
	} else {
		event->ignore();
	}
}
void W参数文本框::dragLeaveEvent(QDragLeaveEvent *event) {
	setStyleSheet(m拖曳前样式);  // 拖曳离开时恢复样式
	QLineEdit::dragLeaveEvent(event);
}
void W参数文本框::dropEvent(QDropEvent *event) {
	if (event->mimeData()->hasUrls()) {
		QStringList filePaths;
		for(const QUrl &url : event->mimeData()->urls()) {
			filePaths.append(url.toLocalFile());
		}
		emit setMoreText(filePaths);
	}
	event->acceptProposedAction();
	setStyleSheet(m拖曳前样式);  // 拖曳结束时恢复样式
}
//==============================================================================
// 代理
//==============================================================================
C参数文本框代理::C参数文本框代理(QLineEdit &a控件):
	w文本框(&a控件) {
}
QString C参数文本框代理::fg值() const {
	return w文本框->text();
}
void C参数文本框代理::fs值(const QString &a文本) const {
	w文本框->setText(a文本);
}
bool C参数文本框代理::fi值() const {
	return !w文本框->text().isEmpty();
}
void C参数文本框代理::fs颜色(E颜色 a颜色) const {
	f文本框_s颜色(*w文本框, a颜色);
}
void C参数文本框代理::f清除输入() const {
	w文本框->clear();
}
//==============================================================================
// 函数
//==============================================================================
void f文本框_s颜色(QLineEdit &w文本框, E颜色 a颜色) {
	switch (a颜色) {
	case E颜色::e白:
		w文本框.setStyleSheet("");
		break;
	case E颜色::e红:
		w文本框.setStyleSheet("background-color: #ffe0e0;");
		break;
	case E颜色::e绿:
		w文本框.setStyleSheet("background-color: #e0ffe0;");
		break;
	case E颜色::e蓝:
		w文本框.setStyleSheet("background-color: #e0e0ff;");
		break;
	}
}