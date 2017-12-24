//
// バーテックスシェーダー
//  拡散光・環境光・鏡面反射の計算
//

// CPU側から送られるデータ
attribute vec4 position;     // 頂点座標
attribute vec3 normal;       // 頂点法線 

// CPU側から送られるデータ
uniform mat4 uni_proj_mat;             // 透視変換
uniform mat4 uni_model_mat;            // モデルビュー
uniform mat3 uni_normal_mat;           // モデルビューの逆転置

uniform vec3 uni_deffuse_dir;
uniform vec4 uni_deffuse;
uniform vec4 uni_ambient;
uniform vec4 uni_specular;
uniform float uni_shine;

// フラグメントシェーダーへ送る変数
varying vec4 vary_color;         // フラグメントシェーダーに送る


void main()
{
  // 視点ベクトル
  vec3 v = normalize(vec3(uni_model_mat * -position));
  // 法線ベクトルを視点座標へ変換
  vec3 n = uni_normal_mat * normal;
  // 拡散光の強さ
  float di = max(dot(uni_deffuse_dir, n), 0.0);

  // 光源ベクトルが反射した時のベクトル
  vec3 rl = reflect(-uni_deffuse_dir, n);
  // スペキュラの強さ
  float si = pow(max(dot(v, rl), 0.0), uni_shine);
 
  // 最終的な色を決める
  vary_color = uni_ambient + uni_deffuse * di + uni_specular * si;

	gl_Position = uni_proj_mat * uni_model_mat * position;
}
