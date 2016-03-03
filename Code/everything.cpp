# include "head.h"
# include "definition.h"
# include "global_value.h"




INT Everything(HWND hWnd,HINSTANCE hInstance)
{
	srand(0);
	keyboard = new KEYBOARDCLASS(hWnd,hInstance);		//�������
	mouse = new MOUSECLASS(hWnd,hInstance);

	camera = new CAMERA(g_pd3dDevice);					//�����
	wchar_t *name_sky[6] = {{L"backsnow1.jpg"},			//��պ�
						{L"leftsnow1.jpg"},
						{L"frontsnow1.jpg"},
						{L"rightsnow1.jpg"},
						{L"topsnow1.jpg"},
						{L"groundsnow1.jpg"}};

	BOUNDINGBOX box_plane(D3DXVECTOR3(25000,10,25000)),
		box_big(D3DXVECTOR3(100000,100000,100000)),
		box_small(D3DXVECTOR3(1000,1000,1000)),
		box_snow(D3DXVECTOR3(10000,15000,10000)),
		box_activity(D3DXVECTOR3(2500,10,2500));

	snow = new SNOW(&box_snow,500);
	snow->init(g_pd3dDevice,L"snowflake.dds");

	PlaySound(L"�ɽ���.wav", NULL, SND_FILENAME | SND_ASYNC|SND_LOOP);

	//****************************̰������Ӳ���**********************************************//
	wchar_t *name_fruit[10] = {		//ˮ��x�ļ�					
			{L"ˮ����.X"},{L"���ʶ�.X"},{L"һ��ƻ��_С.X"}};
	//��������
	skybox = new SKYBOX_DERIVED(g_pd3dDevice,box_plane,3);		//ע�⣬���������һ��
																//�����ˮ�����鳤��һ��
	skybox->init(name_sky,L"�ݵ�1.jpg",name_fruit,box_big);

	mesh_conch = new MESHXFILE(g_pd3dDevice);
	mesh_conch->InitXMesh(L"����2.X");

	mesh_alter = new MESHXFILE(g_pd3dDevice);
	mesh_alter->InitXMesh(L"��̳.X");
	//ѡ��ϵͳ
	select_system = new SELECT_SYSTEM(g_pd3dDevice,box_activity);
	select_system->init(L"snake_jugg.x",L"snake_suqi.X");

	wchar_t *name_attack[2] = {{L"snake_jugg.x"},{L"snake_suqi.X"}};
	//����ϵͳ
	attack = new ATTACK(g_pd3dDevice);
	attack->init(name_attack);

	//̰����
	wchar_t *name_snake = {L"caichong.X"};
	snake = new SNAKE(box_activity);
	snake->init(g_pd3dDevice,name_snake);

	//��Ϸǰ�ڶ���
	D3DXVECTOR3 ca_position(11700,9500,0),ca_target(0,0,0);
	start = new START(g_pd3dDevice);
	wchar_t *a[10] = {{L"������ս�Ͻ�֮��"},{L"   ̰����2014"},{L"�������"},{L"�������"},{L"��ʾ"}};
	start->init(name_snake,a,2,3,ca_position,ca_target);
	return 0;
}