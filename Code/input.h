# ifndef _INPUT_H
# define _INPUT_H

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
# include "head.h"


class ENTER
{
public:
	LPDIRECTINPUTDEVICE8 _input_device;
	REFGUID _guid;
	LPCDIDATAFORMAT	_format;

	//º¯Êý
	ENTER(HWND,HINSTANCE hinstance,REFGUID guid,LPCDIDATAFORMAT format);
	~ENTER();
	virtual BOOL ReadInput() = 0;
};

//Êó±ê
class MOUSECLASS:public ENTER
{
public:
	DIMOUSESTATE _mouse_state;
	MOUSECLASS(HWND,HINSTANCE hinstance);
	virtual BOOL ReadInput();
	~MOUSECLASS();
};

//¼üÅÌ
class KEYBOARDCLASS:public ENTER
{
public:
	char _key_state[256];
	KEYBOARDCLASS(HWND hWnd,HINSTANCE hinstance);
	virtual BOOL ReadInput();
	~KEYBOARDCLASS();
};

# endif 