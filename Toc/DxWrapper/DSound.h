//---------------------------------
//  ������  CDSound
//  ������  DirectSound ��װ��
//  ģʽ��  ����
//---------------------------------



#ifndef _CDSOUND_H
#define _CDSOUND_H


#include <windows.h>
#include <dsound.h>

namespace DxWrapper
{
    class CDSound
    {
    public:
        bool Init(HWND hWnd);       //  �����ʼ��


    private:
        CDSound();
        ~CDSound();

        LPDIRECTSOUND m_lpDS;




    };

};


#endif

