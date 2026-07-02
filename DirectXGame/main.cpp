#include <Windows.h>
#include"KamataEngine.h"
#include"Shader.h"
#include <cassert>
#include "RootSignature.h"
#include "PipelineState.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

using namespace KamataEngine;

//関数プロトタイプ宣言
void SetupPipelineState(PipelineState& pipelineState, RootSignature& rs, Shader& vs, Shader& ps) {

	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[1] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	// BlendState------------今回は不透明
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasterizerState-----------
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面をカリングする
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 塗りつぶしモードをリゾットにする
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// PSO(PipelineStateObject)の生成------
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rs.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = {vs.GetDxcBlob()->GetBufferPointer(), vs.GetDxcBlob()->GetBufferSize()};
	graphicsPipelineStateDesc.PS = {ps.GetDxcBlob()->GetBufferPointer(), ps.GetDxcBlob()->GetBufferSize()};
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;

	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジのタイプ
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むかの設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// 準備は整ったので、PSOを生成する
	pipelineState.Create(graphicsPipelineStateDesc);

};


// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// エンジンの初期化
	KamataEngine::Initialize(L"LE3D_24_ムラタ_カイラ");

	// DirectXCommonのインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// DirectXCommonクラスが管理している、ウインドウの幅と高さの値の取得
	int32_t w = dxCommon->GetBackBufferWidth();
	int32_t h = dxCommon->GetBackBufferHeight();
	DebugText::GetInstance()->ConsolePrintf(std::format("width: {}, height: {}\n", w, h).c_str());

	// DirectXCommonクラスが管理している、コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon->GetCommandList();

	// RootSignatureの作成-----------------
	RootSignature rs;
	rs.Create();


	
	// 頂点シェーダの読み込みとコンパイル
	Shader vs;
	vs.LoadDxc(L"Resources/Shaders/TestVS.hlsl", L"vs_6_0");
	assert(vs.GetDxcBlob() != nullptr);

	// ピクセルシェーダの読み込みとコンパイル
	Shader ps;
	ps.LoadDxc(L"Resources/Shaders/TestPS.hlsl", L"ps_6_0");
	assert(ps.GetDxcBlob() != nullptr);

	
	



	// PipelineStateの生成-----------------
	PipelineState pipelineState;
	SetupPipelineState(pipelineState, rs, vs, ps);

	
	struct VertexData {
		Vector4 position;
	};

	//頂点データの準備（全画面を覆う4頂点）
	VertexData vertices[] = {
		{-1.0f, 1.0f, 0.0f, 1.0f},  // 左上
		{1.0f, 1.0f, 0.0f, 1.0f},   // 右上
		{-1.0f, -1.0f, 0.0f, 1.0f}, // 左下
		{1.0f, -1.0f, 0.0f, 1.0f}   // 右下
	};


	



	// VertexBufferの生成-----------------
	VertexBuffer vb;
	//vb.Create(sizeof(Vector4) * 3, sizeof(Vector4));
	vb.Create(sizeof(vertices), sizeof(vertices[0]));
	
	//頂点リソースに書き込む-------------
	VertexData* pGpuVertexData = nullptr;
	vb.Get()->Map(0, nullptr, reinterpret_cast<void**>(& pGpuVertexData));

	for (int i = 0; i < _countof(vertices); i++) {
		pGpuVertexData[i] = vertices[i];
	}

	// インデックスデータ（2つの三角形で四角形を描画）
	uint16_t indices[] = {
		0, 1, 2, // 1つ目の三角形（左上、右上、左下）
		2, 1, 3  // 2つ目の三角形（左下、右上、右下）
	};


	//IndexBufferの生成-----------------
	IndexBuffer ib;
	ib.Create(sizeof(indices), sizeof(indices[0]));
	//頂点インデックスリソースに書き込む-------------
	uint16_t* pGpuIndexData = nullptr;
	ib.Get()->Map(0, nullptr, reinterpret_cast<void**>(&pGpuIndexData));

	for (int i = 0; i < _countof(indices); i++) {
		pGpuIndexData[i] = indices[i];
	}




	// メインループ
	while (true) {
		// エンジンの更新
		if (KamataEngine::Update()) {
			break;
		}
		// 描画開始
		dxCommon->PreDraw();


		// コマンドを積む
		commandList->SetGraphicsRootSignature(rs.Get());
		commandList->SetPipelineState(pipelineState.Get());
		commandList->IASetVertexBuffers(0, 1, vb.GetView());
		commandList->IASetIndexBuffer(ib.GetView());
		//トポロジの設定
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//頂点数,インデックス数,インデックスの開始位置,インデックスのオフセット
		commandList->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0);

		// 描画終了		
		dxCommon->PostDraw();
	}



	//エンジンの終了処理
	KamataEngine::Finalize();

	return 0;
}
