# ifndef _SNAKE_H
# define _SNAKE_H

# include "Meshxfile.h"

struct SNAKE_STRUCT
{
	D3DXVECTOR3 _position;
	D3DXVECTOR3 _look;
	D3DXVECTOR3 _right;
	D3DXMATRIX	_rotation;
	//�������뷱�ܵı���
	bool		_ishead;
	bool		_alive;
	bool		_child;
	//��������ת��ı���
	bool		_need_turn;
	int			_go_time;
	int			_update;
	struct SNAKE_STRUCT	*_next;
	struct SNAKE_STRUCT	*_before;
};
struct SNAKE_MIDDLE
{
	struct SNAKE_STRUCT *_head;
	struct SNAKE_STRUCT *_end;
	struct SNAKE_MIDDLE *_next;
	struct SNAKE_MIDDLE *_before;
};
struct SNAKE_BIG
{
	struct SNAKE_MIDDLE *_head;
	struct SNAKE_MIDDLE *_end;
	struct SNAKE_BIG	*_next;
};
class SNAKE:public MESHXFILE
{
	SNAKE_BIG		*_snake_big;
	SNAKE_MIDDLE	*_snake_mid;

	const BOUNDINGBOX	_box;
	const int		_product_time;
	int				_product;
	const float		_distance;
	const float		_distance_snake;
	const int		_go_time;
	const int		_go_time_cut;
	const int		_velocity;
	const int		_update_time;
	const int		_update_cut;
	D3DXMATRIX		_scale;
public:
	SNAKE(const BOUNDINGBOX&);
	~SNAKE();
	void init(LPDIRECT3DDEVICE9 device,wchar_t *name_snake);
	void add();									//�����µ���ͷ
	void add(SNAKE_STRUCT *,SNAKE_MIDDLE *mid);	//�β�Ϊ�ߵ����һ���ڵ�
	void update();
	void delete_snake(SNAKE_MIDDLE **);
	void go_ahead(SNAKE_STRUCT *);
	bool isneed_turn(SNAKE_STRUCT *);			//����ͷ���
	bool ishit_snake(SNAKE_STRUCT*);			//����ͷ���
	bool turn(SNAKE_STRUCT*);					//����ͷ���,���ɹ�ת�䣬����true
	void render();
	bool test(FRUIT_STRUCT *head_fruit,PERSON_STRUCT *head_people);
	bool test(PERSON_STRUCT *head_people);
	bool test(FRUIT_STRUCT *head_fruit);
};

# endif