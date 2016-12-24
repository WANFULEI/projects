/****************************************************************************
**
** Qtitan Library by Developer Machines (Advanced Ribbon for Qt)
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
#include <windows.h>

#include "QtnCommonStylePrivate.h"

using namespace Qtitan;


static QImage qtn_fromWinHBITMAP(HDC hdc, HBITMAP bitmap, int w, int h)
{
    BITMAPINFO bmi;
    memset(&bmi, 0, sizeof(bmi));
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = w;
    bmi.bmiHeader.biHeight      = -h;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage   = w * h * 4;

    QImage image(w, h, QImage::Format_ARGB32_Premultiplied);
    image.fill(0);
    if (image.isNull())
        return image;

    // Get bitmap bits
    uchar* data = (uchar*)qMalloc(bmi.bmiHeader.biSizeImage);

    if (GetDIBits(hdc, bitmap, 0, h, data, &bmi, DIB_RGB_COLORS)) 
    {

        // Create image and copy data into image.
        for (int y = 0; y < h; ++y) 
        {
            void* dest = (void*) image.scanLine(y);
            void* src = data + y * image.bytesPerLine();
            memcpy(dest, src, image.bytesPerLine());
        }
    } 
    else 
    {
        qWarning("qtn_fromWinHBITMAP(), failed to get bitmap bits");
    }
    qFree(data);

    return image;
}

QPixmap StyleHelper::alphaBlend(const QPixmap& src)
{
//    HBITMAP winBitmap1 = src.toWinHBITMAP(QPixmap::Alpha);
//    return QPixmap::fromWinHBITMAP(winBitmap1, QPixmap::PremultipliedAlpha);

    bool foundAlpha = false;
    HDC screenDevice = GetDC(0);
    HDC hdc = CreateCompatibleDC(screenDevice);
    ReleaseDC(0, screenDevice);

    int w = src.width();
    int h = src.height();

    HBITMAP winBitmap = src.toWinHBITMAP(QPixmap::Alpha);
    HGDIOBJ oldhdc = (HBITMAP)SelectObject(hdc, winBitmap);

    QImage image = qtn_fromWinHBITMAP(hdc, winBitmap, w, h);

    for (int y = 0 ; y < h && !foundAlpha ; y++) 
    {
        QRgb* scanLine = reinterpret_cast<QRgb*>(image.scanLine(y));
        for (int x = 0; x < w ; x++) 
        {
            if (qAlpha(scanLine[x]) != 0) 
            {
                foundAlpha = true;
                break;
            }
        }
    }

    if (!foundAlpha) 
    {
        //If no alpha was found, we use the mask to set alpha values
        QImage mask = qtn_fromWinHBITMAP(hdc, winBitmap, w, h);

        for (int y = 0 ; y < h ; y++)
        {
            QRgb* scanlineImage = reinterpret_cast<QRgb*>(image.scanLine(y));
            QRgb* scanlineMask = mask.isNull() ? 0 : reinterpret_cast<QRgb*>(mask.scanLine(y));
            for (int x = 0; x < w ; x++)
            {
                if (scanlineMask && qRed(scanlineMask[x]) != 0)
                    scanlineImage[x] = 0; //mask out this pixel
                else
                    scanlineImage[x] |= 0xff000000; // set the alpha channel to 255
            }
        }
    }

    SelectObject(hdc, oldhdc); //restore state
    DeleteObject(winBitmap);
    DeleteDC(hdc);

    return QPixmap::fromImage(image);
}

