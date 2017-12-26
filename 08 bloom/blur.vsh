// CPU側から送られるデータ
attribute vec4 position;     // 頂点座標


void main()
{
	gl_Position = position;
}
