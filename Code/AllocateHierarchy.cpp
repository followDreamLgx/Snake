
#include "AllocateHierarchy.h"

extern LPDIRECT3DDEVICE9	g_pd3dDevice;
char* find_texture(wchar_t*,char**,char*);
/*-----------------------------------------------------------------------------
// Desc: 
1. 将第一个参数的文件名复制到第二个参数中
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
// Desc: 从绝对路径中提取文件名
//-----------------------------------------------------------------------------
void CAllocateHierarchy::RemovePathFromFileName(LPSTR fullPath, LPWSTR fileName)
{
	//先将fullPath的类型变换为LPWSTR
	WCHAR wszBuf[MAX_PATH];
	MultiByteToWideChar( CP_ACP, 0, fullPath, -1, wszBuf, MAX_PATH );
	wszBuf[MAX_PATH-1] = L'\0';

	WCHAR* wszFullPath = wszBuf;

	//从绝对路径中提取文件名
	LPWSTR pch=wcsrchr(wszFullPath,'\\');
	if (pch)
		lstrcpy(fileName, ++pch);
	else
		lstrcpy(fileName, wszFullPath);
}


//-----------------------------------------------------------------------------
//Desc: 生成蒙皮网格模型(含有每个顶点的混合权重、索引和一个骨骼组合表)
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

	//获取网格模型索引缓冲区
	LPDIRECT3DINDEXBUFFER9 pIB;
	if (FAILED(hr = pMeshContainer->pOrigMesh->GetIndexBuffer(&pIB)))
		return E_FAIL;

	//获取影响一个面(三角形)的矩阵数量
	DWORD NumMaxFaceInfl;
	hr = pMeshContainer->pSkinInfo->GetMaxFaceInfluences(pIB, 
		pMeshContainer->pOrigMesh->GetNumFaces(), &NumMaxFaceInfl);
	pIB->Release();
	if (FAILED(hr))
		return E_FAIL;

	//影响一个面的矩阵数量不会超过12
	NumMaxFaceInfl = min(NumMaxFaceInfl, 12);

	//获取当前设备的能力
	IDirect3DDevice9* pd3dDevice = g_pd3dDevice;		//注意，这个地方是全局变量**************
	D3DCAPS9 d3dCaps;
    pd3dDevice->GetDeviceCaps( &d3dCaps );

	//根据当前设备能力和NumMaxFaceInfl, 设置在进行骨骼蒙皮调色时使用骨骼数量的上限
	//由于需要分配一部分索引缓存空间给法线，所以当使用硬件计算的时候，
	//NumMaxFaceInfl(影响一个网格的骨骼数，有可能从根骨骼影响到目前）的最大值
	//只能是MaxVertexBlendMatrixIndex + 1的一半。该值表示混合矩阵的最大索引
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

	//生成蒙皮网格模型
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
// Desc: 创建框架, 仅仅是分配内存和初始化,还没有对其成员赋予合适的值
//-----------------------------------------------------------------------------
HRESULT CAllocateHierarchy::CreateFrame(LPCSTR Name, LPD3DXFRAME *ppNewFrame)
{
    HRESULT hr = S_OK;
    D3DXFRAME_DERIVED *pFrame;
	
    *ppNewFrame = NULL;
	
    pFrame = new D3DXFRAME_DERIVED;  //创建框架结构对象
    if (pFrame == NULL) 
    {
        return E_OUTOFMEMORY;
    }
	
	//为框架指定名称
    hr = AllocateName(Name, (LPSTR*)&pFrame->Name);
	if (FAILED(hr))
	{
		delete pFrame;
		return hr;
	}
	
	//初始化D3DXFRAME_DERIVED结构其它成员变量
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
// Desc: 在这里是调用了成员函数 GenerateSkinnedMesh(pMeshContainer);
//       是在这里加载了蒙皮信息
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
    UINT NumFaces;       //网格中的面数,在填充网格容器结构的邻接信息成员时使用
    UINT iMaterial;      //纹理操作时的循环变量
    UINT cBones;         //当前网格模型骨骼总数
    LPDIRECT3DDEVICE9 pd3dDevice = NULL;
    LPD3DXMESH pMesh    = NULL;				//pMesh的存在是为了服务pMeshContainer
    *ppNewMeshContainer = NULL;
	//判断其类型是否是网格类型
    if (pMeshData->Type != D3DXMESHTYPE_MESH)
    {
        return E_FAIL;
    }
	
    pMesh = pMeshData->pMesh;
	//判断是否具有灵活顶点信息
    if (pMesh->GetFVF() == 0)
    {
        return E_FAIL;
    }

	//为网格容器分配内存
    pMeshContainer = new D3DXMESHCONTAINER_DERIVED;
    if (pMeshContainer == NULL)
    {
        return E_OUTOFMEMORY;
    }
    memset(pMeshContainer, 0, sizeof(D3DXMESHCONTAINER_DERIVED));
	
	//填充网格容器结构D3DXMESHCONTAINER_DERIVED的成员

	//为网格指定名称
	hr = AllocateName(Name, &pMeshContainer->Name);
	if (FAILED(hr))
	{
		DestroyMeshContainer(pMeshContainer);
		return hr;
	}      
	pMesh->GetDevice(&pd3dDevice);		//获取原网格的d3d设备
    NumFaces = pMesh->GetNumFaces();	//计算原网格中三角形的个数

	//确保网格顶点包含法线
	//进入，表示原网格没有法线
    if (!(pMesh->GetFVF() & D3DFVF_NORMAL))		//灵活顶点格式中通过位来确定网格中是否具有某种信息
    {											//通过按位与运算来确认该位是否有值
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
		//将生成的具有法线的网格赋值给原指针，形参有记录了原网格，不怕内存泄露
		pMesh = pMeshContainer->MeshData.pMesh;
        D3DXComputeNormals( pMesh, NULL );
    }
    else 
    {
        pMeshContainer->MeshData.pMesh = pMesh;
        pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;
        pMesh->AddRef();			//原网格已经具有法线信息，网格容器与形参共用网格
									//网格的使用者标志+1
    }
	
    //为网格模型准备材质和纹理
    pMeshContainer->NumMaterials = max(1, NumMaterials); 
    pMeshContainer->pMaterials = new D3DXMATERIAL[pMeshContainer->NumMaterials];
    pMeshContainer->ppTextures = new LPDIRECT3DTEXTURE9[pMeshContainer->NumMaterials];
    pMeshContainer->pAdjacency = new DWORD[NumFaces*3];		//三角形的邻接数为三角形的3倍
    if ((pMeshContainer->pAdjacency == NULL) || (pMeshContainer->pMaterials == NULL)
		|| (pMeshContainer->ppTextures == NULL))
	{
		hr = E_OUTOFMEMORY;
		if(pd3dDevice)
			pd3dDevice->Release();
		DestroyMeshContainer(pMeshContainer);
		return hr;
	}
	//复制形参的三角形邻接信息到网格容器
	//将纹理信息致空
	memcpy(pMeshContainer->pAdjacency, pAdjacency, sizeof(DWORD) * NumFaces*3);  
    memset(pMeshContainer->ppTextures, 0, sizeof(LPDIRECT3DTEXTURE9) * pMeshContainer->NumMaterials);
	
   if (NumMaterials > 0)            
   {    
        //复制材质属性, 设置材质环境光属性I
        memcpy(pMeshContainer->pMaterials, pMaterials, sizeof(D3DXMATERIAL) * NumMaterials); 
		pMeshContainer->pMaterials->MatD3D.Ambient = pMeshContainer->pMaterials->MatD3D.Diffuse;
       
		//提取纹理文件, 创建纹理对象
        for (iMaterial = 0; iMaterial < NumMaterials; iMaterial++) 
        {
            if (pMeshContainer->pMaterials[iMaterial].pTextureFilename != NULL)
            {				
				char *name_xfile;
				name_xfile = new char;
				find_texture(name_x,&name_xfile,pMeshContainer->pMaterials[iMaterial].pTextureFilename);
					D3DXCreateTextureFromFileA(pd3dDevice,					//加载纹理
												name_xfile,				
												&pMeshContainer->ppTextures[iMaterial]);	
				if(name_xfile != NULL)
					delete []name_xfile;
                pMeshContainer->pMaterials[iMaterial].pTextureFilename = NULL;
			}
        }
    }
    else
    {	//网格中没有材质信息，使用默认的材质信息
        pMeshContainer->pMaterials[0].pTextureFilename = NULL;
        memset(&pMeshContainer->pMaterials[0].MatD3D, 0, sizeof(D3DMATERIAL9));
        pMeshContainer->pMaterials[0].MatD3D.Diffuse.r = 0.5f;
        pMeshContainer->pMaterials[0].MatD3D.Diffuse.g = 0.5f;
        pMeshContainer->pMaterials[0].MatD3D.Diffuse.b = 0.5f;
        pMeshContainer->pMaterials[0].MatD3D.Specular = pMeshContainer->pMaterials[0].MatD3D.Diffuse;
    }

	if(pd3dDevice)
		pd3dDevice->Release();

    //如果当前网格包含蒙皮信息
    if (pSkinInfo != NULL)
    {
		//加载蒙皮网格信息
        pMeshContainer->pSkinInfo = pSkinInfo; 
        pSkinInfo->AddRef();
		
		//保留原网格信息
        pMeshContainer->pOrigMesh = pMesh;
        pMesh->AddRef();


		//获取骨骼数量
        cBones = pSkinInfo->GetNumBones();

		//为每块骨骼变换矩阵分配保存初始变换矩阵的内存空间
        pMeshContainer->pBoneOffsetMatrices = new D3DXMATRIX[cBones];
        if (pMeshContainer->pBoneOffsetMatrices == NULL) 
        {
			hr = E_OUTOFMEMORY;
			DestroyMeshContainer(pMeshContainer);
			return hr;
		}
        
		//保存每块骨骼的初始变换矩阵
        for (UINT iBone = 0; iBone < cBones; iBone++)
        {
            pMeshContainer->pBoneOffsetMatrices[iBone] = *(pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(iBone));
        }
		
		//生成蒙皮网格模型
        hr = GenerateSkinnedMesh(pMeshContainer); 
        if (FAILED(hr))
		{
			DestroyMeshContainer(pMeshContainer);
			return hr;
		}
    }
	
    *ppNewMeshContainer = pMeshContainer;		//将蒙皮网格导出
    pMeshContainer = NULL;
    return hr;
}


//-----------------------------------------------------------------------------
// Desc: 释放框架
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
// Desc: 释放网格容器
//-----------------------------------------------------------------------------
HRESULT CAllocateHierarchy::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase)
{
	if(pMeshContainerBase == NULL)
		return S_OK;

    UINT iMaterial;
	// 先转为扩展型以免内存泄漏
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
	//做了很多修改*********************************
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

	len_pic = strlen(pTextureFilename);						//纹理名称长度
	name_pic = new char[len_pic + 10];						//图片完整名称存储
	strcpy(name_pic,pTextureFilename);
	end = name_pic;
	mid = strchr(end,'\\');
	while(mid != NULL)										//寻找纹理文件名
	{
		end = ++mid;
		mid = strchr(end,'\\');
	}

	//name_x为加载x文件前记录的名称，在CSkinMesh中加载
	len_xfile = wcslen(name_x);								//获取x文件名
	if(*name_xfile)
		delete [](*name_xfile);
	*name_xfile = new char[len_xfile + len_pic + 10];	
	WideCharToMultiByte(CP_OEMCP,NULL,name_x,-1,*name_xfile,len_xfile + len_pic + 10,NULL,FALSE);
				
	(*name_xfile)[strlen(*name_xfile) - 2] = '\\';				//合成纹理的路径
	(*name_xfile)[strlen(*name_xfile) - 1] = 0;
	strcat(*name_xfile,end);
	if(name_pic)
		delete []name_pic;
return *name_xfile;
}