# include "head.h"
# include "definition.h"
# include "global_value.h"

D3DXMATRIX world;

void one(bool &,bool &);
int select();
extern void light(int swi);													//光照
void environment();															//绘制所有环境
void element_person();
void test();

VOID Direct3DRender()
{
	g_pd3dDevice->Clear(0,NULL,D3DCLEAR_TARGET								//后台
		| D3DCLEAR_ZBUFFER,													//深度
		D3DCOLOR_XRGB(50,100,150),1.0f,
		0);																	//模板缓存设置为0
	g_pd3dDevice->BeginScene();							
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);				//设置渲染状态(逆时针消隐）
	static bool stage_one = true;
	static bool stage_mid = true;											//用于防止鼠标点击时产生消息误判
	if(stage_one == true)													//游戏开始和画面
	{
		environment();
		one(stage_one,stage_mid);					//游戏开始场面		
		select_system->render();					//绘制选择系统
	}
	else if(stage_one == false)						//进入普通渲染
	{
		camera->View();
		select();
		D3DXMatrixIdentity(&world);
		test();
		environment();								//绘制所有环境	
		element_person();							//所有与人相关的绘制
	}

	g_pd3dDevice->EndScene();
	g_pd3dDevice->Present(NULL,NULL,NULL,NULL);
}
//所有与选择相关的函数
void test()
{
	snake->test(skybox->_fruit_head->_head,attack->_person->_head);
	if(skybox->game_end() == true)
		::MessageBox(0,L"Game Over",0,0);
}
void environment()
{
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING,FALSE);
	skybox->render();
	
	//绘制祭坛和海螺,为了防止与其它变量冲突，使用en为后缀
	D3DXMATRIX world_en,scale_en,tran_en,ro_en;
	D3DXMatrixScaling(&scale_en,30,30,30);
	D3DXMatrixIdentity(&world_en);
	D3DXMatrixTranslation(&tran_en,0,0,0);
	world_en = world_en * scale_en * tran_en;
	g_pd3dDevice->SetTransform(D3DTS_WORLD,&world_en);
	mesh_alter->DrawMesh();							//祭坛

	float time;
	time = timeGetTime();
	D3DXMatrixIdentity(&world_en);
	D3DXMatrixScaling(&scale_en,5,5,5);
	D3DXMatrixTranslation(&tran_en,-500,200,-1600);
	D3DXMatrixRotationY(&ro_en,time * 0.001);
	world_en = world_en * scale_en * tran_en * ro_en;
	g_pd3dDevice->SetTransform(D3DTS_WORLD,&world_en);
	mesh_conch->DrawMesh();							//海螺

	//snow->update(1);
	//snow->render();
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING,FALSE);		//雪花开启了灯光渲染
}
void element_person()
{
	attack->render(world);			//绘制攻击人物
	snake->render();
	select_system->render();		//绘制选择系统
}
//所有的选择操作
int select()
{
	static bool sign1 = false,sign2 = false;
	if(keyboard->ReadInput())
	{
		if(keyboard->_key_state[DIK_A] & 0x80)		//A
			camera->TranslationRight(-5);
		if(keyboard->_key_state[DIK_D] & 0x80)		//D
			camera->TranslationRight(5);
		if(keyboard->_key_state[DIK_W] & 0x80)		//W
			camera->TranslationLook(5);
		if(keyboard->_key_state[DIK_S] & 0x80)		//S
			camera->TranslationLook(-5);
		if(keyboard->_key_state[DIK_R] & 0x80)		//R
			camera->TranslationUp(5);
		if(keyboard->_key_state[DIK_F] & 0x80)		//F
			camera->TranslationUp(-5);

		if(keyboard->_key_state[DIK_LEFTARROW] & 0x80)		//左
			camera->RotationY(1);

		if(keyboard->_key_state[DIK_RIGHTARROW] & 0x80)		//右
			camera->RotationY(-1);

		if(keyboard->_key_state[DIK_UPARROW] & 0x80)		//上
			camera->RotationTargetRight(D3DX_PI / (500.0));

		if(keyboard->_key_state[DIK_DOWNARROW] & 0x80)		//下
			camera->RotationTargetRight(D3DX_PI / (-500.0));

		//光照
		if(keyboard->_key_state[DIK_NUMPAD1] & 0x80)				//1
			light(1);
		if(keyboard->_key_state[DIK_NUMPAD2] & 0x80)				//2
			light(2);
		if(keyboard->_key_state[DIK_NUMPAD3] & 0x80)				//3
			light(3);
	}
		if(mousel_down == true && sign1 == false)				//鼠标左键
		{
			if(select_system->test(mouse->_mouse_state.lX,mouse->_mouse_state.lY) == true)
			{	
				sign1 = true;
			}
		}
		else if(mousel_down == false && sign1 == true)			
			sign2 = true;
		else if(mousel_down == true && sign2 == true)				//鼠标左键
		{	//添加人物成功了才将标志恢复，因为有可能将人物添加到地图之外
			if(select_system->add_person(mouse->_mouse_state.lX,mouse->_mouse_state.lY,world,attack) == true)
				sign1 = sign2 = false;
		}
			
return 0;
}

//第一阶段的操作函数
void one(bool &stage_one,bool &stage_mid)
{
	if(keyboard->ReadInput())
	{	
		if(keyboard->_key_state[DIK_F5] & 0x80)		//重新设置开场
		{
			start->reset();
			stage_one = stage_mid = true;
		}
		if(keyboard->_key_state[DIK_F12] & 0x80)		//重新设置开场
			start->begin_game();
	}
	start->render();
	if(start->stage() == 2)												//摄像机停下，出现开始按钮
	{
		if(mousel_down == true)											//鼠标按下
			if(start->test(mouse->_mouse_state.lX,mouse->_mouse_state.lY) == true)		//鼠标是否点击了开始按钮
				start->set_prompt();									//设置游戏提示，
	}
	else if(start->stage() == 3)
	{
		if(mousel_down == false && stage_mid == true)					//等待鼠标松开
			stage_mid = false;							
		else if(mousel_down == true && stage_mid == false)				//鼠标重新按下
		{	stage_one = false;
			skybox->game_begin();										//开始添加水果
		}
	}
}


