//
// バーテックスシェーダー
//

// CPU側から送られるデータ
attribute vec4 position;     // 頂点座標


// CPU側から贈られるデータ
uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;


void main()
{
	gl_Position = projectionMatrix * modelViewMatrix * position;
}
