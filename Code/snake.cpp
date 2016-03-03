# include "snake.h"

# define XMIN_SNAKE _box._min.x
# define XMAX_SNAKE _box._max.x
# define Y_SNAKE 200
# define ZMIN_SNAKE _box._min.z
# define ZMAX_SNAKE _box._max.z
# define SCALE_SNAKE 5	
# define RO_SNAKE	0

SNAKE::SNAKE(const BOUNDINGBOX &box):
	_box(box),				//�߻�ķ�Χ
	_product_time(2000),	//�����µ�������Ҫ��ʱ��
	_distance(300),		//��ײ������
	_distance_snake(200),	//ͬһ���߲�ͬ�ڵ�ľ���
	_go_time(500),			//ǰ��ʱ��
	_go_time_cut(1),
	_velocity(200),
	_update_time(50),
	_update_cut(1)
{
	_snake_big = new SNAKE_BIG();		//
	_snake_big->_head = _snake_big->_end = NULL;
	_snake_big->_next = NULL;
	_snake_mid = NULL;
}
SNAKE::~SNAKE()
{

}
//--------------------------------------
//��ʼ�������x�ļ�
//--------------------------------------
void SNAKE::init(LPDIRECT3DDEVICE9 device,wchar_t *name_snake)
{
	_product = _product_time;
	D3DXMatrixScaling(&_scale,SCALE_SNAKE,SCALE_SNAKE,SCALE_SNAKE);
	MESHXFILE::init(name_snake,device);
	for(int i = 0;i < 2;i++)			//����������
		add();
}
//------------------------------------------
//�����µ���ͷ
//------------------------------------------
void SNAKE::add()						
{
	SNAKE_STRUCT *head;
	D3DXVECTOR3 Z(0,0,1),X(1,0,0);
	head = new SNAKE_STRUCT();
	head->_ishead = true;				//ͷ
	head->_alive = true;				//���
	head->_child = false;				//����
	head->_go_time = _go_time;			//����ǰ��ʱ��
	head->_before = head->_next = NULL;					
	head->_update = _update_time;

	if(GetRandomFloat(0,2) < 1)			//�Ϸ������µ���
	{	head->_position.z = ZMAX_SNAKE - 2 * _distance;
		head->_position.x = GetRandomFloat(XMIN_SNAKE + 2 * _distance,XMAX_SNAKE - 2 * _distance);
		head->_position.y = Y_SNAKE;
		head->_look = -Z;
		head->_right = -X;
		/*********�޸���ת����*********/
		D3DXMatrixRotationAxis(&(head->_rotation),&D3DXVECTOR3(0,1,0),0);
	}
	else								//�Ҳ�����µ���
	{	head->_position.x = XMAX_SNAKE - 2 * _distance;
		head->_position.z = GetRandomFloat(ZMIN_SNAKE + 2 * _distance,ZMAX_SNAKE - 2 * _distance);
		head->_position.y = Y_SNAKE;
		head->_look = -X;
		head->_right = Z;
		/*********�޸���ת����*********/
		D3DXMatrixRotationAxis(&(head->_rotation),&D3DXVECTOR3(0,1,0),D3DX_PI / 2);
	}
	if(_snake_mid == NULL)		//����ոտ�ʼ����û����
	{
		_snake_big->_head = _snake_big->_end = _snake_mid = new SNAKE_MIDDLE();
		_snake_big->_next = NULL;
		_snake_mid->_before = NULL;						//��һ���ڵ�֮ǰû�нڵ�
		_snake_mid->_next = NULL;
		_snake_mid->_head = _snake_mid->_end = head;
	}
	else						//�������ߵĻ����ϲ����µ���
	{	
		_snake_big->_end->_next = new SNAKE_MIDDLE();
		_snake_big->_end->_next->_before = _snake_big->_end;
		_snake_big->_end = _snake_big->_end->_next;
		_snake_big->_end->_head = _snake_big->_end->_end = head;
		_snake_big->_end->_next = NULL;
	}
}
//-----------------------------------------
//�����ߵĳ���
//�ߵ�ͷ��־�Ƿ����ӳ��ȣ�����β������߽ڵ�
//-----------------------------------------
void SNAKE::add(SNAKE_STRUCT *p,SNAKE_MIDDLE *mid)		//�β�Ϊ�ߵ����һ���ڵ�
{
	mid->_end = p->_next = new SNAKE_STRUCT();
	p->_next->_before = p;						//ǰ��ڵ�����
	p->_next->_next = NULL;						//��β
	p->_next->_ishead = false;					//����ͷ��
	p->_next->_alive = true;					//���
	p->_next->_look = p->_look;					//���߷���
	p->_next->_right = p->_right;				//�Ҳ�
	p->_next->_position = p->_position - p->_look * _distance_snake;	//λ��
	p->_next->_rotation = p->_rotation;			//�Ӿ�Ч��
	p->_next->_child = mid->_head->_child = false;		//�������µĽڵ��Ϊfalse
}
void SNAKE::update()
{
	_product -= _go_time_cut;
	if(_product <= 0)
	{	add();
		_product = _product_time;
	}
	if(_snake_mid != NULL)								//���ߴ���
	{
		SNAKE_MIDDLE *p_mid = _snake_mid,*p_next;
		for( ; p_mid != NULL ;)							//�����ߴ����ǽ���ѭ��
		{
			p_next = p_mid->_next;						//��¼��ǰ�ڵ����һ���ڵ㣬ɾ���ߵ�ʱ����Ҫ
			SNAKE_STRUCT *p = p_mid->_head;

			p->_go_time -= _go_time_cut;

			if(p->_alive == false)						//���Ѿ�����
			{	delete_snake(&p_mid);					//ɾ��������
				p_mid = p_next;
				continue;
			}

			if(p->_child == true)						//�ж���ͷ�Ƿ����ӳ��ȵ�
				add(p_mid->_end,p_mid);					//�ǣ�����β���ӳ���

			if(isneed_turn(p) == true)					//�ж����Ƿ���Ҫ����
				if(turn(p) == false)
					continue;							//����Ҫת�䣬����û�ܳɹ�ת�䣬�ȴ�ת��Ļ���
			
			p->_update -= _update_cut;					//Ϊ�˷�ֹ�����߹��죬���ô˱���
			if(p->_update <= 0)
			{
				if(p->_go_time <= 0)						//��������ʱ��
					p->_need_turn = true;
				go_ahead(p_mid->_end);						//����������
				p->_update = _update_time;
			}
			p_mid = p_mid->_next;						//�鿴��һ����
		}
	}
}

//---------------------------------------
//������ǰ����
//---------------------------------------
void SNAKE::go_ahead(SNAKE_STRUCT *end)
{
	if(end->_before == NULL)			//������ͷ
	{
		end->_position += end->_look * _velocity;
		return ;
	}
	else
	{
		end->_position += end->_look * _velocity;
		//�ж��Ƿ���Ҫ�����ڵ��������Ϣ,�����Ƿ񵽴�����
		if(D3DXVec3Length(&(end->_look - end->_before->_look)) > 
			D3DXVec3Length(&(end->_look)))
		{
			end->_look = end->_before->_look;
			end->_rotation = end->_before->_rotation;
		}
		go_ahead(end->_before);		//�ݹ�һ��
	}
}
//---------------------------------------
//ɾ���Ѿ���������
//---------------------------------------
void SNAKE::delete_snake(SNAKE_MIDDLE **mid)	//ע��ı������ϵ����������
{
	SNAKE_STRUCT *p = (*mid)->_head,*p1;
	for(;p != NULL;)							//ɾ�������ߵĽڵ�
	{
		p1 = p->_next;
		delete p;
		p = p1;
	}
	(*mid)->_head = NULL;

	if((*mid)->_next == NULL && (*mid)->_before == NULL)		//ֻ��һ����
	{
		_snake_mid = _snake_big->_head = _snake_big->_end = NULL;
		delete *mid;
		*mid = NULL;
		return ;
	}
	else if((*mid)->_before == NULL)							//��һ����
	{
		_snake_big->_head = _snake_mid = (*mid)->_next;
		_snake_big->_head->_before = NULL;						//��ͷ
		delete *mid;
		*mid = NULL;
		return ;
	}
	else if((*mid)->_next == NULL)								//���һ����
	{
		_snake_big->_end = (*mid)->_before;
		_snake_big->_end->_next = NULL;							//��β
		delete *mid;
		*mid = NULL;
		return ;
	}
	else														//��ͨ����
	{
		(*mid)->_before->_next = (*mid)->_next;
		(*mid)->_next->_before = (*mid)->_before;
		delete *mid;
		*mid = NULL;	
		return ;
	}
}
//----------------------------------------
//�ж����Ƿ���������ײ
bool SNAKE::isneed_turn(SNAKE_STRUCT *head)			//����ͷ���
{
	if(head->_need_turn == true)					//����һ����Ҫ����ʱ��û�й���ɹ�
		return true;
	//���������ܵ�ǽ�ڼ�������
	if(((head->_position + head->_look * _distance).x > XMAX_SNAKE) || 
		((head->_position +  head->_look * _distance).x < XMIN_SNAKE) ||
		((head->_position +  head->_look * _distance).z > ZMAX_SNAKE) ||
		((head->_position +  head->_look * _distance).z < ZMIN_SNAKE) )
		return true;
	if(ishit_snake(head) == true)					//������������ײ��ֱ�ӷ���true
		return true;
return false;
}
//-----------------------------------------
//�ж���֮���Ƿ���ײ
//-----------------------------------------
bool SNAKE::ishit_snake(SNAKE_STRUCT *head)			//����ͷ���
{
	//������
return false;
}
bool SNAKE::turn(SNAKE_STRUCT *head)					//����ͷ���
{
	float a;
	int b;
	while((a = GetRandomFloat(-1,1)) == 0);
	if(a < 0) b = -2;					//�ж���ת90����-90��
	else if(a > 0) b = 2;
	D3DXMATRIX ro;
	D3DXMatrixRotationAxis(&ro,&(D3DXVECTOR3(0,1,0)),D3DX_PI / b);
	D3DXVec3TransformNormal(&(head->_look),&(head->_look),&ro);
	D3DXVec3Normalize(&(head->_look),&(head->_look));
	head->_rotation *= ro;
	head->_need_turn = false;
	if(isneed_turn(head) == true)				//��תʧ�ܣ��߻�����Ҫ��ת
	{
		return false;
		head->_need_turn = true;
	}
	head->_go_time = _go_time;
return true;
}
//-----------------------------
//�������������ˮ���Ĺ�ϵ
bool SNAKE::test(FRUIT_STRUCT *head_fruit,PERSON_STRUCT *head_people)
{
	test(head_people);
	test(head_fruit);
return true;					//---------------�ݶ�����ֵ
}
# ifndef JUGG 1
# define JUGG 1
# endif
# ifndef SUQI 2
# define SUQI 2
# endif
//���������Ƿ��ɱ����̰����
bool SNAKE::test(PERSON_STRUCT *head_people)
{
	SNAKE_STRUCT *snake;
	SNAKE_MIDDLE *snake_mid;
	
	while(head_people != NULL)
	{
		snake_mid = _snake_mid;
		while(snake_mid != NULL)
		{
			snake = snake_mid->_head;
			while(snake != NULL)
			{
				//��������˵ľ���		
				if(head_people->_style == JUGG)
				{	
					if(D3DXVec3Length(&(snake->_position - head_people->_position))
						< 3 * _distance)		//�����˵ľ��뿿��
					//���ڽ�ʥ֮ǰ����������󲻻ᣬ��ⲿλ��ȫ����������ͷ��־
					if(D3DXVec3Dot(&(snake->_position - head_people->_position),
						&(head_people->_look)) > 0)
					{	
						snake_mid->_head->_alive = false;
						break;
					}
				}
				else if(head_people->_style == SUQI)
				{	
					if(D3DXVec3Length(&(snake->_position - head_people->_position))
						<  _distance)
					//������������������,���ڼ��Ĳ�λ����ȫ��������������ͷ��־
					snake_mid->_head->_alive = false;
					break;
				}
				
				snake = snake->_next;					//�鿴�ߵ���һ���ڵ�
			}
			snake_mid = snake_mid->_next;				//�鿴��һ����
		}
		head_people = head_people->_next;				//�鿴��һ����
	}
return true;				//----------------�ݶ�����ֵ
}
//����̰�����Ƿ�Ե�ˮ��
bool SNAKE::test(FRUIT_STRUCT *head_fruit)
{
	SNAKE_MIDDLE *snake_mid;
	snake_mid = _snake_mid;
	while(head_fruit != NULL)				//����ˮ������
	{
		snake_mid = _snake_mid;
		while(snake_mid != NULL)			//����̰����ͷ������Ϊֻ��ͷ�����Գ�ˮ��
		{
			if(D3DXVec3Length(&(snake_mid->_head->_position - head_fruit->_position))
				< _distance)				//��ͷ��ˮ�������㹻
				if(D3DXVec3Dot(&(snake_mid->_head->_position - head_fruit->_position),
						&(snake_mid->_head->_look)) > 0)		//ͨ�������жϷ���
			{
				snake_mid->_head->_child = true;
				head_fruit->_alive = false;
			}
			snake_mid = snake_mid->_next;						//�鿴��һ����ͷ
		}
		head_fruit = head_fruit->_next;							//�鿴��һ��ˮ��
	}
return true;						//----------------�ݶ�����ֵ
}
void SNAKE::render()
{
	update();
	SNAKE_MIDDLE *p_mid;
	SNAKE_STRUCT *p_str;
	D3DXMATRIX	world,tran;
	for(p_mid = _snake_mid;p_mid != NULL;p_mid = p_mid->_next)
	{
		for(p_str = p_mid->_head;p_str != NULL;p_str = p_str->_next)
		{
			D3DXMatrixIdentity(&world);
			D3DXMatrixTranslation(&tran,p_str->_position.x,p_str->_position.y,p_str->_position.z);
			world = world * _scale * p_str->_rotation * tran;
			_device->SetTransform(D3DTS_WORLD,&world);
			MESHXFILE::DrawMesh();						//�ڲ�ͬ�ĵص���Ƴ���ͬ����
		}
	}
}
