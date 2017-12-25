uniform sampler2D uni_texture;

varying vec2 vary_uv;


void main()
{
  gl_FragColor = texture2D(uni_texture, vary_uv);
}
