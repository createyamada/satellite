#include <vector>
#include <cmath>
#include <emscripten/emscripten.h>

extern "C" {

// =========================
// 定数
// =========================
const double G = 6.6743e-11;

// =========================
// オブジェクト構造体
// =========================
struct Object {
    double x, y, z;
    double vx, vy, vz;
    double mass;
};

// =========================
// シミュレーションデータ
// =========================
std::vector<Object> objs;

// JSに渡す用バッファ
std::vector<double> positions;

// =========================
// 初期化
// =========================
EMSCRIPTEN_KEEPALIVE
void init_simulation() {
    objs.clear();

    // 太陽っぽいもの
    objs.push_back({0, 0, 0, 0, 0, 0, 1.989e25});

    // 2つの惑星
    objs.push_back({-5000, 0, 0, 0, 0, 1200, 5.972e22});
    objs.push_back({5000, 0, 0, 0, 0, -1200, 5.972e22});
}

// =========================
// 重力計算（1ステップ）
// =========================
EMSCRIPTEN_KEEPALIVE
void step() {
    int n = objs.size();

    // 加速度配列
    std::vector<double> ax(n, 0), ay(n, 0), az(n, 0);

    // 全ペア計算
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j) continue;

            double dx = objs[j].x - objs[i].x;
            double dy = objs[j].y - objs[i].y;
            double dz = objs[j].z - objs[i].z;

            double dist = sqrt(dx*dx + dy*dy + dz*dz) + 1e-6;

            double force = G * objs[i].mass * objs[j].mass / (dist * dist);

            double acc = force / objs[i].mass;

            ax[i] += acc * dx / dist;
            ay[i] += acc * dy / dist;
            az[i] += acc * dz / dist;
        }
    }

    // 速度・位置更新
    for (int i = 0; i < n; i++) {
        objs[i].vx += ax[i];
        objs[i].vy += ay[i];
        objs[i].vz += az[i];

        objs[i].x += objs[i].vx * 0.01;
        objs[i].y += objs[i].vy * 0.01;
        objs[i].z += objs[i].vz * 0.01;
    }
}

// =========================
// JSに位置配列を渡す
// =========================
EMSCRIPTEN_KEEPALIVE
double* get_positions() {
    positions.clear();

    for (auto& o : objs) {
        positions.push_back(o.x);
        positions.push_back(o.y);
        positions.push_back(o.z);
    }

    return positions.data();
}

// =========================
// オブジェクト数取得
// =========================
EMSCRIPTEN_KEEPALIVE
int get_count() {
    return objs.size();
}


EMSCRIPTEN_KEEPALIVE
double getGridY(double x, double z) {
    double y = 0;

    for (auto& o : objs) {
        double dx = x - o.x;
        double dz = z - o.z;

        double dist = sqrt(dx*dx + dz*dz) + 1e-6;

        // 重力ポテンシャル（マイナス）
        y -= o.mass / dist;
    }

    return y;
}

// =========================
// オブジェクト追加（JSから）
// =========================
EMSCRIPTEN_KEEPALIVE
void add_object(double x, double y, double z,
                double vx, double vy, double vz,
                double mass) {

    objs.push_back({x, y, z, vx, vy, vz, mass});
}

}
