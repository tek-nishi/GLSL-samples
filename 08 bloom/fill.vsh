// CPU側から送られるデータ
attribute vec4 position;     // 頂点座標
attribute vec2 uv;

varying vec2 vary_uv;


void main()
{
  vary_uv = uv;

	gl_Position = position;
}
