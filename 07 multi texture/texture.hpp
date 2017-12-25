#pragma once

//
// 画像を読み込む
//

// 画像を読み込む
GLuint createTexture(std::string path) {
  // テクスチャを１つ生成
  GLuint id;
  glGenTextures(1, &id);

  // テクスチャを拘束
  // NOTICE 以下テクスチャに対する命令は拘束したテクスチャに対して実行される
  glBindTexture(GL_TEXTURE_2D, id);

  // 画像を読み込む
  int width;
  int height;
  int comp;
  unsigned char *data = stbi_load(path.c_str(), &width, &height, &comp, 0);

  // アルファの有無でデータ形式が異なる
  GLint type = (comp == 3) ? GL_RGB
                           : GL_RGBA;

  // 画像データをOpenGLへ転送
  glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type, GL_UNSIGNED_BYTE, data);
  stbi_image_free(data);
  
  // 表示用の細々とした設定
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  return id;
}
