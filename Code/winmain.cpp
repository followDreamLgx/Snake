/****************************************
说明
1.函数参数的详细说明以及详细用法在函数的定义处，声明处只说明该函数的作用
2.为了方便查找，所有的全局变量在winmain.cpp中定义，需要用到的时候自行拓展
3.为了查找书签，使用Pn(郑阿奇)和PLn(龙书)的形式记录书签
****************************************/
# include "head.h"										//包含了所有头文件
# include "definition.h"
# include "camera.h"
# include "skybox.h"
# include "Meshxfile.h"
# include "snow.h"
# include "input.h"
# include "SkinMesh.h"
# include "attack.h"
# include "snake.h"
# include "start.h"

wchar_t				*g_pClassName = L"7-25";							//初始化窗口类以及创建窗口的时候用到，必须一致
wchar_t				*g_pWindowName = L"贪吃蛇2014——决战紫禁之巅";			//窗口名称
WNDCLASSEX			wndclass;											//全局变量，此变量在初始化窗口以及撤销窗口的时候都要用到，
LPDIRECT3DDEVICE9	g_pd3dDevice = NULL;								//Direct3DDevice9设备接口
CAMERA				*camera = NULL;

SNOW				*snow = NULL;									//雪花纷飞
KEYBOARDCLASS		*keyboard = NULL;								//键盘
MOUSECLASS			*mouse = NULL;									//鼠标
bool				mousel_down = false;
//-------------贪吃蛇增添部分-------------------//
SKYBOX_DERIVED		*skybox = NULL;
SELECT_SYSTEM		*select_system = NULL;
ATTACK				*attack = NULL;
SNAKE				*snake = NULL;
MESHXFILE *mesh_alter = NULL;										//祭坛
MESHXFILE *mesh_conch = NULL;										//海螺
START	  *start = NULL;

//各个函数声明------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
extern HWND Init_Window(HINSTANCE hInstance,int nShowCmd);
extern HRESULT Init_Direct3D(HWND hWnd);
extern INT Buffer();
extern VOID Direct3DRender();
VOID Direct3DClearUp();
//-------------------------------------------------------------------------------------------------

extern INT Everything(HWND hWnd,HINSTANCE hInstance);
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevinstance,
					LPSTR lpCmdLine,int nShowCmd)
{
	HWND hWnd;
	hWnd = Init_Window(hInstance,nShowCmd);						//初始化窗口，并初始化Direct3D		
	Everything(hWnd,hInstance);									//创建所有物体模型

	//消息循环
	MSG msg;	
	ZeroMemory(&msg,sizeof(msg));
	while (msg.message!=WM_QUIT)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			TranslateMessage(&msg);				//转换消息
			DispatchMessage(&msg);				//投递消息（将消息传递给相应窗口）
		}
		else
		{
			Direct3DRender();					//渲染
		}
	}

	//注销窗口类
	UnregisterClass(g_pClassName,wndclass.hInstance);
return 0;
}

/**************************
)口消息处理函数
**************************/
LRESULT CALLBACK WndProc(HWND hWnd,UINT message,
						WPARAM wParam,LPARAM lParam)
{
	switch (message)
	{
	//客户区重绘消息
	case WM_PAINT:
		ValidateRect(hWnd,NULL);								//更新客户区的显示
		break;
	//窗口销毁消息
	case WM_DESTROY:											//窗口销毁消息
		PostQuitMessage(0);										//退出程序
		Direct3DClearUp();
		break;
	//鼠标相关判断
	case WM_LBUTTONDOWN:
		mousel_down = true;
		break;

	case WM_LBUTTONUP:
		mousel_down = false;
		break;

	case WM_MOUSEMOVE:
		mouse->_mouse_state.lX = LOWORD (lParam);
		mouse->_mouse_state.lY = HIWORD (lParam);
		break;
	}
	//默认的消息处理
	return DefWindowProc(hWnd,message,wParam,lParam);
}

VOID Direct3DClearUp()
{
	if(g_pd3dDevice)
		g_pd3dDevice->Release();									//释放Direct3D9Device设备接口
}
