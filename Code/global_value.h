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

//全局变量作用域拓展
extern WNDCLASSEX wndclass;											//全局变量，此变量在初始化窗口以及撤销窗口的时候都要用到，
extern wchar_t *g_pClassName;										//初始化窗口类以及创建窗口的时候用到，必须一致
extern wchar_t *g_pWindowName;										//窗口名称
extern LPDIRECT3DDEVICE9	g_pd3dDevice;							//Direct3DDevice9设备接口
extern CAMERA *camera;												//摄像机		
extern SNOW *snow;													//雪花纷飞

extern KEYBOARDCLASS *keyboard;										//键盘
extern MOUSECLASS	*mouse;											//鼠标
extern bool				mousel_down;									//鼠标左键是否按下
//-----------------贪吃蛇添加部分-----------------------------//
extern SKYBOX_DERIVED		*skybox;
extern SELECT_SYSTEM		*select_system;
extern ATTACK				*attack;
extern SNAKE				*snake;
extern MESHXFILE *mesh_alter;										//祭坛
extern MESHXFILE *mesh_conch;										//海螺
extern START	  *start;

# endif