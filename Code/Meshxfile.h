# ifndef _MESHXFILE_H
# define _MESHXFILE_H


# include "head.h"


class MESHXFILE
{
protected:
	LPDIRECT3DDEVICE9	_device;			//direct3D设备接口
	LPD3DXMESH			_mesh;				//网格对象
	D3DMATERIAL9		*_materials;		//网格的材质信息
	LPDIRECT3DTEXTURE9  *_textures;			//网格的纹理信息
	DWORD				_NumMaterials;		//材质的数目

public:
	MESHXFILE();
	MESHXFILE(LPDIRECT3DDEVICE9);
	void init(wchar_t*,LPDIRECT3DDEVICE9);
	void InitXMesh(wchar_t *);			//传入x文件名，创建网格对象与纹理
	void DrawMesh();					//绘制网格对象
};
# endif