#include "WorldTransform.h"

void WorldTransform::UpdateMatrix() {
	// スケール、回転、屁に乞う移動を合成して行列を計算する
	matWorld_ = Matrix::MakeAffineMatrix(scale_, rotation_, translation_);
	// 定数バッファに転送する
	TransferMatrix();
}
