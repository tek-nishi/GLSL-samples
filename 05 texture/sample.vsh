//
// バーテックスシェーダー
//

attribute vec4 position;
attribute vec2 uv;

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;

varying vec2 vary_uv;


void main()
{
  vary_uv = uv;
	gl_Position = projectionMatrix * modelViewMatrix * position;
}
