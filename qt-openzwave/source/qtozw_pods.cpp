//-----------------------------------------------------------------------------
//
//	qtozw_pods.cpp
//
//	Common Structures for QTOZW Wrapper
//
//	Copyright (c) 2019 Justin Hammond <Justin@dynam.ac>
//
//	SOFTWARE NOTICE AND LICENSE
//
//	This file is part of QT-OpenZWave.
//
//	OpenZWave is free software: you can redistribute it and/or modify
//	it under the terms of the GNU Lesser General Public License as published
//	by the Free Software Foundation, either version 3 of the License,
//	or (at your option) any later version.
//
//	OpenZWave is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU Lesser General Public License for more details.
//
//	You should have received a copy of the GNU Lesser General Public License
//	along with OpenZWave.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------

#include "qt-openzwave/qtozw_pods.h"



OptionList::OptionList() :
    enumnames(QStringList()),
    selected(0)
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
