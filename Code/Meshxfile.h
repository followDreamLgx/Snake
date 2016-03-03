# ifndef _MESHXFILE_H
# define _MESHXFILE_H


# include "head.h"


class MESHXFILE
{
protected:
	LPDIRECT3DDEVICE9	_device;			//direct3D�豸�ӿ�
	LPD3DXMESH			_mesh;				//�������
	D3DMATERIAL9		*_materials;		//����Ĳ�����Ϣ
	LPDIRECT3DTEXTURE9  *_textures;			//�����������Ϣ
	DWORD				_NumMaterials;		//���ʵ���Ŀ

public:
	MESHXFILE();
	MESHXFILE(LPDIRECT3DDEVICE9);
	void init(wchar_t*,LPDIRECT3DDEVICE9);
	void InitXMesh(wchar_t *);			//����x�ļ����������������������
	void DrawMesh();					//�����������
};
# endif