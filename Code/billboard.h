# ifndef _BILLBOARD_H
# include "head.h"
# define _BILLBOARD_H

class BILLBOARD
{
protected:
	LPDIRECT3DDEVICE9				_device;
	LPDIRECT3DVERTEXBUFFER9			_vertex;
	LPDIRECT3DTEXTURE9				_texture;
public:
	BILLBOARD(LPDIRECT3DDEVICE9 device);
	//����ǰ����һ�ε��ô˺�����
	void Init(wchar_t *name);
	void draw(D3DXMATRIX &view,D3DXMATRIX &world);
};

# endif