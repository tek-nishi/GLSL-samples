//
// フラグメントシェーダー
//

uniform vec4 uni_color;

varying vec4 vary_color;


void main()
{
  gl_FragColor = uni_color * vary_color;
}
