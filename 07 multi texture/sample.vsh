//
// バーテックスシェーダー
//

attribute vec4 position;
attribute vec3 normal;
attribute vec3 tangent;
attribute vec2 uv;

uniform mat4 uni_proj_mat;
uniform mat4 uni_model_mat;
uniform mat3 uni_normal_mat;

varying vec2 vary_uv;
varying vec3 vary_light_dir;


// 転置行列
// SOURCE https://github.com/AnalyticalGraphicsInc/cesium/blob/master/Source/Shaders/Builtin/Functions/transpose.glsl
mat3 czm_transpose(mat3 matrix)
{
    return mat3(
        matrix[0][0], matrix[1][0], matrix[2][0],
        matrix[0][1], matrix[1][1], matrix[2][1],
        matrix[0][2], matrix[1][2], matrix[2][2]);
}


void main()
{
  vary_uv = uv;

  // 接ベクトル
  vec3 z_vec = uni_normal_mat * normal;
  vec3 y_vec = uni_normal_mat * tangent;
  vec3 x_vec = cross(y_vec, z_vec);

  // 接ベクトルから行列を生成
  mat3 m = czm_transpose(mat3(x_vec,
                              y_vec,
                              z_vec));

  // 光源の向きを接空間の座標系へ変換
  vary_light_dir = m * vec3(0, 0, 1);

	gl_Position = uni_proj_mat * uni_model_mat * position;
}
