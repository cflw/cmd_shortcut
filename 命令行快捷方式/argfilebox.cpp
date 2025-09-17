#include "argfilebox.h"
#include <QFileDialog>
#include "file_command.h"
W参数文件框::W参数文件框(QWidget *a父, const S命令参数 &a参数):
	QWidget(a父),
	w文本框(new W参数文本框(this)),
	w文件按钮(new QPushButton(this)),
	w布局(new QHBoxLayout(this)),
	m参数(&a参数) {
	//初始化控件
	w文本框->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	w文件按钮->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	w文件按钮->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::FolderOpen));
	w布局->addWidget(w文本框.get());
	w布局->addWidget(w文件按钮.get());
	w布局->setContentsMargins({0, 0, 0, 0});
	setLayout(w布局.get());
	show();
	//连接槽
	connect(w文件按钮.get(), &QPushButton::clicked, this, &W参数文件框::on_filebutton_clicked);
}
QString W参数文件框::text() const {
	return w文本框->text();
}
void W参数文件框::setText(const QString &a) {
	w文本框->setText(a);
}
QSize W参数文件框::sizeHint() const {
	const QSize v文本框尺寸 = w文本框->sizeHint();
	const QSize v文件按钮尺寸 = w文件按钮->sizeHint();
	return QSize{
		v文本框尺寸.width() + v文件按钮尺寸.width(),
		std::max(v文本框尺寸.height(), v文件按钮尺寸.height())
	};
}
void W参数文件框::setPlaceholderText(const QString &a) {
	w文本框->setPlaceholderText(a);
}
W参数文本框 &W参数文件框::fg文本框() const {
	return *w文本框;
}
//槽
void W参数文件框::on_filebutton_clicked() {
	QString v文件名 = QFileDialog::getOpenFileName(this, "选择一个文件", QDir::currentPath(), m参数->m过滤器);
	if (!v文件名.isEmpty()) {
		w文本框->setText(v文件名);
		emit w文本框->textEdited(v文件名);
	}
}