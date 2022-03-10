////////////////////////////////////////////////////////////////////////////
//
//  声明：此文件属于游戏创造传说的一部分，全部版权归属 COKUCO 工作室所有
//        如有侵权，COKUCO 工作室保留法律起诉权利
//
//  类名：CDInput （C - Class  DInput - DirectInput）
//  模式：Singleton 唯一实例
//  描述：DirectInput 封装类，可以非常方便的进行 DirectInput 的操作
//        目前支持键盘设备和手柄设备，可以获得按键的状态。
//
//  作者：Kudoxinyiji
//  日期：2005-05-22
//
////////////////////////////////////////////////////////////////////////////



#ifndef _CDINPUT_H
#define _CDINPUT_H


#define DIRECTINPUT_VERSION 0x0800

#include <windows.h>
#include <dinput.h>


#pragma comment(lib, "dinput.lib")
#pragma comment(lib, "dinput8.lib")




namespace DxWrapper
{

    class CDInput
    {
    public:
        static bool InitInstance();
        static void ExitInstance();
		static inline CDInput* Instance()
		{	return m_pInput;	}

        //  初始化键盘设备
        bool Init_Keyboard(HWND hWnd);

        //  初始化脚柄设备
        bool Init_Joystick(HWND hWnd);

        //  终止键盘设备
        void Term_Keyboard();

        //  终止脚柄设备
        void Term_Joystick();

        //  更新键盘数据
        bool UpdateKeyData();

        //  更新脚柄数据
        bool UpdateJoyData();

        //  获取设备
        bool Acquire();

        //  归还设备
        void UnAcquire();

        //  键盘按键是否按下
        bool IsKeyDown (const long&);
        
        //  键盘按键是否正在被按下
        bool IsKeyPress(const long&);
        
        //  脚柄按键正在被按下时返回 true
        bool IsJoyDown (const long&);
        
        //  脚柄按键按下时返回 true
        bool IsJoyPress(const long&);

        //  获得脚柄 X 轴的状态信息
        long GetJoyAxisX();
        
        //  获得脚柄 Y 轴的状态信息
        long GetJoyAxisY();

    private:
        CDInput();
        ~CDInput();

        static BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext);
        static BOOL CALLBACK EnumObjectsCallback(const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext);

    private:
        //  主 DirectInput8 对象引用
        LPDIRECTINPUT8 m_lpDI;

        //  DirectInput8 键盘设备
        LPDIRECTINPUTDEVICE8 m_lpDIKeyboard;

        //  DirectInput8 脚柄设备
        LPDIRECTINPUTDEVICE8 m_lpDIJoystick;

        //  指向当前键盘数据
        BYTE* m_pKeyStateNow;

        //  指向前一帧键盘数据
        BYTE* m_pKeyStatePrev;

        //  指向当前的脚柄数据
        DIJOYSTATE2* m_pJoyStateNow;
        
        //  指向前一帧的脚柄数据
        DIJOYSTATE2* m_pJoyStatePrev;

        //  CDInput 唯一实例的指针
        static CDInput* m_pInput;

    };

};


#endif

