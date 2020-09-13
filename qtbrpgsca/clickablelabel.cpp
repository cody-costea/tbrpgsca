//source: https://wiki.qt.io/index.php?title=Clickable_QLabel&oldid=30963
#include "clickablelabel.h"

ClickableLabel::ClickableLabel(QWidget* const parent, Qt::WindowFlags f) : QLabel(parent)
{
    Q_UNUSED(f)
}

ClickableLabel::~ClickableLabel()
{

}

void ClickableLabel::mousePressEvent(QMouseEvent* const event)
{
    Q_UNUSED(event)
    emit clicked();
}
