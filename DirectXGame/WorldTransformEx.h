#pragma once
#include "KamataEngine.h"

class WorldTransformEx : public KamataEngine::WorldTransform {

public:

	
	void UpdateMatrix();

	KamataEngine::Matrix4x4 MakeAffineMatrix();

	
	


};
