//
// バーテックスシェーダー
//

// CPU側から送られるデータ
attribute vec4 position;     // 頂点座標
attribute vec3 normal;       // 頂点法線 


// CPU側から贈られるデータ
uniform mat4 uni_proj_mat;
uniform mat4 uni_model_mat;
uniform mat3 uni_normal_mat;

// フラグメントシェーダーへ送る変数
varying float intensity;         // フラグメントシェーダーに送る


void main()
{
  vec3 light = vec3(0.0, 0.0, 1.0);   // 光源のベクトル

  vec3 n = uni_normal_mat * normal;
  intensity  = dot(light, n);          // 面の明るさ??

	gl_Position = uni_proj_mat * uni_model_mat * position;
}
