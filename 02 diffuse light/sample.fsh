//
// フラグメントシェーダー
//

uniform vec4 color;

varying float intensity;         // フラグメントシェーダー側で更新する


void main()
{
  gl_FragColor = color * intensity;
}
