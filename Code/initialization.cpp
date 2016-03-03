# include "head.h"
# include "global_value.h"									//包含了所有全局变量的拓展
# include "definition.h"
#include <tchar.h>



//外部函数作用域拓展
extern LRESULT CALLBACK WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);			//消息处理函数

//本文件中函数声明-------------------------------------------------------------
//1.初始化direct3d
HRESULT Init_Direct3D(HWND hWnd);		
//顶点的各个操作
extern INT Buffer();


//窗口初始化函数
//7-25
//初始化窗口
//两个参数都来自于WinMain的形参
//包括了初始化HWND句柄，窗口，注册窗口，显示更新窗口
HWND Init_Window(HINSTANCE hInstance,int nShowCmd)
{
	//初始化窗口类，详见P10																													
	wndclass.cbSize = sizeof( WNDCLASSEX ) ;								//增加
	wndclass.cbClsExtra = 0;												//用于指向应用程序窗口的消息处理函数，通常不适用，设为0
	wndclass.cbWndExtra = 0;												//与上相同
	wndclass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);			//用于指定显示窗口时客户区的背景色，默认为白色
	wndclass.hCursor = LoadCursor(NULL,IDC_ARROW);							//指定光标形状
	wndclass.hIcon=(HICON)::LoadImage(NULL,_T("陆雪琪(修改).ico"),IMAGE_ICON,0,0,LR_DEFAULTSIZE|LR_LOADFROMFILE);	//增加
					///												//从全局的::LoadImage函数从本地加载自定义ico图标
	wndclass.hInstance = hInstance;											//保存应用程序实例的句柄，通常从winmain中得来
	wndclass.lpfnWndProc = WndProc;											//用于指向应用程序窗口的消息处理函数
	wndclass.lpszClassName = g_pClassName;									//窗口类名结构的名称
	wndclass.lpszMenuName = NULL;											//窗口菜单的名称
	wndclass.style = CS_HREDRAW | CS_VREDRAW;								//指定窗口样式，包括边框，标题栏，滚动条等
	
	//注册窗口类，详见P11
	if(::RegisterClassEx(&wndclass) == 0)
		return 0 ;
	
	//创建窗口，详见P11
	HWND hWnd = CreateWindow(
		g_pClassName,								//窗口类名
		g_pWindowName,								//窗口标题
		WS_OVERLAPPEDWINDOW | WS_VSCROLL,			//窗口样式
		20,											//窗口最初x的位置
		20,											//窗口最初y的位置
		SCREEN_WIDTH,								//窗口的宽度
		SCREEN_HEIGHT,								//窗口的高度
		NULL,										//父窗口的话柄
		NULL,										//窗口的菜单话柄
		hInstance,									//应用程序实例话柄
		NULL);										//创建窗口的参数
	
	//显示窗口，更新窗口
	ShowWindow(hWnd,nShowCmd);
	UpdateWindow(hWnd);	
	Init_Direct3D(hWnd);							//初始化direct3d


return hWnd;
}

//初始化direct3D
//参数为WinMain的形参得来
HRESULT Init_Direct3D(HWND hWnd)
{
	//创建COM接口，IDirect3D9的指针
	LPDIRECT3D9 pD3D = NULL;
	if( (pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
		return E_FAIL;

	D3DCAPS9 caps;									
	int sign = 0;

	//获取硬件信息，检验硬件时候支持硬件计算,获取后的信息需要记录，创建接口的时候需要用到
	pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&caps);
	if(caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		sign = D3DCREATE_MIXED_VERTEXPROCESSING;			//支持硬件顶点运算，使用混合计算方式
	else
		sign = D3DCREATE_SOFTWARE_VERTEXPROCESSING;			//不支持，采用软件顶点运算

	//创建Direct3d设备接口	详细内容 P34
	D3DPRESENT_PARAMETERS d3dpp;

	ZeroMemory(&d3dpp,sizeof(d3dpp));								//将内存清零
	d3dpp.BackBufferWidth = SCREEN_WIDTH;									//后台缓冲区的宽度
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;									//后台缓冲区的高度
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;						//后台缓冲区保存像素的格式，等号右边是一个枚举类型
	d3dpp.BackBufferCount = 1;										//后台缓冲区的数量
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;					//多重采样的类型
	d3dpp.MultiSampleQuality = 0;									//多重采样的格式
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;						//指定后台缓冲区的内容如何复制到前台缓冲区，通常设为此值，
																	//表示当后台缓冲区复制到前台时清除后台缓冲区内容
	d3dpp.hDeviceWindow = hWnd;	
	d3dpp.Windowed = true;											//指定绘制图形的窗口句柄
	d3dpp.EnableAutoDepthStencil = true;							//表示Direct3D是否为应用程序自动管理深度缓存，
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;					//为前者成员指定深度缓冲的像素格式
	d3dpp.Flags = 0;												//表示附加属性
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;	
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;		//用于指定平面的翻转模式，窗口模式下，取值为0
	
	//创建IDirect3DDevice9接口对象		P31例程 P35详解
	pD3D->CreateDevice(D3DADAPTER_DEFAULT,					//显卡类型
					   D3DDEVTYPE_HAL,						
					   hWnd,								
					   sign,									//指定Direct3D设备进行3D运算的方式
					   &d3dpp,
					   &g_pd3dDevice);

	pD3D->Release();											//释放Direct3D接口
return S_OK;
}