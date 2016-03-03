# ifndef _GLOBAL_VALUE_H
# define _GLOBAL_VALUE_H

# include "head.h"
# include "skybox.h"
# include "camera.h"
# include "Meshxfile.h"
# include "billboard.h"
# include "snow.h"
# include "input.h"
# include "SkinMesh.h"
# include "attack.h"
# include "snake.h"
# include "start.h"

//ȫ�ֱ�����������չ
extern WNDCLASSEX wndclass;											//ȫ�ֱ������˱����ڳ�ʼ�������Լ��������ڵ�ʱ��Ҫ�õ���
extern wchar_t *g_pClassName;										//��ʼ���������Լ��������ڵ�ʱ���õ�������һ��
extern wchar_t *g_pWindowName;										//��������
extern LPDIRECT3DDEVICE9	g_pd3dDevice;							//Direct3DDevice9�豸�ӿ�
extern CAMERA *camera;												//�����		
extern SNOW *snow;													//ѩ���׷�

extern KEYBOARDCLASS *keyboard;										//����
extern MOUSECLASS	*mouse;											//���
extern bool				mousel_down;									//�������Ƿ���
//-----------------̰������Ӳ���-----------------------------//
extern SKYBOX_DERIVED		*skybox;
extern SELECT_SYSTEM		*select_system;
extern ATTACK				*attack;
extern SNAKE				*snake;
extern MESHXFILE *mesh_alter;										//��̳
extern MESHXFILE *mesh_conch;										//����
extern START	  *start;

# endif