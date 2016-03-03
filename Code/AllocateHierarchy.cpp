
#include "AllocateHierarchy.h"

extern LPDIRECT3DDEVICE9	g_pd3dDevice;
char* find_texture(wchar_t*,char**,char*);
/*-----------------------------------------------------------------------------
// Desc: 
1. ����һ���������ļ������Ƶ��ڶ���������
//---------------------------------------------------------------------------*/
HRESULT CAllocateHierarchy::AllocateName( LPCSTR Name, LPSTR *pNewName )
{
    UINT cbLength;

    if( Name != NULL )
    {
        cbLength = (UINT)strlen(Name) + 1;
        *pNewName = new CHAR[cbLength];
        if (*pNewName == NULL)
            return E_OUTOFMEMORY;
        memcpy( *pNewName, Name, cbLength*sizeof(CHAR) );
    }
    else
    {
        *pNewName = NULL;
    }

    return S_OK;
}

//-----------------------------------------------------------------------------
// Desc: �Ӿ���·������ȡ�ļ���
//-----------------------------------------------------------------------------
void CAllocateHierarchy::RemovePathFromFileName(LPSTR fullPath, LPWSTR fileName)
{
	//�Ƚ�fullPath�����ͱ任ΪLPWSTR
	WCHAR wszBuf[MAX_PATH];
	MultiByteToWideChar( CP_ACP, 0, fullPath, -1, wszBuf, MAX_PATH );
	wszBuf[MAX_PATH-1] = L'\0';

	WCHAR* wszFullPath = wszBuf;

	//�Ӿ���·������ȡ�ļ���
	LPWSTR pch=wcsrchr(wszFullPath,'\\');
	if (pch)
		lstrcpy(fileName, ++pch);
	else
		lstrcpy(fileName, wszFullPath);
}


//-----------------------------------------------------------------------------
//Desc: ������Ƥ����ģ��(����ÿ������Ļ��Ȩ�ء�������һ��������ϱ�)
//-----------------------------------------------------------------------------
HRESULT CAllocateHierarchy::GenerateSkinnedMesh(D3DXMESHCONTAINER_DERIVED *pMeshContainer)
{
	HRESULT hr = S_OK;

	if (pMeshContainer->pSkinInfo == NULL)
		return hr;

	if(pMeshContainer->MeshData.pMesh)
		pMeshContainer->MeshData.pMesh->Release();
	if(pMeshContainer->pBoneCombinationBuf)
		pMeshContainer->pBoneCombinationBuf->Release();

	//��ȡ����ģ������������
	LPDIRECT3DINDEXBUFFER9 pIB;
	if (FAILED(hr = pMeshContainer->pOrigMesh->GetIndexBuffer(&pIB)))
		return E_FAIL;

	//��ȡӰ��һ����(������)�ľ�������
	DWORD NumMaxFaceInfl;
	hr = pMeshContainer->pSkinInfo->GetMaxFaceInfluences(pIB, 
		pMeshContainer->pOrigMesh->GetNumFaces(), &NumMaxFaceInfl);
	pIB->Release();
	if (FAILED(hr))
		return E_FAIL;

	//Ӱ��һ����ľ����������ᳬ��12
	NumMaxFaceInfl = min(NumMaxFaceInfl, 12);

	//��ȡ��ǰ�豸������
	IDirect3DDevice9* pd3dDevice = g_pd3dDevice;		//ע�⣬����ط���ȫ�ֱ���**************
	D3DCAPS9 d3dCaps;
    pd3dDevice->GetDeviceCaps( &d3dCaps );

	//���ݵ�ǰ�豸������NumMaxFaceInfl, �����ڽ��й�����Ƥ��ɫʱʹ�ù�������������
	//������Ҫ����һ������������ռ�����ߣ����Ե�ʹ��Ӳ�������ʱ��
	//NumMaxFaceInfl(Ӱ��һ������Ĺ��������п��ܴӸ�����Ӱ�쵽Ŀǰ�������ֵ
	//ֻ����MaxVertexBlendMatrixIndex + 1��һ�롣��ֵ��ʾ��Ͼ�����������
	if( (d3dCaps.MaxVertexBlendMatrixIndex+1)/2 < NumMaxFaceInfl )
	{
		pMeshContainer->NumPaletteEntries = min(256, pMeshContainer->pSkinInfo->GetNumBones());
		pMeshContainer->UseSoftwareVP = true;
	}
	else
	{
		pMeshContainer->NumPaletteEntries = min( (d3dCaps.MaxVertexBlendMatrixIndex+1) / 2, 
			                                     pMeshContainer->pSkinInfo->GetNumBones() );
		pMeshContainer->UseSoftwareVP = false;
	}

	//������Ƥ����ģ��
	hr = pMeshContainer->pSkinInfo->ConvertToIndexedBlendedMesh
		                              ( pMeshContainer->pOrigMesh,
		                                0,
		                                pMeshContainer->NumPaletteEntries,
		                                pMeshContainer->pAdjacency,
		                                NULL, NULL, NULL,
		                                &pMeshContainer->NumInfl,
		                                &pMeshContainer->NumAttributeGroups,
		                                &pMeshContainer->pBoneCombinationBuf,
		                                &pMeshContainer->MeshData.pMesh);
	
	return hr;

}


//-----------------------------------------------------------------------------
// Desc: �������, �����Ƿ����ڴ�ͳ�ʼ��,��û�ж����Ա������ʵ�ֵ
//-----------------------------------------------------------------------------
HRESULT CAllocateHierarchy::CreateFrame(LPCSTR Name, LPD3DXFRAME *ppNewFrame)
{
    HRESULT hr = S_OK;
    D3DXFRAME_DERIVED *pFrame;
	
    *ppNewFrame = NULL;
	
    pFrame = new D3DXFRAME_DERIVED;  //������ܽṹ����
    if (pFrame == NULL) 
    {
        return E_OUTOFMEMORY;
    }
	
	//Ϊ���ָ������
    hr = AllocateName(Name, (LPSTR*)&pFrame->Name);
	if (FAILED(hr))
	{
		delete pFrame;
		return hr;
	}
	
	//��ʼ��D3DXFRAME_DERIVED�ṹ������Ա����
    D3DXMatrixIdentity(&pFrame->TransformationMatrix);
    D3DXMatrixIdentity(&pFrame->CombinedTransformationMatrix);
	
    pFrame->pMeshContainer = NULL;
    pFrame->pFrameSibling = NULL;
    pFrame->pFrameFirstChild = NULL;
	
    *ppNewFrame = pFrame;
    pFrame = NULL;

	return hr;
}


//-----------------------------------------------------------------------------
// Desc: �������ǵ����˳�Ա���� GenerateSkinnedMesh(pMeshContainer);
//       ���������������Ƥ��Ϣ
//-----------------------------------------------------------------------------
HRESULT CAllocateHierarchy::CreateMeshContainer(LPCSTR Name, 
                                                CONST D3DXMESHDATA *pMeshData,
                                                CONST D3DXMATERIAL *pMaterials, 
                                                CONST D3DXEFFECTINSTANCE *pEffectInstances, 
                                                DWORD NumMaterials, 
                                                CONST DWORD *pAdjacency, 
                                                LPD3DXSKININFO pSkinInfo, 
                                                LPD3DXMESHCONTAINER *ppNewMeshContainer) 
{
    HRESULT hr;
    D3DXMESHCONTAINER_DERIVED *pMeshContainer = NULL;
    UINT NumFaces;       //�����е�����,��������������ṹ���ڽ���Ϣ��Աʱʹ��
    UINT iMaterial;      //�������ʱ��ѭ������
    UINT cBones;         //��ǰ����ģ�͹�������
    LPDIRECT3DDEVICE9 pd3dDevice = NULL;
    LPD3DXMESH pMesh    = NULL;				//pMesh�Ĵ�����Ϊ�˷���pMeshContainer
    *ppNewMeshContainer = NULL;
	//�ж��������Ƿ�����������
    if (pMeshData->Type != D3DXMESHTYPE_MESH)
    {
        return E_FAIL;
    }
	
    pMesh = pMeshData->pMesh;
	//�ж��Ƿ����������Ϣ
    if (pMesh->GetFVF() == 0)
    {
        return E_FAIL;
    }

	//Ϊ�������������ڴ�
    pMeshContainer = new D3DXMESHCONTAINER_DERIVED;
    if (pMeshContainer == NULL)
    {
        return E_OUTOFMEMORY;
    }
    memset(pMeshContainer, 0, sizeof(D3DXMESHCONTAINER_DERIVED));
	
	//������������ṹD3DXMESHCONTAINER_DERIVED�ĳ�Ա

	//Ϊ����ָ������
	hr = AllocateName(Name, &pMeshContainer->Name);
	if (FAILED(hr))
	{
		DestroyMeshContainer(pMeshContainer);
		return hr;
	}      
	pMesh->GetDevice(&pd3dDevice);		//��ȡԭ�����d3d�豸
    NumFaces = pMesh->GetNumFaces();	//����ԭ�����������εĸ���

	//ȷ�����񶥵��������
	//���룬��ʾԭ����û�з���
    if (!(pMesh->GetFVF() & D3DFVF_NORMAL))		//�����ʽ��ͨ��λ��ȷ���������Ƿ����ĳ����Ϣ
    {											//ͨ����λ��������ȷ�ϸ�λ�Ƿ���ֵ
        pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;
        hr = pMesh->CloneMeshFVF( pMesh->GetOptions(), 
			                      pMesh->GetFVF() | D3DFVF_NORMAL, 
			                      pd3dDevice, 
			                      &pMeshContainer->MeshData.pMesh );
		if (FAILED(hr))
		{
			if(pd3dDevice)
				pd3dDevice->Release();
			DestroyMeshContainer(pMeshContainer);
			return hr;
		}
		//�����ɵľ��з��ߵ�����ֵ��ԭָ�룬�β��м�¼��ԭ���񣬲����ڴ�й¶
		pMesh = pMeshContainer->MeshData.pMesh;
        D3DXComputeNormals( pMesh, NULL );
    }
    else 
    {
        pMeshContainer->MeshData.pMesh = pMesh;
        pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;
        pMesh->AddRef();			//ԭ�����Ѿ����з�����Ϣ�������������βι�������
									//�����ʹ���߱�־+1
    }
	
    //Ϊ����ģ��׼�����ʺ�����
    pMeshContainer->NumMaterials = max(1, NumMaterials); 
    pMeshContainer->pMaterials = new D3DXMATERIAL[pMeshContainer->NumMaterials];
    pMeshContainer->ppTextures = new LPDIRECT3DTEXTURE9[pMeshContainer->NumMaterials];
    pMeshContainer->pAdjacency = new DWORD[NumFaces*3];		//�����ε��ڽ���Ϊ�����ε�3��
    if ((pMeshContainer->pAdjacency == NULL) || (pMeshContainer->pMaterials == NULL)
		|| (pMeshContainer->ppTextures == NULL))
	{
		hr = E_OUTOFMEMORY;
		if(pd3dDevice)
			pd3dDevice->Release();
		DestroyMeshContainer(pMeshContainer);
		return hr;
	}
	//�����βε��������ڽ���Ϣ����������
	//��������Ϣ�¿�
	memcpy(pMeshContainer->pAdjacency, pAdjacency, sizeof(DWORD) * NumFaces*3);  
    memset(pMeshContainer->ppTextures, 0, sizeof(LPDIRECT3DTEXTURE9) * pMeshContainer->NumMaterials);
	
   if (NumMaterials > 0)            
   {    
        //���Ʋ�������, ���ò��ʻ���������I
        memcpy(pMeshContainer->pMaterials, pMaterials, sizeof(D3DXMATERIAL) * NumMaterials); 
		pMeshContainer->pMaterials->MatD3D.Ambient = pMeshContainer->pMaterials->MatD3D.Diffuse;
       
		//��ȡ�����ļ�, �����������
        for (iMaterial = 0; iMaterial < NumMaterials; iMaterial++) 
        {
            if (pMeshContainer->pMaterials[iMaterial].pTextureFilename != NULL)
            {				
				char *name_xfile;
				name_xfile = new char;
				find_texture(name_x,&name_xfile,pMeshContainer->pMaterials[iMaterial].pTextureFilename);
					D3DXCreateTextureFromFileA(pd3dDevice,					//��������
												name_xfile,				
												&pMeshContainer->ppTextures[iMaterial]);	
				if(name_xfile != NULL)
					delete []name_xfile;
                pMeshContainer->pMaterials[iMaterial].pTextureFilename = NULL;
			}
        }
    }
    else
    {	//������û�в�����Ϣ��ʹ��Ĭ�ϵĲ�����Ϣ
        pMeshContainer->pMaterials[0].pTextureFilename = NULL;
        memset(&pMeshContainer->pMaterials[0].MatD3D, 0, sizeof(D3DMATERIAL9));
        pMeshContainer->pMaterials[0].MatD3D.Diffuse.r = 0.5f;
        pMeshContainer->pMaterials[0].MatD3D.Diffuse.g = 0.5f;
        pMeshContainer->pMaterials[0].MatD3D.Diffuse.b = 0.5f;
        pMeshContainer->pMaterials[0].MatD3D.Specular = pMeshContainer->pMaterials[0].MatD3D.Diffuse;
    }

	if(pd3dDevice)
		pd3dDevice->Release();

    //�����ǰ���������Ƥ��Ϣ
    if (pSkinInfo != NULL)
    {
		//������Ƥ������Ϣ
        pMeshContainer->pSkinInfo = pSkinInfo; 
        pSkinInfo->AddRef();
		
		//����ԭ������Ϣ
        pMeshContainer->pOrigMesh = pMesh;
        pMesh->AddRef();


		//��ȡ��������
        cBones = pSkinInfo->GetNumBones();

		//Ϊÿ������任������䱣���ʼ�任������ڴ�ռ�
        pMeshContainer->pBoneOffsetMatrices = new D3DXMATRIX[cBones];
        if (pMeshContainer->pBoneOffsetMatrices == NULL) 
        {
			hr = E_OUTOFMEMORY;
			DestroyMeshContainer(pMeshContainer);
			return hr;
		}
        
		//����ÿ������ĳ�ʼ�任����
        for (UINT iBone = 0; iBone < cBones; iBone++)
        {
            pMeshContainer->pBoneOffsetMatrices[iBone] = *(pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(iBone));
        }
		
		//������Ƥ����ģ��
        hr = GenerateSkinnedMesh(pMeshContainer); 
        if (FAILED(hr))
		{
			DestroyMeshContainer(pMeshContainer);
			return hr;
		}
    }
	
    *ppNewMeshContainer = pMeshContainer;		//����Ƥ���񵼳�
    pMeshContainer = NULL;
    return hr;
}


//-----------------------------------------------------------------------------
// Desc: �ͷſ��
//-----------------------------------------------------------------------------
HRESULT CAllocateHierarchy::DestroyFrame(LPD3DXFRAME pFrameToFree) 
{
	if(pFrameToFree != NULL)
	{
		if(pFrameToFree->Name)
			delete []pFrameToFree->Name;
		if(pFrameToFree)
			delete pFrameToFree;
	}
    return S_OK; 
}


//-----------------------------------------------------------------------------
// Desc: �ͷ���������
//-----------------------------------------------------------------------------
HRESULT CAllocateHierarchy::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase)
{
	if(pMeshContainerBase == NULL)
		return S_OK;

    UINT iMaterial;
	// ��תΪ��չ�������ڴ�й©
    D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;
	
    if(pMeshContainer->Name)
		delete []pMeshContainer->Name;
    if(pMeshContainer->pAdjacency)
		delete []pMeshContainer->pAdjacency;
    if(pMeshContainer->pMaterials)
		delete []pMeshContainer->pMaterials;
    if(pMeshContainer->pBoneOffsetMatrices)
		delete []pMeshContainer->pBoneOffsetMatrices;
	
    if (pMeshContainer->ppTextures != NULL)
    {
        for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
        {
            if(pMeshContainer->ppTextures[iMaterial])
				delete pMeshContainer->ppTextures[iMaterial];
        }
    }
	//���˺ܶ��޸�*********************************
    if(pMeshContainer->ppTextures)
		delete *(pMeshContainer->ppTextures);
    if(pMeshContainer->ppBoneMatrixPtrs)
		delete []pMeshContainer->ppBoneMatrixPtrs;
    if(pMeshContainer->pBoneCombinationBuf )
		pMeshContainer->pBoneCombinationBuf->Release();
    if(pMeshContainer->MeshData.pMesh )
		pMeshContainer->MeshData.pMesh->Release();
	if(pMeshContainer->pSkinInfo)
		pMeshContainer->pSkinInfo->Release();
    if(pMeshContainer->pOrigMesh )
		pMeshContainer->pOrigMesh->Release();
    if( pMeshContainer )
		delete pMeshContainer;
    return S_OK;
}

char* find_texture(wchar_t*name_x,char**name_xfile,char*pTextureFilename)
{
	int len_pic,len_xfile;
	char *name_pic,*end,*mid;

	len_pic = strlen(pTextureFilename);						//�������Ƴ���
	name_pic = new char[len_pic + 10];						//ͼƬ�������ƴ洢
	strcpy(name_pic,pTextureFilename);
	end = name_pic;
	mid = strchr(end,'\\');
	while(mid != NULL)										//Ѱ�������ļ���
	{
		end = ++mid;
		mid = strchr(end,'\\');
	}

	//name_xΪ����x�ļ�ǰ��¼�����ƣ���CSkinMesh�м���
	len_xfile = wcslen(name_x);								//��ȡx�ļ���
	if(*name_xfile)
		delete [](*name_xfile);
	*name_xfile = new char[len_xfile + len_pic + 10];	
	WideCharToMultiByte(CP_OEMCP,NULL,name_x,-1,*name_xfile,len_xfile + len_pic + 10,NULL,FALSE);
				
	(*name_xfile)[strlen(*name_xfile) - 2] = '\\';				//�ϳ������·��
	(*name_xfile)[strlen(*name_xfile) - 1] = 0;
	strcat(*name_xfile,end);
	if(name_pic)
		delete []name_pic;
return *name_xfile;
}