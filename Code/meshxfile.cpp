
# include "Meshxfile.h"

extern char* find_texture(wchar_t*,char**,char*);

MESHXFILE::MESHXFILE(LPDIRECT3DDEVICE9 device)
{
	_device = device;			//direct3D�豸�ӿ�
	_mesh = NULL;				//�������
	_textures = NULL;			//�����������Ϣ
	_NumMaterials = 0;			//���ʵ���Ŀ
}
MESHXFILE::MESHXFILE()
{
	_device = NULL;			//direct3D�豸�ӿ�
	_mesh = NULL;				//�������
	_textures = NULL;			//�����������Ϣ
	_NumMaterials = 0;			//���ʵ���Ŀ
}
void MESHXFILE::init(wchar_t *xfile,LPDIRECT3DDEVICE9 device)
{
	_device = device;			//direct3D�豸�ӿ�
	_mesh = NULL;				//�������
	_textures = NULL;			//�����������Ϣ
	_NumMaterials = 0;			//���ʵ���Ŀ	
	InitXMesh(xfile);
}
void MESHXFILE::InitXMesh(wchar_t *xfile)	//����x�ļ����������������������
{
	LPD3DXBUFFER pAdjBuffer = NULL;
	LPD3DXBUFFER pMtrlBuffer = NULL;
	//��ȡx�ļ��е���������
	D3DXLoadMeshFromX(xfile,
		D3DXMESH_MANAGED,
		_device,
		&pAdjBuffer,
		&pMtrlBuffer,
		NULL,
		&_NumMaterials,
		&_mesh);
	
	//��ȡ���ʺ���������
	D3DXMATERIAL *pMaterial = (D3DXMATERIAL*)pMtrlBuffer->GetBufferPointer();
	_materials = new D3DMATERIAL9[_NumMaterials];			//������¼������Ϣ�Ķ���
	_textures = new LPDIRECT3DTEXTURE9[_NumMaterials];		//������¼�����ָ��

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
				name_xfile,									//��pMtrBuffer�õ���ʼ��ַ
				&_textures[i]);								//�洢����
		}
		if(name_xfile != NULL)
					delete []name_xfile;
	}
	pAdjBuffer->Release();
	pMtrlBuffer->Release();
	//�����������ģʽ
}
void MESHXFILE::DrawMesh()				//�����������
{
	for(DWORD i = 0;i < _NumMaterials;i++)
	{
		_device->SetMaterial(&_materials[i]);
		_device->SetTexture(0,_textures[i]);
		_mesh->DrawSubset(i);
	}
}
