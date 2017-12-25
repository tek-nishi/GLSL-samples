//
// バーテックスシェーダー
//  拡散光・環境光・鏡面反射の計算
//

// CPU側から送られるデータ
attribute vec4 position;     // 頂点座標
attribute vec3 normal;       // 頂点法線
attribute vec2 uv;

// CPU側から送られるデータ
uniform mat4 uni_proj_mat;             // 透視変換
uniform mat4 uni_model_mat;            // モデルビュー
uniform mat3 uni_normal_mat;           // モデルビューの逆転置

// フラグメントシェーダーへ送る変数
varying vec3 vary_position;           // 視点座標
varying vec3 vary_normal;             // 法線ベクトル
varying vec2 vary_uv;


void main()
{
  vary_position = vec3(uni_model_mat * -position);
  vary_normal   = uni_normal_mat * normal;
  vary_uv       = uv;

	gl_Position = uni_proj_mat * uni_model_mat * position;
}
