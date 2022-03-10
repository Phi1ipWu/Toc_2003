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



#include "DInput.h"
using namespace DxWrapper;




//  CDInput 唯一实例初始化
CDInput* CDInput::m_pInput = 0;




//-----------------------------------------------------
//  描述： CDInput 唯一实例的内存分配（静态）
//  日期： 2005/05/04
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
//  描述： CDInput 唯一实例的内存回收（静态）
//  日期： 2005/05/04
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
//  描述： 获得 CDInput 唯一实例（静态）
//  日期： 2005/05/04
//  返回值： CDInput 唯一实例的指针
//-----------------------------------------------------
//CDInput* CDInput::Instance()
//{
//    return m_pInput;
//}




//-----------------------------------------------------
//  描述： CDInput 类的构造函数
//  日期： 2005/05/22
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
//  描述： CDInput 类的析构函数
//  日期： 2005/05/22
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
//  描述： DirectInput 键盘设备的终止函式
//  日期： 2005/05/13
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
            //  如果脚柄设备已释放，则将 DI 对象也释放
            m_lpDI->Release();
            m_lpDI = 0;
        }
    }
}




//-----------------------------------------------------
//  描述： DirectInput 脚柄设备的终止函式
//  日期： 2005/05/13
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
            //  如果键盘设备已释放，则将 DI 对象也释放
            m_lpDI->Release();
            m_lpDI = NULL;
        }
    }
}




//-----------------------------------------------------
//  描述： DirectInput 键盘设备对象的初始化
//  日期： 2005/05/12
//  参数： HWND hWnd [in] 主窗体句柄
//  返回值： true 初始化成功，false 初始化失败
//-----------------------------------------------------
bool CDInput::Init_Keyboard(HWND hWnd)
{

    //  如果主 DI 对象未初始化
    if(0 == m_lpDI)
    {
        //  创建主 DirectInput COM 对象
        if(DI_OK != DirectInput8Create(GetModuleHandle(0),
                                       DIRECTINPUT_VERSION, 
                                       IID_IDirectInput8, 
                                       (void**)&m_lpDI, 0))
        {
            return false;
        }
    }

    //  如果键盘设备未初始化
    if(0 == m_lpDIKeyboard)
    {
        //  创建键盘设备
        if(DI_OK != (m_lpDI->CreateDevice(GUID_SysKeyboard, &m_lpDIKeyboard, 0)))
        {
            Term_Keyboard();
            return false;   
        }

        //  设置键盘设备数据格式
        if(DI_OK != (m_lpDIKeyboard->SetDataFormat(&c_dfDIKeyboard)))
        {
            Term_Keyboard();
            return false;
        }

        //  设置键盘设备协作等级（非独占，后台）
        if(DI_OK != (m_lpDIKeyboard->SetCooperativeLevel(hWnd, 
                    DISCL_NONEXCLUSIVE | DISCL_BACKGROUND)))
        {
            Term_Keyboard();
            return false;
        }

        //  获取键盘
        if(DI_OK != (m_lpDIKeyboard->Acquire()))
        {
            Term_Keyboard();
            return false;   
        }
    }

    //  对象创建和设置的过程中没有出错
    //  返回 true 表示成功
    return true;
}




//-----------------------------------------------------
//  描述： DirectInput 脚柄设备对象的初始化
//  日期： 2005/05/13
//  参数： HWND hWnd [in]  主窗体句柄
//  返回值： true 初始化成功，false 初始化失败
//-----------------------------------------------------
bool CDInput::Init_Joystick(HWND hWnd)
{
    //  如果主 DI 对象未初始化
   if(0 == m_lpDI)
    {
        //  创建主 DirectInput COM 对象
        if(DI_OK != DirectInput8Create(GetModuleHandle(NULL),
                                       DIRECTINPUT_VERSION, 
                                       IID_IDirectInput8, 
                                       (void**)&m_lpDI, 0))
        {
            return false;
        }
    }

    //  如果脚柄设备未初始化
   if(0 == m_lpDIJoystick)
    {
        //  枚举脚柄
        if(DI_OK != m_lpDI->EnumDevices(DI8DEVTYPE_JOYSTICK, EnumJoysticksCallback, 0, DIEDFL_ATTACHEDONLY))
        {
            Term_Joystick();
            return false;
        }

        //  检测枚举后的脚柄设备对象
        if(0 == m_lpDIJoystick)
        {
            Term_Joystick();
            return false;
        }

        //  设置数据格式，使用最新格式 c_dfDIJoystick2，支持力回馈
        if(DI_OK != m_lpDIJoystick->SetDataFormat(&c_dfDIJoystick2))
        {
            Term_Joystick();
            return false;
        }

        //  设备协作等级
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

    //  对象创建和设置的过程中没有出错
    //  返回 true 表示成功，哇咔咔咔
    return true;
}




//-----------------------------------------------------
//  描述： 读取键盘按键状态，游戏中每次循环最开始调用
//  日期： 2005/05/15
//  返回值： true 更新成功，false 更新失败
//-----------------------------------------------------
bool CDInput::UpdateKeyData()
{
    HRESULT hr;
    BYTE* pKeyStateTemp;

    //  键盘数据缓冲指针交换，可以保留前一帧的数据
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
//  描述： 读取脚柄按键状态，游戏中每次循环最开始调用
//  日期： 2005/05/15
//  返回值： true 更新成功，false 更新失败
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

    //  临时变量，仅用于两个指针交换
    DIJOYSTATE2* pJoyStateTemp;

    pJoyStateTemp   = m_pJoyStateNow;
    m_pJoyStateNow  = m_pJoyStatePrev;
    m_pJoyStatePrev = pJoyStateTemp;

    //  获得脚柄输入的状态
    if(FAILED(hr = m_lpDIJoystick->GetDeviceState(sizeof(DIJOYSTATE2), m_pJoyStateNow)))
    {
        // The device should have been acquired during the Poll()
        return false;
    }

    return true;
}




//-----------------------------------------------------
//  描述： 获得键盘按键状态
//  日期： 2005/05/15
//  参数： long nDIKey DI键码，参见 dinput.h
//  返回值： true 键按下，false 键未按下
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
//  描述： 获得脚柄按键状态
//  日期： 2005/05/15
//  参数： long nDIKey 手柄的按钮编号
//  返回值： true 键按下，false 键未按下
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
//  描述： 获得键盘是否正在被按下
//  日期： 2005/05/15
//  参数： long nDIKey DI键码，参见 dinput.h
//  返回值： true 键按下，false 键未按下
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
//  描述： 返回键盘是否正在被按下
//  日期： 2005/05/15
//  参数： long nDIKey 手柄的按钮编号
//  参数： 无
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
//  描述： 获得脚柄十字键 X 轴的状态
//  日期： 2005/05/15
//  返回值： 左：-1  右：1  未按：0
//-----------------------------------------------------
long CDInput::GetJoyAxisX()
{
    //  X 轴与笛卡尔坐标系相同

    if(m_pJoyStateNow->lX < -32)
    {
        //  左
        return -1;  
    }
    
    if(m_pJoyStateNow->lX > 32)
    {
        //  右
        return 1;   
    }

    //  未按下
    return 0;
}




//-----------------------------------------------------
//  描述： 获得脚柄十字键 Y 轴的状态
//  日期： 2005/05/15
//  返回值： 上：-1  下：1  未按：0
//-----------------------------------------------------
long CDInput::GetJoyAxisY()
{
    //  Y 轴与笛卡尔坐标系相反。
    //  从上到小为递增

    if(m_pJoyStateNow->lY < -32)
    {
        //  上
        return -1;  
    }

    if(m_pJoyStateNow->lY > 32)
    {
        //  下
        return 1;   
    }

    //  未按下
    return 0;
}




//-----------------------------------------------------
//  描述： 获取设备（外部调用）
//  日期： 2005/05/12
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
//  描述： 归还设备（外部调用）
//  日期： 2005/05/12
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
//  描述： 枚举第一个脚柄（回调函式）（粘的代码）
//  日期： 2005/05/13
//  参数： 不懂
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
//  描述： （回调函式）（粘的代码）
//  日期： 2005/05/13
//  参数： 不懂
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


