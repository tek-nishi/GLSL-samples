//
// フラグメントシェーダー
//

uniform sampler2D uni_normal_map;
uniform sampler2D uni_color_map;

varying vec2 vary_uv;
varying vec3 vary_light_dir;


void main()
{
  // RGB値→法線ベクトル
  vec4 normal = texture2D(uni_normal_map, vary_uv) * 2.0 - 1.0;
  // 明るさを計算
  float i = max(dot(normalize(vary_light_dir), normal.xyz), 0.0);

  gl_FragColor = texture2D(uni_color_map, vary_uv) * i;
}
