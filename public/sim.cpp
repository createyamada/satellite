#include <vector>
#include <cmath>
#include <emscripten/emscripten.h>

extern "C" {

const double G = 6.67430e-11;

struct Body {
    double x,y,z;
    double vx,vy,vz;
    double m;
};

std::vector<Body> bodies;
std::vector<double> pos;

// =========================
// 初期化（SI単位）
// =========================
EMSCRIPTEN_KEEPALIVE
void init_simulation(){

    bodies.clear();

    // ☀️ 太陽
    bodies.push_back({0,0,0,0,0,0,1.989e30});

    // 🌍 地球
    bodies.push_back({1.496e11,0,0,0,29780,0,5.972e24});

    // 🔴 火星
    bodies.push_back({2.279e11,0,0,0,24070,0,6.39e23});
}

// =========================
// ステップ
// =========================
EMSCRIPTEN_KEEPALIVE
void step(){

    int n = bodies.size();
    double dt = 3600; // 1時間

    std::vector<double> ax(n,0), ay(n,0), az(n,0);

    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            if(i==j) continue;

            double dx = bodies[j].x - bodies[i].x;
            double dy = bodies[j].y - bodies[i].y;
            double dz = bodies[j].z - bodies[i].z;

            double r2 = dx*dx + dy*dy + dz*dz + 1e18;
            double r = sqrt(r2);

            double a = G * bodies[j].m / r2;

            ax[i] += a * dx / r;
            ay[i] += a * dy / r;
            az[i] += a * dz / r;
        }
    }

    for(int i=0;i<n;i++){
        bodies[i].vx += ax[i]*dt;
        bodies[i].vy += ay[i]*dt;
        bodies[i].vz += az[i]*dt;

        bodies[i].x += bodies[i].vx*dt;
        bodies[i].y += bodies[i].vy*dt;
        bodies[i].z += bodies[i].vz*dt;
    }
}

// =========================
// 位置取得
// =========================
EMSCRIPTEN_KEEPALIVE
double* get_positions(){
    pos.clear();
    for(auto &b: bodies){
        pos.push_back(b.x);
        pos.push_back(b.y);
        pos.push_back(b.z);
    }
    return pos.data();
}

EMSCRIPTEN_KEEPALIVE
int get_count(){
    return bodies.size();
}

// =========================
// 重力ポテンシャル
// =========================
EMSCRIPTEN_KEEPALIVE
double getPotential(double x,double z){

    double phi = 0;

    for(auto &b: bodies){
        double dx = x - b.x;
        double dz = z - b.z;

        double r = sqrt(dx*dx + dz*dz + 1e18);

        phi += -G * b.m / r;
    }

    return phi;
}

EMSCRIPTEN_KEEPALIVE
double* get_masses(){
    static std::vector<double> masses;
    masses.clear();

    for(auto &b : bodies){
        masses.push_back(b.m);
    }
    return masses.data();
}

}