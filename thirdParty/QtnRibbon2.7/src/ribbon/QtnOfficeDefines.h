/****************************************************************************
**
** Qtitan Library by Developer Machines
** 
** Copyright (c) 2009-2012 Developer Machines (http://www.devmachines.com)
**           ALL RIGHTS RESERVED
** 
**  The entire contents of this file is protected by copyright law and
**  international treaties. Unauthorized reproduction, reverse-engineering
**  and distribution of all or any portion of the code contained in this
**  file is strictly prohibited and may result in severe civil and 
**  criminal penalties and will be prosecuted to the maximum extent 
**  possible under the law.
**
**  RESTRICTIONS
**
**  THE SOURCE CODE CONTAINED WITHIN THIS FILE AND ALL RELATED
**  FILES OR ANY PORTION OF ITS CONTENTS SHALL AT NO TIME BE
**  COPIED, TRANSFERRED, SOLD, DISTRIBUTED, OR OTHERWISE MADE
**  AVAILABLE TO OTHER INDIVIDUALS WITHOUT WRITTEN CONSENT
**  AND PERMISSION FROM DEVELOPER MACHINES
**
**  CONSULT THE END USER LICENSE AGREEMENT FOR INFORMATION ON
**  ADDITIONAL RESTRICTIONS.
**
****************************************************************************/
#ifndef OFFICEDEFINES_H
#define OFFICEDEFINES_H

namespace Qtitan
{
    #define QTN_VERSION_RIBBON_STR "2.7"

    #define qtn_Prop_Index          "Index"
    #define qtn_Prop_Group          "Group"
    #define qtn_Prop_Wrap           "Wrap"
    #define qtn_Prop_Wrap_Group     "WrapGroup"
    #define qtn_Prop_Begin_Group    "BeginGroup"

    #define qtn_PopupButtonGallery        "PopupButtonGallery"
    #define qtn_ScrollUpButtonGallery     "ScrollUpButtonGallery"
    #define qtn_ScrollDownButtonGallery   "ScrollDownButtonGallery"

    #define qtn_PopupBar                  "PopupBar"
    #define qtn_WidgetGallery             "WidgetGallery"

    enum BarType
    {
        TypeMenuBar,
        TypeNormal,
        TypePopup,
        TypeRibbon
    };

    enum BarPosition
    {
        BarTop,
        BarBottom,
        BarLeft,
        BarRight,
        BarFloating,
        BarPopup,
        BarListBox,
        BarNone
    };

}; //namespace Qtitan

#endif //OFFICEDEFINES_H

