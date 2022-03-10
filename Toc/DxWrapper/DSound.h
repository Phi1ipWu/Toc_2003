//---------------------------------
//  类名：  CDSound
//  描述：  DirectSound 封装类
//  模式：  单例
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
        bool Init(HWND hWnd);       //  对象初始化


    private:
        CDSound();
        ~CDSound();

        LPDIRECTSOUND m_lpDS;




    };

};


#endif

