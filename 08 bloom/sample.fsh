//
// フラグメントシェーダー
//

uniform sampler2D uni_texture;

uniform vec3 uni_deffuse_dir;
uniform vec4 uni_deffuse;
uniform vec4 uni_ambient;
uniform vec4 uni_specular;
uniform float uni_shine;

varying vec3 vary_position;
varying vec3 vary_normal;
varying vec2 vary_uv;


void main()
{
  vec3 v = normalize(vary_position);
  vec3 n = normalize(vary_normal);

  // 拡散光の強さ
  float di = max(dot(uni_deffuse_dir, n), 0.0);

  // 光源ベクトルが反射した時のベクトル
  vec3 rl = reflect(-uni_deffuse_dir, n);
  // スペキュラの強さ
  float si = pow(max(dot(v, rl), 0.0), uni_shine);

  gl_FragColor = texture2D(uni_texture, vary_uv) * (uni_ambient + uni_deffuse * di) + uni_specular * si;
}
