#pragma once
#include <d3d12.h>

class IndexBuffer {

	public:

		//IndexBuffer生成
	    void Create(const UINT size, const UINT stride);

		//ゲッター
	    ID3D12Resource* Get(); // インデックスバッファービュー
	    D3D12_INDEX_BUFFER_VIEW* GetView(); // インデックスバッファービュー
	   

		//コンストラクタ
	    IndexBuffer();

		// デストラクタ
	    ~IndexBuffer();


	private:

	    // インデックスバッファー
	    ID3D12Resource* indexBuffer_ = nullptr;
	    // インデックスバッファービュー
	    D3D12_INDEX_BUFFER_VIEW indexBufferView_ = {};
	  







};
