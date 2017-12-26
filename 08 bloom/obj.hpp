#pragma once

//
// OBJ形式を読む
//  頂点座標＋法線ベクトル＋テクスチャ座標
//

#include <vector>
#include <sstream>


// 読み込んだOBJ形式の情報
// NOTICE:OpenGLにそのまま渡してglDrawArraysで描画する
//        構造になっている
struct Obj {
  std::vector<GLfloat> vtx;
  std::vector<GLfloat> texture;
  std::vector<GLfloat> normal;

  bool has_texture;
  bool has_normal;
};

// 面を構成する頂点情報
struct Face {
  int vi[3];           // 頂点番号
  int ti[3];           // テクスチャ座標番号
  int ni[3];           // 法線ベクトル番号
};

// OBJ形式を読み込む
Obj loadObj(const std::string& path) {
  std::ifstream stream(path);
  assert(stream);

  // ファイルから読み取った値を一時的に保持しておく変数
  std::vector<GLfloat> vtx;
  std::vector<GLfloat> normal;
  std::vector<GLfloat> texture;
  std::vector<Face>    face;

  // 変換後の値
  Obj obj;

  // NOTICE OBJ形式はデータの並びに決まりがないので、
  //        いったん全ての情報をファイルから読み取る
  while (!stream.eof()) {
    std::string s;
    std::getline(stream, s);

    // TIPS:文字列ストリームを使い
    //      文字列→数値のプログラム負荷を下げている
    std::stringstream ss(s);
    std::string key;
    ss >> key;
    if (key == "v") {
      // 頂点座標
      float x, y, z;
      ss >> x >> y >> z;
      vtx.push_back(x);
      vtx.push_back(y);
      vtx.push_back(z);
    }
    else if (key == "vt") {
      // 頂点テクスチャ座標
      float u, v;
      ss >> u >> v;
      texture.push_back(u);
      texture.push_back(v);
    }
    else if (key == "vn") {
      // 頂点法線ベクトル
      float x, y, z;
      ss >> x >> y >> z;
      normal.push_back(x);
      normal.push_back(y);
      normal.push_back(z);
    }
    else if (key == "f") {
      // 面を構成する頂点情報
      // FIXME ３角形のみの対応

      // TIPS 初期値を-1にしておき、
      //      「データが存在しない」状況に対応
      Face f = {
        { -1, -1, -1 },
        { -1, -1, -1 },
        { -1, -1, -1 }
      };

      for (int i = 0; i < 3; ++i) {
        std::string text;
        ss >> text;
        // TIPS '/' で区切られた文字列から数値に変換するため
        //      一旦stringstreamへ変換
        std::stringstream fs(text);

        // 面を構成する頂点番号、法線ベクトル番号を取り出す
        {
          std::string v;
          std::getline(fs, v, '/');
          f.vi[i] = std::stoi(v) - 1;
        }
        {
          std::string v;
          std::getline(fs, v, '/');
          if (!v.empty()) {
            f.ti[i] = std::stoi(v) - 1;
          }
        }
        {
          std::string v;
          std::getline(fs, v, '/');
          if (!v.empty()) {
            f.ni[i] = std::stoi(v) - 1;
          }
        }
      }
      face.push_back(f);
    }
  }

  // NOTICE 配列の要素番号が０以上なら、テクスチャ座標が有効
  obj.has_texture = face[0].ti[0] >= 0;
  // NOTICE 配列の要素番号が０以上なら、法線ベクトルが有効
  obj.has_normal = face[0].ni[0] >= 0;

  // 読み込んだ面情報から最終的な頂点配列を作成
  for (auto& f : face) {
    for (int i = 0; i < 3; ++i) {
      {
        // 頂点座標
        int index = f.vi[i] * 3;
        for (int j = 0; j < 3; ++j) {
          obj.vtx.push_back(vtx[index + j]);
        }
      }
      if (obj.has_texture) {
        // テクスチャ座標
        int index = f.ti[i] * 2;
        for (int j = 0; j < 2; ++j) {
          obj.texture.push_back(texture[index + j]);
        }
      }
      if (obj.has_normal) {
        // 法線ベクトル
        int index = f.ni[i] * 3;
        for (int j = 0; j < 3; ++j) {
          obj.normal.push_back(normal[index + j]);
        }
      }
    }
  }

  // テクスチャ座標のYを反転する
  for (int i = 1; i < obj.texture.size(); i += 2) {
    obj.texture[i] = -obj.texture[i];
  }
  

  return obj;
}
