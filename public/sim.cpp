#include <vector>
#include <cmath>
#include <emscripten/emscripten.h>

extern "C" {

// =========================
// 定数（SI単位）
// =========================
const double G = 6.67430e-11;

// =========================
struct Object {
    double x, y, z;      // m
    double vx, vy, vz;   // m/s
    double mass;         // kg
};

std::vector<Object> objs;
std::vector<double> positions;

// =========================
// 初期化（太陽＋地球）
// =========================
EMSCRIPTEN_KEEPALIVE
void init_simulation() {
    objs.clear();

    // 太陽
    objs.push_back({
        0,0,0,
        0,0,0,
        1.989e30
    });

    // 地球
    objs.push_back({
        1.5e11, 0, 0,        // 1AU
        0, 30000, 0,         // 約30km/s
        5.972e24
    });
}

// =========================
// 重力ステップ（SI）
// =========================
EMSCRIPTEN_KEEPALIVE
void step() {
    int n = objs.size();

    std::vector<double> ax(n,0), ay(n,0), az(n,0);

    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            if(i==j) continue;

            double dx = objs[j].x - objs[i].x;
            double dy = objs[j].y - objs[i].y;
            double dz = objs[j].z - objs[i].z;

            double r2 = dx*dx + dy*dy + dz*dz;

            // ソフトニング（発散防止）
            double eps = 1e9; // 1000km
            r2 += eps*eps;

            double r = sqrt(r2);

            double a = G * objs[j].mass / r2;

            ax[i] += a * dx / r;
            ay[i] += a * dy / r;
            az[i] += a * dz / r;
        }
    }

    double dt = 60.0; // 60秒

    for(int i=0;i<n;i++){
        objs[i].vx += ax[i] * dt;
        objs[i].vy += ay[i] * dt;
        objs[i].vz += az[i] * dt;

        objs[i].x += objs[i].vx * dt;
        objs[i].y += objs[i].vy * dt;
        objs[i].z += objs[i].vz * dt;
    }
}

// =========================
// 位置取得
// =========================
EMSCRIPTEN_KEEPALIVE
double* get_positions() {
    positions.clear();

    for(auto& o : objs){
        positions.push_back(o.x);
        positions.push_back(o.y);
        positions.push_back(o.z);
    }

    return positions.data();
}

EMSCRIPTEN_KEEPALIVE
int get_count() {
    return objs.size();
}

// =========================
// 重力ポテンシャル（SI）
// =========================
EMSCRIPTEN_KEEPALIVE
double getGridY(double x, double z) {
    double phi = 0;

    for(auto& o : objs){
        double dx = x - o.x;
        double dz = z - o.z;

        double r2 = dx*dx + dz*dz;

        double eps = 1e9;
        r2 += eps*eps;

        double r = sqrt(r2);

        phi += -G * o.mass / r;
    }

    return phi;
}

}
