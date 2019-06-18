#include "qt-openzwave/qtozw_pods.h"



OptionList::OptionList()
{
}
OptionList::~OptionList()
{
}
bool OptionList::operator!=(OptionList &c2)
{
    return this->selected != c2.selected;
}
void OptionList::setEnums(QStringList list)
{
    this->enumnames = list;
}
QStringList OptionList::getEnums()
{
    return this->enumnames;
}
QString OptionList::getSelectedName()
{
    return this->enumnames.at(this->selected);
}
int OptionList::getSelected()
{
    return this->selected;
}
bool OptionList::setSelected(int index)
{
    if (!this->enumnames.value(index).isEmpty()) {
        this->selected = index;
        return true;
    }
    return false;
}
bool OptionList::setSelected(QString value)
{
    if (this->enumnames.count(value) > 0) {
        this->selected = this->enumnames.indexOf(value);
        return true;
    }
    return false;
}
