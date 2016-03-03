
# include "Meshxfile.h"

extern char* find_texture(wchar_t*,char**,char*);

MESHXFILE::MESHXFILE(LPDIRECT3DDEVICE9 device)
{
	_device = device;			//direct3D设备接口
	_mesh = NULL;				//网格对象
	_textures = NULL;			//网格的纹理信息
	_NumMaterials = 0;			//材质的数目
}
MESHXFILE::MESHXFILE()
{
	_device = NULL;			//direct3D设备接口
	_mesh = NULL;				//网格对象
	_textures = NULL;			//网格的纹理信息
	_NumMaterials = 0;			//材质的数目
}
void MESHXFILE::init(wchar_t *xfile,LPDIRECT3DDEVICE9 device)
{
	_device = device;			//direct3D设备接口
	_mesh = NULL;				//网格对象
	_textures = NULL;			//网格的纹理信息
	_NumMaterials = 0;			//材质的数目	
	InitXMesh(xfile);
}
void MESHXFILE::InitXMesh(wchar_t *xfile)	//传入x文件名，创建网格对象与纹理
{
	LPD3DXBUFFER pAdjBuffer = NULL;
	LPD3DXBUFFER pMtrlBuffer = NULL;
	//读取x文件中的网格数据
	D3DXLoadMeshFromX(xfile,
		D3DXMESH_MANAGED,
		_device,
		&pAdjBuffer,
		&pMtrlBuffer,
		NULL,
		&_NumMaterials,
		&_mesh);
	
	//读取材质和纹理数据
	D3DXMATERIAL *pMaterial = (D3DXMATERIAL*)pMtrlBuffer->GetBufferPointer();
	_materials = new D3DMATERIAL9[_NumMaterials];			//创建记录材质信息的对象
	_textures = new LPDIRECT3DTEXTURE9[_NumMaterials];		//创建记录纹理的指针

	for(DWORD i = 0;i < _NumMaterials;i++)
	{
		_materials[i] = pMaterial[i].MatD3D;
		_materials[i].Ambient = _materials[i].Diffuse;
		_textures[i] = NULL;
		char *name_xfile;
		name_xfile = new char;
		if(pMaterial[i].pTextureFilename != NULL)
		{		
			find_texture(xfile,&name_xfile,pMaterial[i].pTextureFilename);

			D3DXCreateTextureFromFileA(_device,
				name_xfile,									//从pMtrBuffer得到起始地址
				&_textures[i]);								//存储纹理
		}
		if(name_xfile != NULL)
					delete []name_xfile;
	}
	pAdjBuffer->Release();
	pMtrlBuffer->Release();
	//设置纹理过滤模式
}
void MESHXFILE::DrawMesh()				//绘制网格对象
{
	for(DWORD i = 0;i < _NumMaterials;i++)
	{
		_device->SetMaterial(&_materials[i]);
		_device->SetTexture(0,_textures[i]);
		_mesh->DrawSubset(i);
	}
}
