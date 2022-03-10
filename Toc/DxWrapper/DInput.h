////////////////////////////////////////////////////////////////////////////
//
//  ���������ļ�������Ϸ���촫˵��һ���֣�ȫ����Ȩ���� COKUCO ����������
//        ������Ȩ��COKUCO �����ұ�����������Ȩ��
//
//  ������CDInput ��C - Class  DInput - DirectInput��
//  ģʽ��Singleton Ψһʵ��
//  ������DirectInput ��װ�࣬���Էǳ�����Ľ��� DirectInput �Ĳ���
//        Ŀǰ֧�ּ����豸���ֱ��豸�����Ի�ð�����״̬��
//
//  ���ߣ�Kudoxinyiji
//  ���ڣ�2005-05-22
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

        //  ��ʼ�������豸
        bool Init_Keyboard(HWND hWnd);

        //  ��ʼ���ű��豸
        bool Init_Joystick(HWND hWnd);

        //  ��ֹ�����豸
        void Term_Keyboard();

        //  ��ֹ�ű��豸
        void Term_Joystick();

        //  ���¼�������
        bool UpdateKeyData();

        //  ���½ű�����
        bool UpdateJoyData();

        //  ��ȡ�豸
        bool Acquire();

        //  �黹�豸
        void UnAcquire();

        //  ���̰����Ƿ���
        bool IsKeyDown (const long&);
        
        //  ���̰����Ƿ����ڱ�����
        bool IsKeyPress(const long&);
        
        //  �ű��������ڱ�����ʱ���� true
        bool IsJoyDown (const long&);
        
        //  �ű���������ʱ���� true
        bool IsJoyPress(const long&);

        //  ��ýű� X ���״̬��Ϣ
        long GetJoyAxisX();
        
        //  ��ýű� Y ���״̬��Ϣ
        long GetJoyAxisY();

    private:
        CDInput();
        ~CDInput();

        static BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext);
        static BOOL CALLBACK EnumObjectsCallback(const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext);

    private:
        //  �� DirectInput8 ��������
        LPDIRECTINPUT8 m_lpDI;

        //  DirectInput8 �����豸
        LPDIRECTINPUTDEVICE8 m_lpDIKeyboard;

        //  DirectInput8 �ű��豸
        LPDIRECTINPUTDEVICE8 m_lpDIJoystick;

        //  ָ��ǰ��������
        BYTE* m_pKeyStateNow;

        //  ָ��ǰһ֡��������
        BYTE* m_pKeyStatePrev;

        //  ָ��ǰ�Ľű�����
        DIJOYSTATE2* m_pJoyStateNow;
        
        //  ָ��ǰһ֡�Ľű�����
        DIJOYSTATE2* m_pJoyStatePrev;

        //  CDInput Ψһʵ����ָ��
        static CDInput* m_pInput;

    };

};


#endif

