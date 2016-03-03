# include "head.h"
# include "definition.h"
# include "global_value.h"

D3DXMATRIX world;

void one(bool &,bool &);
int select();
extern void light(int swi);													//����
void environment();															//�������л���
void element_person();
void test();

VOID Direct3DRender()
{
	g_pd3dDevice->Clear(0,NULL,D3DCLEAR_TARGET								//��̨
		| D3DCLEAR_ZBUFFER,													//���
		D3DCOLOR_XRGB(50,100,150),1.0f,
		0);																	//ģ�建������Ϊ0
	g_pd3dDevice->BeginScene();							
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);				//������Ⱦ״̬(��ʱ��������
	static bool stage_one = true;
	static bool stage_mid = true;											//���ڷ�ֹ�����ʱ������Ϣ����
	if(stage_one == true)													//��Ϸ��ʼ�ͻ���
	{
		environment();
		one(stage_one,stage_mid);					//��Ϸ��ʼ����		
		select_system->render();					//����ѡ��ϵͳ
	}
	else if(stage_one == false)						//������ͨ��Ⱦ
	{
		camera->View();
		select();
		D3DXMatrixIdentity(&world);
		test();
		environment();								//�������л���	
		element_person();							//����������صĻ���
	}

	g_pd3dDevice->EndScene();
	g_pd3dDevice->Present(NULL,NULL,NULL,NULL);
}
//������ѡ����صĺ���
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
	
	//���Ƽ�̳�ͺ���,Ϊ�˷�ֹ������������ͻ��ʹ��enΪ��׺
	D3DXMATRIX world_en,scale_en,tran_en,ro_en;
	D3DXMatrixScaling(&scale_en,30,30,30);
	D3DXMatrixIdentity(&world_en);
	D3DXMatrixTranslation(&tran_en,0,0,0);
	world_en = world_en * scale_en * tran_en;
	g_pd3dDevice->SetTransform(D3DTS_WORLD,&world_en);
	mesh_alter->DrawMesh();							//��̳

	float time;
	time = timeGetTime();
	D3DXMatrixIdentity(&world_en);
	D3DXMatrixScaling(&scale_en,5,5,5);
	D3DXMatrixTranslation(&tran_en,-500,200,-1600);
	D3DXMatrixRotationY(&ro_en,time * 0.001);
	world_en = world_en * scale_en * tran_en * ro_en;
	g_pd3dDevice->SetTransform(D3DTS_WORLD,&world_en);
	mesh_conch->DrawMesh();							//����

	//snow->update(1);
	//snow->render();
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING,FALSE);		//ѩ�������˵ƹ���Ⱦ
}
void element_person()
{
	attack->render(world);			//���ƹ�������
	snake->render();
	select_system->render();		//����ѡ��ϵͳ
}
//���е�ѡ�����
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

		if(keyboard->_key_state[DIK_LEFTARROW] & 0x80)		//��
			camera->RotationY(1);

		if(keyboard->_key_state[DIK_RIGHTARROW] & 0x80)		//��
			camera->RotationY(-1);

		if(keyboard->_key_state[DIK_UPARROW] & 0x80)		//��
			camera->RotationTargetRight(D3DX_PI / (500.0));

		if(keyboard->_key_state[DIK_DOWNARROW] & 0x80)		//��
			camera->RotationTargetRight(D3DX_PI / (-500.0));

		//����
		if(keyboard->_key_state[DIK_NUMPAD1] & 0x80)				//1
			light(1);
		if(keyboard->_key_state[DIK_NUMPAD2] & 0x80)				//2
			light(2);
		if(keyboard->_key_state[DIK_NUMPAD3] & 0x80)				//3
			light(3);
	}
		if(mousel_down == true && sign1 == false)				//������
		{
			if(select_system->test(mouse->_mouse_state.lX,mouse->_mouse_state.lY) == true)
			{	
				sign1 = true;
			}
		}
		else if(mousel_down == false && sign1 == true)			
			sign2 = true;
		else if(mousel_down == true && sign2 == true)				//������
		{	//�������ɹ��˲Ž���־�ָ�����Ϊ�п��ܽ�������ӵ���ͼ֮��
			if(select_system->add_person(mouse->_mouse_state.lX,mouse->_mouse_state.lY,world,attack) == true)
				sign1 = sign2 = false;
		}
			
return 0;
}

//��һ�׶εĲ�������
void one(bool &stage_one,bool &stage_mid)
{
	if(keyboard->ReadInput())
	{	
		if(keyboard->_key_state[DIK_F5] & 0x80)		//�������ÿ���
		{
			start->reset();
			stage_one = stage_mid = true;
		}
		if(keyboard->_key_state[DIK_F12] & 0x80)		//�������ÿ���
			start->begin_game();
	}
	start->render();
	if(start->stage() == 2)												//�����ͣ�£����ֿ�ʼ��ť
	{
		if(mousel_down == true)											//��갴��
			if(start->test(mouse->_mouse_state.lX,mouse->_mouse_state.lY) == true)		//����Ƿ����˿�ʼ��ť
				start->set_prompt();									//������Ϸ��ʾ��
	}
	else if(start->stage() == 3)
	{
		if(mousel_down == false && stage_mid == true)					//�ȴ�����ɿ�
			stage_mid = false;							
		else if(mousel_down == true && stage_mid == false)				//������°���
		{	stage_one = false;
			skybox->game_begin();										//��ʼ���ˮ��
		}
	}
}


