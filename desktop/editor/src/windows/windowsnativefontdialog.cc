#include "windows/windowsnativefontdialog.h"

Win32FontDialog::Win32FontDialog(QWidget *parent)
    : QObject(NULL),
      m_cf(NULL)
{
    ZeroMemory(m_cf, sizeof(m_cf));
    m_cf->lStructSize = sizeof(m_cf);

    m_cf->hwndOwner = NULL;
    m_cf->lpLogFont = m_lf;
    m_cf->Flags = 0;

    if(parent)
        m_cf->hwndOwner = (HWND)parent->winId();

    HDC hdc;
    HFONT hfont, hfontPrev;


    //m_cf->rgbColors = rgbCurrent;
    //m_cf->Flags = CF_SCREENFONTS | CF_EFFECTS;

    //if (ChooseFont(&cf)==TRUE)
        /* hfont = CreateFontIndirect(cf.lpLogFont);
        hfontPrev = SelectObject(hdc, hfont);
        rgbCurrent= cf.rgbColors;
        rgbPrev = SetTextColor(hdc, rgbCurrent); */

}

Win32FontDialog::~Win32FontDialog()
{
    if(m_cf)
    {
        free(m_cf);
        m_cf = NULL;
    }
}

int Win32FontDialog::exec()
{
    BOOL result = ChooseFont(m_cf);

    return 1;
}
