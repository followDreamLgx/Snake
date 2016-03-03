# include "input.h"

ENTER::ENTER(HWND hwnd,HINSTANCE hinstance,REFGUID guid,LPCDIDATAFORMAT format):_guid(guid),_format(format)
{
	//创建IDirectInput8接口对象
	LPDIRECTINPUT8 direct_input = NULL;
	HRESULT hr;
	hr = DirectInput8Create(
		hinstance,
		0x0800,						//通常可以取该值
		IID_IDirectInput8,			//
		(void**)&direct_input,		//
		NULL);						//
	if(FAILED(hr))
	{
		::MessageBox(0,L"Create IDirectInput8 - FAILED",0,0);
	}
	 
	//创建IDirectInputDevice8接口对象，
	//_guid在初始化时指定是鼠标还是键盘
	_input_device = NULL;
	hr = direct_input->CreateDevice(_guid,
		&_input_device,
		NULL);
	if(FAILED(hr))
	{
		direct_input->Release();
		::MessageBox(0,L"Create IDirectInputDevice8 - FAILED",0,0);
	}

	//设置协作级别					
	hr = _input_device->SetCooperativeLevel(hwnd,DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if(FAILED(hr))								
	{
		direct_input->Release();
		_input_device->Release();
		::MessageBox(0,L"Create operator level - FAILED",0,0);
	}

	//设置设备数据格式
	hr = _input_device->SetDataFormat(_format);
	if(FAILED(hr))
	{
		direct_input->Release();
		_input_device->Release();
		::MessageBox(0,L"Create data format - FAILED",0,0);
	}

	//获取设备
	hr = _input_device->Acquire();
	_input_device->Poll();
}

ENTER::~ENTER()
{
	_input_device->Release();
}


//-----------------------------------------
//鼠标
//-----------------------------------------
MOUSECLASS::MOUSECLASS(HWND hwnd,HINSTANCE hinstance):ENTER(hwnd,hinstance,GUID_SysMouse,&c_dfDIMouse)
{
}
MOUSECLASS::~MOUSECLASS()
{
	if(_input_device != NULL)
		_input_device->Unacquire();
}
BOOL MOUSECLASS::ReadInput()
{
	ZeroMemory(&_mouse_state,sizeof(_mouse_state));
	_input_device->Acquire();  
	//注意，该函数成功时不会返回S_OK，不能用FAILED测试
	//获取鼠标输入消息
	HRESULT hr = _input_device->GetDeviceState(sizeof(_mouse_state), (LPVOID)&_mouse_state);	//读取设备数据	
	if(SUCCEEDED(hr))
		 return true;
return  false;
}

//--------------------------------------
//键盘
//--------------------------------------
KEYBOARDCLASS::KEYBOARDCLASS(HWND hwnd,HINSTANCE hinstance):ENTER(hwnd,hinstance,GUID_SysKeyboard,&c_dfDIKeyboard)
{}
KEYBOARDCLASS::~KEYBOARDCLASS()
{
	if(_input_device != NULL)
		_input_device->Unacquire();
}
BOOL KEYBOARDCLASS::ReadInput()
{
	ZeroMemory(_key_state,sizeof(_key_state));
	_input_device->Poll();
	_input_device->Acquire();
	//获取键盘输入消息
	HRESULT hr = _input_device->GetDeviceState(sizeof(_key_state), (LPVOID)_key_state);			//读取设备数据
	return true;
}