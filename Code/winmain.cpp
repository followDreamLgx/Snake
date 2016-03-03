/****************************************
˵��
1.������������ϸ˵���Լ���ϸ�÷��ں����Ķ��崦��������ֻ˵���ú���������
2.Ϊ�˷�����ң����е�ȫ�ֱ�����winmain.cpp�ж��壬��Ҫ�õ���ʱ��������չ
3.Ϊ�˲�����ǩ��ʹ��Pn(֣����)��PLn(����)����ʽ��¼��ǩ
****************************************/
# include "head.h"										//����������ͷ�ļ�
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

wchar_t				*g_pClassName = L"7-25";							//��ʼ���������Լ��������ڵ�ʱ���õ�������һ��
wchar_t				*g_pWindowName = L"̰����2014������ս�Ͻ�֮��";			//��������
WNDCLASSEX			wndclass;											//ȫ�ֱ������˱����ڳ�ʼ�������Լ��������ڵ�ʱ��Ҫ�õ���
LPDIRECT3DDEVICE9	g_pd3dDevice = NULL;								//Direct3DDevice9�豸�ӿ�
CAMERA				*camera = NULL;

SNOW				*snow = NULL;									//ѩ���׷�
KEYBOARDCLASS		*keyboard = NULL;								//����
MOUSECLASS			*mouse = NULL;									//���
bool				mousel_down = false;
//-------------̰����������-------------------//
SKYBOX_DERIVED		*skybox = NULL;
SELECT_SYSTEM		*select_system = NULL;
ATTACK				*attack = NULL;
SNAKE				*snake = NULL;
MESHXFILE *mesh_alter = NULL;										//��̳
MESHXFILE *mesh_conch = NULL;										//����
START	  *start = NULL;

//������������------------------------------------------------------------------------------------
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
	hWnd = Init_Window(hInstance,nShowCmd);						//��ʼ�����ڣ�����ʼ��Direct3D		
	Everything(hWnd,hInstance);									//������������ģ��

	//��Ϣѭ��
	MSG msg;	
	ZeroMemory(&msg,sizeof(msg));
	while (msg.message!=WM_QUIT)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			TranslateMessage(&msg);				//ת����Ϣ
			DispatchMessage(&msg);				//Ͷ����Ϣ������Ϣ���ݸ���Ӧ���ڣ�
		}
		else
		{
			Direct3DRender();					//��Ⱦ
		}
	}

	//ע��������
	UnregisterClass(g_pClassName,wndclass.hInstance);
return 0;
}

/**************************
)����Ϣ������
**************************/
LRESULT CALLBACK WndProc(HWND hWnd,UINT message,
						WPARAM wParam,LPARAM lParam)
{
	switch (message)
	{
	//�ͻ����ػ���Ϣ
	case WM_PAINT:
		ValidateRect(hWnd,NULL);								//���¿ͻ�������ʾ
		break;
	//����������Ϣ
	case WM_DESTROY:											//����������Ϣ
		PostQuitMessage(0);										//�˳�����
		Direct3DClearUp();
		break;
	//�������ж�
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
	//Ĭ�ϵ���Ϣ����
	return DefWindowProc(hWnd,message,wParam,lParam);
}

VOID Direct3DClearUp()
{
	if(g_pd3dDevice)
		g_pd3dDevice->Release();									//�ͷ�Direct3D9Device�豸�ӿ�
}
