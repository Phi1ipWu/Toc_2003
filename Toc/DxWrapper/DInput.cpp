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



#include "DInput.h"
using namespace DxWrapper;




//  CDInput Ψһʵ����ʼ��
CDInput* CDInput::m_pInput = 0;




//-----------------------------------------------------
//  ������ CDInput Ψһʵ�����ڴ���䣨��̬��
//  ���ڣ� 2005/05/04
//-----------------------------------------------------
bool CDInput::InitInstance()
{
    if(0 == m_pInput)
    {
        m_pInput = new CDInput();
    }

    return true;
}




//-----------------------------------------------------
//  ������ CDInput Ψһʵ�����ڴ���գ���̬��
//  ���ڣ� 2005/05/04
//-----------------------------------------------------
void CDInput::ExitInstance()
{
    if(m_pInput)
    {
        delete m_pInput;
        m_pInput = 0;   
    }
}




//-----------------------------------------------------
//  ������ ��� CDInput Ψһʵ������̬��
//  ���ڣ� 2005/05/04
//  ����ֵ�� CDInput Ψһʵ����ָ��
//-----------------------------------------------------
//CDInput* CDInput::Instance()
//{
//    return m_pInput;
//}




//-----------------------------------------------------
//  ������ CDInput ��Ĺ��캯��
//  ���ڣ� 2005/05/22
//-----------------------------------------------------
CDInput::CDInput()
{
    m_lpDI          = 0;
    m_lpDIKeyboard  = 0;
    m_lpDIJoystick  = 0;

    m_pKeyStateNow  = new BYTE[256];
    m_pKeyStatePrev = new BYTE[256];
    m_pJoyStateNow  = new DIJOYSTATE2;
    m_pJoyStatePrev = new DIJOYSTATE2;

    memset(m_pKeyStateNow,  0, 256);
    memset(m_pKeyStatePrev, 0, 256);
    memset(m_pJoyStateNow,  0, sizeof(DIJOYSTATE2));
    memset(m_pJoyStatePrev, 0, sizeof(DIJOYSTATE2));
}




//-----------------------------------------------------
//  ������ CDInput �����������
//  ���ڣ� 2005/05/22
//-----------------------------------------------------
CDInput::~CDInput()
{
    Term_Keyboard();
    Term_Joystick();
    
    if(m_pKeyStateNow)
    {
        delete[] m_pKeyStateNow;
        m_pKeyStateNow = 0;
    }

    if(m_pKeyStatePrev)
    {
        delete[] m_pKeyStatePrev;
        m_pKeyStatePrev = 0;
    }

    if(m_pJoyStateNow)
    {
        delete m_pJoyStateNow;
        m_pJoyStateNow = 0;
    }
    
    if(m_pJoyStatePrev)
    {
        delete m_pJoyStatePrev;
        m_pJoyStatePrev = 0;
    }
}




//-----------------------------------------------------
//  ������ DirectInput �����豸����ֹ��ʽ
//  ���ڣ� 2005/05/13
//-----------------------------------------------------
void CDInput::Term_Keyboard()
{
    if(m_lpDI)
    {
        if(m_lpDIKeyboard)
        {
            m_lpDIKeyboard->Unacquire();
            m_lpDIKeyboard->Release();
            m_lpDIKeyboard = 0;
        }

        if(0 == m_lpDIJoystick)
        {
            //  ����ű��豸���ͷţ��� DI ����Ҳ�ͷ�
            m_lpDI->Release();
            m_lpDI = 0;
        }
    }
}




//-----------------------------------------------------
//  ������ DirectInput �ű��豸����ֹ��ʽ
//  ���ڣ� 2005/05/13
//-----------------------------------------------------
void CDInput::Term_Joystick()
{
    if(m_lpDI)
    {
        if(m_lpDIJoystick)
        {
            m_lpDIJoystick->Unacquire();
            m_lpDIJoystick->Release();
            m_lpDIJoystick = NULL;
        }

        if(0 == m_lpDIKeyboard)
        {
            //  ��������豸���ͷţ��� DI ����Ҳ�ͷ�
            m_lpDI->Release();
            m_lpDI = NULL;
        }
    }
}




//-----------------------------------------------------
//  ������ DirectInput �����豸����ĳ�ʼ��
//  ���ڣ� 2005/05/12
//  ������ HWND hWnd [in] ��������
//  ����ֵ�� true ��ʼ���ɹ���false ��ʼ��ʧ��
//-----------------------------------------------------
bool CDInput::Init_Keyboard(HWND hWnd)
{

    //  ����� DI ����δ��ʼ��
    if(0 == m_lpDI)
    {
        //  ������ DirectInput COM ����
        if(DI_OK != DirectInput8Create(GetModuleHandle(0),
                                       DIRECTINPUT_VERSION, 
                                       IID_IDirectInput8, 
                                       (void**)&m_lpDI, 0))
        {
            return false;
        }
    }

    //  ��������豸δ��ʼ��
    if(0 == m_lpDIKeyboard)
    {
        //  ���������豸
        if(DI_OK != (m_lpDI->CreateDevice(GUID_SysKeyboard, &m_lpDIKeyboard, 0)))
        {
            Term_Keyboard();
            return false;   
        }

        //  ���ü����豸���ݸ�ʽ
        if(DI_OK != (m_lpDIKeyboard->SetDataFormat(&c_dfDIKeyboard)))
        {
            Term_Keyboard();
            return false;
        }

        //  ���ü����豸Э���ȼ����Ƕ�ռ����̨��
        if(DI_OK != (m_lpDIKeyboard->SetCooperativeLevel(hWnd, 
                    DISCL_NONEXCLUSIVE | DISCL_BACKGROUND)))
        {
            Term_Keyboard();
            return false;
        }

        //  ��ȡ����
        if(DI_OK != (m_lpDIKeyboard->Acquire()))
        {
            Term_Keyboard();
            return false;   
        }
    }

    //  ���󴴽������õĹ�����û�г���
    //  ���� true ��ʾ�ɹ�
    return true;
}




//-----------------------------------------------------
//  ������ DirectInput �ű��豸����ĳ�ʼ��
//  ���ڣ� 2005/05/13
//  ������ HWND hWnd [in]  ��������
//  ����ֵ�� true ��ʼ���ɹ���false ��ʼ��ʧ��
//-----------------------------------------------------
bool CDInput::Init_Joystick(HWND hWnd)
{
    //  ����� DI ����δ��ʼ��
   if(0 == m_lpDI)
    {
        //  ������ DirectInput COM ����
        if(DI_OK != DirectInput8Create(GetModuleHandle(NULL),
                                       DIRECTINPUT_VERSION, 
                                       IID_IDirectInput8, 
                                       (void**)&m_lpDI, 0))
        {
            return false;
        }
    }

    //  ����ű��豸δ��ʼ��
   if(0 == m_lpDIJoystick)
    {
        //  ö�ٽű�
        if(DI_OK != m_lpDI->EnumDevices(DI8DEVTYPE_JOYSTICK, EnumJoysticksCallback, 0, DIEDFL_ATTACHEDONLY))
        {
            Term_Joystick();
            return false;
        }

        //  ���ö�ٺ�Ľű��豸����
        if(0 == m_lpDIJoystick)
        {
            Term_Joystick();
            return false;
        }

        //  �������ݸ�ʽ��ʹ�����¸�ʽ c_dfDIJoystick2��֧��������
        if(DI_OK != m_lpDIJoystick->SetDataFormat(&c_dfDIJoystick2))
        {
            Term_Joystick();
            return false;
        }

        //  �豸Э���ȼ�
        if(DI_OK != m_lpDIJoystick->SetCooperativeLevel(hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND))
        {
            Term_Joystick();
            return false;
        }

        if(DI_OK != m_lpDIJoystick->EnumObjects(EnumObjectsCallback, (void*)hWnd, DIDFT_ALL))
        {
            Term_Joystick();
            return false;
        }
    }

    //  ���󴴽������õĹ�����û�г���
    //  ���� true ��ʾ�ɹ�����������
    return true;
}




//-----------------------------------------------------
//  ������ ��ȡ���̰���״̬����Ϸ��ÿ��ѭ���ʼ����
//  ���ڣ� 2005/05/15
//  ����ֵ�� true ���³ɹ���false ����ʧ��
//-----------------------------------------------------
bool CDInput::UpdateKeyData()
{
    HRESULT hr;
    BYTE* pKeyStateTemp;

    //  �������ݻ���ָ�뽻�������Ա���ǰһ֡������
    pKeyStateTemp   = m_pKeyStateNow;
    m_pKeyStateNow  = m_pKeyStatePrev;
    m_pKeyStatePrev = pKeyStateTemp;

    while(hr = m_lpDIKeyboard->GetDeviceState(256,
            (void*)m_pKeyStateNow) == DIERR_INPUTLOST)
    {
        if(FAILED(hr = m_lpDIKeyboard->Acquire()))
        {
            break;
        }
    }

    if(FAILED(hr))
    {
        return false;
    }

    return true;
}




//-----------------------------------------------------
//  ������ ��ȡ�ű�����״̬����Ϸ��ÿ��ѭ���ʼ����
//  ���ڣ� 2005/05/15
//  ����ֵ�� true ���³ɹ���false ����ʧ��
//-----------------------------------------------------
bool CDInput::UpdateJoyData()
{
    HRESULT hr;

    if(0 == m_lpDIJoystick)
    {
        return true;
    }

    // Poll the device to read the current state
    hr = m_lpDIJoystick->Poll(); 

    if(FAILED(hr))  
    {
        // DInput is telling us that the input stream has been
        // interrupted. We aren't tracking any state between polls, so
        // we don't have any special reset that needs to be done. We
        // just re-acquire and try again.
        hr = m_lpDIJoystick->Acquire();

        while(hr == DIERR_INPUTLOST) 
        {
            hr = m_lpDIJoystick->Acquire();
        }

        // hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
        // may occur when the app is minimized or in the process of 
        // switching, so just try again later 
        return true; 
    }

    //  ��ʱ����������������ָ�뽻��
    DIJOYSTATE2* pJoyStateTemp;

    pJoyStateTemp   = m_pJoyStateNow;
    m_pJoyStateNow  = m_pJoyStatePrev;
    m_pJoyStatePrev = pJoyStateTemp;

    //  ��ýű������״̬
    if(FAILED(hr = m_lpDIJoystick->GetDeviceState(sizeof(DIJOYSTATE2), m_pJoyStateNow)))
    {
        // The device should have been acquired during the Poll()
        return false;
    }

    return true;
}




//-----------------------------------------------------
//  ������ ��ü��̰���״̬
//  ���ڣ� 2005/05/15
//  ������ long nDIKey DI���룬�μ� dinput.h
//  ����ֵ�� true �����£�false ��δ����
//-----------------------------------------------------
bool CDInput::IsKeyDown(const long& nDIKey)
{
//    if(0 <= nDIKey && nDIKey < 256)
//    {
        return ((m_pKeyStateNow[nDIKey] & 0x80) == 0x80);
//    }

//    return false;
}




//-----------------------------------------------------
//  ������ ��ýű�����״̬
//  ���ڣ� 2005/05/15
//  ������ long nDIKey �ֱ��İ�ť���
//  ����ֵ�� true �����£�false ��δ����
//-----------------------------------------------------
inline bool CDInput::IsJoyDown(const long& nDIKey)
{
    if(0 <= nDIKey && nDIKey < 128)
    {
        return ((m_pJoyStateNow->rgbButtons[nDIKey] & 0x80) == 0x80);
    }

    return false;
}




//-----------------------------------------------------
//  ������ ��ü����Ƿ����ڱ�����
//  ���ڣ� 2005/05/15
//  ������ long nDIKey DI���룬�μ� dinput.h
//  ����ֵ�� true �����£�false ��δ����
//-----------------------------------------------------
bool CDInput::IsKeyPress(const long& nDIKey)
{
    if(0 <= nDIKey && nDIKey < 256)
    {
        return (((m_pKeyStateNow [nDIKey] & 0x80) == 0x80) &&
                ((m_pKeyStatePrev[nDIKey] & 0x80) == 0x00));
    }

    return false;
}




//-----------------------------------------------------
//  ������ ���ؼ����Ƿ����ڱ�����
//  ���ڣ� 2005/05/15
//  ������ long nDIKey �ֱ��İ�ť���
//  ������ ��
//-----------------------------------------------------
bool CDInput::IsJoyPress(const long& nDIKey)
{
    if(0 <= nDIKey && nDIKey < 128)
    {
        return (((m_pJoyStateNow->rgbButtons [nDIKey] & 0x80) == 0x80) &&
                ((m_pJoyStatePrev->rgbButtons[nDIKey] & 0x80) == 0x00));
    }

    return false;
}




//-----------------------------------------------------
//  ������ ��ýű�ʮ�ּ� X ���״̬
//  ���ڣ� 2005/05/15
//  ����ֵ�� ��-1  �ң�1  δ����0
//-----------------------------------------------------
long CDInput::GetJoyAxisX()
{
    //  X ����ѿ�������ϵ��ͬ

    if(m_pJoyStateNow->lX < -32)
    {
        //  ��
        return -1;  
    }
    
    if(m_pJoyStateNow->lX > 32)
    {
        //  ��
        return 1;   
    }

    //  δ����
    return 0;
}




//-----------------------------------------------------
//  ������ ��ýű�ʮ�ּ� Y ���״̬
//  ���ڣ� 2005/05/15
//  ����ֵ�� �ϣ�-1  �£�1  δ����0
//-----------------------------------------------------
long CDInput::GetJoyAxisY()
{
    //  Y ����ѿ�������ϵ�෴��
    //  ���ϵ�СΪ����

    if(m_pJoyStateNow->lY < -32)
    {
        //  ��
        return -1;  
    }

    if(m_pJoyStateNow->lY > 32)
    {
        //  ��
        return 1;   
    }

    //  δ����
    return 0;
}




//-----------------------------------------------------
//  ������ ��ȡ�豸���ⲿ���ã�
//  ���ڣ� 2005/05/12
//-----------------------------------------------------
bool CDInput::Acquire()
{
    if(m_lpDIKeyboard)
    {
        if(DI_OK != m_lpDIKeyboard->Acquire())
        {
            return false;
        }
    }

    if(m_lpDIJoystick)
    {
        if(DI_OK != m_lpDIJoystick->Acquire())
        {
            return false;
        }
    }

    return true;
}




//-----------------------------------------------------
//  ������ �黹�豸���ⲿ���ã�
//  ���ڣ� 2005/05/12
//-----------------------------------------------------
void CDInput::UnAcquire()
{
    if(m_lpDIKeyboard)
    {
        m_lpDIKeyboard->Unacquire();
    }

    if(m_lpDIJoystick)
    {
        m_lpDIJoystick->Unacquire();
    }
}




//-----------------------------------------------------
//  ������ ö�ٵ�һ���ű����ص���ʽ����ճ�Ĵ��룩
//  ���ڣ� 2005/05/13
//  ������ ����
//-----------------------------------------------------
BOOL CALLBACK CDInput::EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext)
{
    // Obtain an interface to the enumerated joystick.
    if(FAILED(m_pInput->m_lpDI->CreateDevice(pdidInstance->guidInstance, &(m_pInput->m_lpDIJoystick), 0)))
    {
        // If it failed, then we can't use this joystick. (Maybe the user unplugged
        // it while we were in the middle of enumerating it.)
        return DIENUM_CONTINUE;
    }

    // Stop enumeration. Note: we're just taking the first joystick we get. You
    // could store all the enumerated joysticks and let the user pick.
    return DIENUM_STOP;
}




//-----------------------------------------------------
//  ������ ���ص���ʽ����ճ�Ĵ��룩
//  ���ڣ� 2005/05/13
//  ������ ����
//-----------------------------------------------------
BOOL CALLBACK CDInput::EnumObjectsCallback(const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext )
{
    static int nSliderCount = 0;  // Number of returned slider controls
    static int nPOVCount = 0;     // Number of returned POV controls

    // For axes that are returned, set the DIPROP_RANGE property for the
    // enumerated axis in order to scale min/max values.
    if(pdidoi->dwType & DIDFT_AXIS)
    {
        DIPROPRANGE diprg; 
        diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
        diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
        diprg.diph.dwHow        = DIPH_BYID; 
        diprg.diph.dwObj        = pdidoi->dwType; // Specify the enumerated axis
        diprg.lMin              = -1000; 
        diprg.lMax              = +1000; 

        // Set the range for the axis
        if(FAILED(m_pInput->m_lpDIJoystick->SetProperty(DIPROP_RANGE, &diprg.diph))) 
        {
            return DIENUM_STOP;
        }
    }

    return DIENUM_CONTINUE;
}


