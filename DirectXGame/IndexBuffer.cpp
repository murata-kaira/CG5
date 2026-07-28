#include "IndexBuffer.h"
#include "KamataEngine.h"

#include <cassert>
#include <d3d12.h>

using namespace KamataEngine;

// IndexBufferの生成
void IndexBuffer::Create(const UINT size, const UINT stride) {

	assert(stride == 2 || stride == 4); // 2byte or 4byte のみ受け付ける
	DXGI_FORMAT format = (stride == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;

	// クラス内でdxCommonを利用するために追加
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// IndexBufferの生成-----------
	// インデックスリソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; // CPUから書き込むヒープ
	// インデックスリソースの設定
	D3D12_RESOURCE_DESC indexResourceDesc{};
	indexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER; // バッファ
	indexResourceDesc.Width = size;                                // リソースのサイズ
	// バッファの場合はこれらは1にする決まり
	indexResourceDesc.Height = 1;
	indexResourceDesc.DepthOrArraySize = 1;
	indexResourceDesc.MipLevels = 1;
	indexResourceDesc.SampleDesc.Count = 1;
	// バッファの場合はこれにする決まり
	indexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// インデックスリソースを生成する
	ID3D12Resource* indexResource = nullptr;
	[[maybe_unused]]
	HRESULT hr =
	    dxCommon->GetDevice()->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &indexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&indexResource));
	assert(SUCCEEDED(hr));

	// 生成したインデックスリソースをとっておく
	indexBuffer_ = indexResource;

	// IndexBufferViewを生成する---------
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};
	// リソースの先頭アドレスから使う
	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	// 使用するリソースのサイズはインデックスの全サイズ
	indexBufferView.SizeInBytes = size; // インデックスリソース1つ分のサイズ
	// インデックス1つ分のサイズ
	indexBufferView.Format = format; // インデックス1つ分のサイズ

	// IndexBufferViewをとっておく
	indexBufferView_ = indexBufferView;
}

// 生成したインデックスバッファを返す
ID3D12Resource* IndexBuffer::Get() {
	return indexBuffer_;
}

// 用意済みのインデックスバッファービューを返す
D3D12_INDEX_BUFFER_VIEW* IndexBuffer::GetView() {
	return &indexBufferView_;
}


// コンストラクタ
IndexBuffer::IndexBuffer() {}

// デストラクタ
IndexBuffer::~IndexBuffer() {
	if (indexBuffer_) {
		indexBuffer_->Release();
		indexBuffer_ = nullptr;
	}
}





