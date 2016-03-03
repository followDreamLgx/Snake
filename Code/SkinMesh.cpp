//=============================================================================
// SkinMesh.cpp: 蒙皮网格模型类的实现
//=============================================================================

#include "SkinMesh.h"

//--------------------------------------------------------------------
// Desc: 构造函数和析构函数  Construction/Destruction
//--------------------------------------------------------------------
CSkinMesh::CSkinMesh()
{
	m_bPlayAnim         = true;
	m_pd3dDevice        = NULL;
    m_pAnimController   = NULL;
    m_pFrameRoot        = NULL;
    m_pBoneMatrices     = NULL;
	m_pAnimationset		= NULL;
    m_NumBoneMatricesMax = 0;

	m_pAlloc = new CAllocateHierarchy();
}


//-----------------------------------------------------------------------------
// Desc: 构造函数和析构函数 
//-----------------------------------------------------------------------------
CSkinMesh::~CSkinMesh()
{
	D3DXFrameDestroy(m_pFrameRoot, m_pAlloc);
    if(m_pAnimController)
		m_pAnimController->Release();
	delete m_pAlloc;
}
 

//-----------------------------------------------------------------------------
// Desc:创建并加载蒙皮网格模型
//-----------------------------------------------------------------------------
HRESULT CSkinMesh::OnCreate(LPDIRECT3DDEVICE9 pD3DDevice, WCHAR *strFileName)
{
	HRESULT hr;
	m_pd3dDevice = pD3DDevice;
	hr = LoadFromXFile(strFileName);
	if(FAILED(hr))
		return hr;
	return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: 从文件加载蒙皮网格模型
//-----------------------------------------------------------------------------
HRESULT CSkinMesh::LoadFromXFile(WCHAR *strFileName)
{
    HRESULT hr;
	//记录x文件名，用于创建纹理路径（注意加载x文件与记录文件名的顺序）
	m_pAlloc->name_x = new wchar_t[wcslen(strFileName) + 10];
	wcscpy(m_pAlloc->name_x,strFileName);
	//从.X文件加载层次框架和动画数据
	if(FAILED(D3DXLoadMeshHierarchyFromX(strFileName, D3DXMESH_MANAGED, m_pd3dDevice, 
		                            m_pAlloc, NULL, &m_pFrameRoot, &m_pAnimController)))
		MessageBox(0,L"SkinMesh.cpp -- LoadFromXFile error——1",0,0);
	
	//建立各级框架的组合变换矩阵
    if(FAILED(SetupBoneMatrixPointers(m_pFrameRoot)))
		MessageBox(0,L"SkinMesh.cpp -- LoadFromXFile error——2",0,0);  
	
	//计算框架对象的边界球
    hr = D3DXFrameCalculateBoundingSphere(m_pFrameRoot, &m_vObjectCenter, &m_fObjectRadius);
	if (FAILED(hr))
        return hr;

	return S_OK;
}


//--------------------------------------------------------------------------
// Desc: 仅在LoadFromXFile中调用。调用子函数SetupBoneMatrixPointersOnMesh()
//       安置好各级框架(实际上是各个骨骼)的组合变换矩阵。
// 注意: 在这里其实并没有计算出各个骨骼的组合变换矩阵，只是为每个矩阵开辟了相应
//       的存储空间，真正的计算是在函数CSkinMesh::UpdateFrameMatrices()中完成的。
//---------------------------------------------------------------------------
HRESULT CSkinMesh::SetupBoneMatrixPointers(LPD3DXFRAME pFrame)		//该参数首次调用对应的实参为根框架
{
    HRESULT hr;

	//只有当形参为根框架的时候才会调用此函数******************************跳过
    if (pFrame->pMeshContainer != NULL)
    {
        hr = SetupBoneMatrixPointersOnMesh(pFrame->pMeshContainer);  //调用子函数
        if (FAILED(hr))
            return hr;
    }

    if (pFrame->pFrameSibling != NULL)
    {
        hr = SetupBoneMatrixPointers(pFrame->pFrameSibling);   //递归
        if (FAILED(hr))
            return hr;
    }

    if (pFrame->pFrameFirstChild != NULL)
    {
        hr = SetupBoneMatrixPointers(pFrame->pFrameFirstChild);  //递归
        if (FAILED(hr))
            return hr;
    }

    return S_OK;
}

//跳过*************
//-----------------------------------------------------------------------------
// Desc: 仅在SetupBoneMatrixPointers()中被调用，设置每个骨骼的组合变换矩阵
//-----------------------------------------------------------------------------
HRESULT CSkinMesh::SetupBoneMatrixPointersOnMesh(LPD3DXMESHCONTAINER pMeshContainerBase)	//通过根框架的网格调用此函数
																		//即，在本网格中的操作只与根框架有关
{
    UINT iBone, cBones;  // cBones表示骨骼数量，iBone表示循环变量
    D3DXFRAME_DERIVED *pFrame;

	//先强制转为扩展型
    D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;

    // 只有蒙皮网格模型才有骨骼矩阵
    if (pMeshContainer->pSkinInfo != NULL)
    {
		//得到骨骼数量
        cBones = pMeshContainer->pSkinInfo->GetNumBones();

		//申请存储骨骼矩阵的空间,注意这是指针的空间
        pMeshContainer->ppBoneMatrixPtrs = new D3DXMATRIX*[cBones];     
		if (pMeshContainer->ppBoneMatrixPtrs == NULL)
            return E_OUTOFMEMORY;

        for (iBone = 0; iBone < cBones; iBone++)
        {
			//通过循环，遍历骨骼框架，寻找所有骨骼
            pFrame = (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pFrameRoot, pMeshContainer->pSkinInfo->GetBoneName(iBone));
            if (pFrame == NULL)
                return E_FAIL;

			//将框架的组合变换矩阵赋值给对应的骨骼的复合变换矩阵
            pMeshContainer->ppBoneMatrixPtrs[iBone] = &pFrame->CombinedTransformationMatrix;
		}
    }

    return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: 更新框架并绘制框架
//       (1)用m_pAnimController->AdvanceTime()设置时间，m_pAnimController是
//          类LPD3DXANIMATIONCONTROLLER的一个对象
//       (2)用函数CSkinMesh::UpdateFrameMatrices()更新框架
//       (3)用函数CSkinMesh::DrawFrame()绘制框架
//-----------------------------------------------------------------------------
HRESULT CSkinMesh::Render(D3DXMATRIXA16* matWorld,double fElapsedAppTime)
{
    if( 0.0f == fElapsedAppTime ) 
        return S_OK;

	if (m_bPlayAnim && m_pAnimController != NULL)		//动画时间设置，动作的时间差
		m_pAnimController->AdvanceTime(fElapsedAppTime, NULL );

	UpdateFrameMatrices(m_pFrameRoot, matWorld);		//更新整个骨骼框架，调用子函数
	DrawFrame(m_pFrameRoot);							//绘制整个蒙皮骨骼，调用子函数
	
	return S_OK;
}


/*-----------------------------------------------------------------------------
// Desc:计算各个骨骼的组合变换矩阵
1. 传入父类骨骼指针，父类世界变换矩阵（根骨骼的世界变换矩阵为外界传入的世界变换矩阵，即第一次调用时，
	传入了根框架与外界的世界变换，可以理解为根的父框架为外界世界变换）
2. 通过当前的初始变换矩阵与父类的变换矩阵得到组合变换矩阵
3. 先将兄弟骨骼完成上述操作
4. 再从小兄弟的儿子开始，重复他以及他的兄弟的矩阵变换，
5. 小兄弟完成后，再从从他的小哥哥开始重复第四个动作，直到全部完成
//---------------------------------------------------------------------------*/
VOID CSkinMesh::UpdateFrameMatrices(LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix)
{
    D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED*)pFrameBase;

	//将父类世界变换（即父类组合变换矩阵）与初始变换矩阵结合形成当前骨骼的组合变换矩阵
    if (pParentMatrix != NULL)
        D3DXMatrixMultiply(&pFrame->CombinedTransformationMatrix, &pFrame->TransformationMatrix, pParentMatrix);
    else
        pFrame->CombinedTransformationMatrix = pFrame->TransformationMatrix;
	//兄弟递归
    if (pFrame->pFrameSibling != NULL)
    {
        UpdateFrameMatrices(pFrame->pFrameSibling, pParentMatrix);
    }
	//儿子递归
    if (pFrame->pFrameFirstChild != NULL)
    {
        UpdateFrameMatrices(pFrame->pFrameFirstChild, &pFrame->CombinedTransformationMatrix);
    }
}


//-----------------------------------------------------------------------------
// Desc: 绘制框架.
//       先用CSkinMesh::DrawMeshContainer()绘制一个LPD3DXMESHCONTAINER类型
//       的变量pMeshContainer.然后递归绘制同一级框架和子一级框架。
//-----------------------------------------------------------------------------
VOID CSkinMesh::DrawFrame(LPD3DXFRAME pFrame)
{
    LPD3DXMESHCONTAINER pMeshContainer;
	//获取当前骨骼对应的网格
    pMeshContainer = pFrame->pMeshContainer;
    while (pMeshContainer != NULL)
    {	
        DrawMeshContainer(pMeshContainer, pFrame);				//绘制当前骨骼的子网络
        pMeshContainer = pMeshContainer->pNextMeshContainer;	//获取下一个子网络
    }
	
    if (pFrame->pFrameSibling != NULL)
    {
        DrawFrame(pFrame->pFrameSibling);
    }
	
    if (pFrame->pFrameFirstChild != NULL)
    {
        DrawFrame(pFrame->pFrameFirstChild);
    }
}


//-----------------------------------------------------------------------------
// Name: DrawMeshContainer()
// Desc: Called to render a mesh in the hierarchy
//-----------------------------------------------------------------------------
VOID CSkinMesh::DrawMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase)
{
	//获取蒙皮信息
	D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;
	//获取骨骼信息
	D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED*)pFrameBase;
	UINT iMaterial;
	UINT iAttrib;
	LPD3DXBONECOMBINATION pBoneComb;

	UINT iMatrixIndex;
	UINT iPaletteEntry;
	D3DXMATRIXA16 matTemp;

	if (pMeshContainer->pSkinInfo != NULL) //如果是蒙皮网格
	{
		//检查是否使用软件顶点混合

		//如果当前硬件不支持, 则使用软件顶点处理
		if (pMeshContainer->UseSoftwareVP)
			m_pd3dDevice->SetSoftwareVertexProcessing(true);

		//启用索引顶点混合
		if (pMeshContainer->NumInfl)
			m_pd3dDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, true);
		
		//根据影响当前网格模型顶点的骨骼数量, 设置需要使用的混合矩阵索引数量
		//已经经过优化，原代码可以查看缩略部分
		if (pMeshContainer->NumInfl == 1)			//D3DVBF_OWEIGHTS = 256
			m_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_0WEIGHTS);
		else 
			m_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, pMeshContainer->NumInfl - 1);

		//逐个子网格渲染进行渲染
		//获取组合变换矩阵的索引缓存
		pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
		for (iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)		//遍历子网格
		{
			//设置混合矩阵，NumPalleteEntries为骨骼的上限
			//外循环获取一个子网格，内循环逐个设置子网格的各个世界变换矩阵
			for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
			{
				iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
				if (iMatrixIndex != UINT_MAX)
				{
					D3DXMatrixMultiply( &matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], 
						                 pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex] );
					m_pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( iPaletteEntry ), &matTemp );
				}
			}

			//设置材质和纹理
			m_pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D );
			m_pd3dDevice->SetTexture( 0, pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId] );
			
			//渲染子网格模型
			pMeshContainer->MeshData.pMesh->DrawSubset( iAttrib );
		}

		//恢复顶点混合状态
		//不一定需要，但是这是系统的默认设置，加了也没事
		m_pd3dDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
		m_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE);

		//恢复顶点处理模式
		if (pMeshContainer->UseSoftwareVP)
			m_pd3dDevice->SetSoftwareVertexProcessing(false);
	} 
	else  // 如果只是普通网格，在添加材质后就绘制它。
	{
		m_pd3dDevice->SetTransform(D3DTS_WORLD, &pFrame->CombinedTransformationMatrix);
		for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
		{
			m_pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[iMaterial].MatD3D );
			m_pd3dDevice->SetTexture( 0, pMeshContainer->ppTextures[iMaterial] );
			pMeshContainer->MeshData.pMesh->DrawSubset(iMaterial);
		}
	}
}
//----------------------------------------------------------------------------
//Desc:设置动画
//---------------------------------------------------------------------------
void CSkinMesh::set_animation(char *name)
{
	static DWORD new_track = 1,old_track = 0;
	float currtime = 0;
	//返回动画集
	m_pAnimController->GetAnimationSetByName(name,&m_pAnimationset);
	new_track = old_track == 0 ? 1 :0;	
	m_pAnimController->SetTrackAnimationSet(new_track,m_pAnimationset);

	m_pAnimController->UnkeyAllTrackEvents(old_track);		//清除Track通道的所有事件
	m_pAnimController->UnkeyAllTrackEvents(new_track);
	m_pAnimController->KeyTrackEnable(old_track,false,currtime + 0.25);	//关闭原通道
	m_pAnimController->SetTrackEnable(new_track,true);					//开启新的通道
	//改变两个通道的速率
	m_pAnimController->KeyTrackSpeed(old_track,0.0f,currtime,0.25f,D3DXTRANSITION_LINEAR);
	m_pAnimController->KeyTrackSpeed(new_track,1.0f,currtime,0.25f,D3DXTRANSITION_LINEAR);
	//改变两个通道的权重
	m_pAnimController->KeyTrackWeight(old_track,0.0f,currtime,0.25f,D3DXTRANSITION_LINEAR);	
	m_pAnimController->KeyTrackWeight(new_track,1.0f,currtime,0.25f,D3DXTRANSITION_LINEAR);
	//修改轨道标志
	old_track = new_track;
}

//-----------------------------------------------------------------------------
// Desc: 释放蒙皮网格模型
//-----------------------------------------------------------------------------
HRESULT CSkinMesh::OnDestory()
{
	delete this;
	return S_OK;
}
