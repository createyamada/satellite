#include <vector>
#include <cmath>
#include <emscripten/emscripten.h>

extern "C" {

struct Body {

    double x, y, z;
    double vx, vy, vz;
    double mass;

};

std::vector<Body> bodies;

const double G  = 6.67430e-11;
const double DT = 60.0 * 60.0 * 6.0;

// =====================================================
// add planet
// =====================================================

void add_planet(
    double distance,
    double velocity,
    double mass
){

    Body b;

    b.x = distance;
    b.y = 0;
    b.z = 0;

    b.vx = 0;
    b.vy = 0;
    b.vz = velocity;

    b.mass = mass;

    bodies.push_back(b);
}

// =====================================================
// init
// =====================================================

EMSCRIPTEN_KEEPALIVE
void init_simulation(){

    bodies.clear();

    // Sun
    bodies.push_back({
        0,0,0,
        0,0,0,
        1.989e30
    });

    add_planet(5.79e10, 47400, 3.30e23); // Mercury
    add_planet(1.082e11, 35000, 4.87e24); // Venus
    add_planet(1.496e11, 29780, 5.97e24); // Earth
    add_planet(2.279e11, 24077, 6.42e23); // Mars
    add_planet(7.785e11, 13070, 1.90e27); // Jupiter
    add_planet(1.433e12, 9680, 5.68e26); // Saturn
    add_planet(2.877e12, 6800, 8.68e25); // Uranus
    add_planet(4.503e12, 5430, 1.02e26); // Neptune
}

// =====================================================
// acceleration
// =====================================================

void compute_acceleration(
    const std::vector<Body>& state,
    std::vector<double>& ax,
    std::vector<double>& ay,
    std::vector<double>& az
){

    int n = state.size();

    ax.assign(n,0);
    ay.assign(n,0);
    az.assign(n,0);

    for(int i=0;i<n;i++){

        for(int j=0;j<n;j++){

            if(i==j) continue;

            double dx = state[j].x - state[i].x;
            double dy = state[j].y - state[i].y;
            double dz = state[j].z - state[i].z;

            double distSq =
                dx*dx + dy*dy + dz*dz + 1e6;

            double dist =
                sqrt(distSq);

            double force =
                G * state[j].mass /
                distSq;

            ax[i] += force * dx / dist;
            ay[i] += force * dy / dist;
            az[i] += force * dz / dist;
        }
    }
}

// =====================================================
// RK4 step
// =====================================================

EMSCRIPTEN_KEEPALIVE
void step(){

    int n = bodies.size();

    std::vector<double> ax1,ay1,az1;
    std::vector<double> ax2,ay2,az2;
    std::vector<double> ax3,ay3,az3;
    std::vector<double> ax4,ay4,az4;

    compute_acceleration(
        bodies,
        ax1,ay1,az1
    );

    std::vector<Body> s2 = bodies;

    for(int i=0;i<n;i++){

        s2[i].x += bodies[i].vx * DT * 0.5;
        s2[i].y += bodies[i].vy * DT * 0.5;
        s2[i].z += bodies[i].vz * DT * 0.5;

        s2[i].vx += ax1[i] * DT * 0.5;
        s2[i].vy += ay1[i] * DT * 0.5;
        s2[i].vz += az1[i] * DT * 0.5;
    }

    compute_acceleration(
        s2,
        ax2,ay2,az2
    );

    std::vector<Body> s3 = bodies;

    for(int i=0;i<n;i++){

        s3[i].x += s2[i].vx * DT * 0.5;
        s3[i].y += s2[i].vy * DT * 0.5;
        s3[i].z += s2[i].vz * DT * 0.5;

        s3[i].vx += ax2[i] * DT * 0.5;
        s3[i].vy += ay2[i] * DT * 0.5;
        s3[i].vz += az2[i] * DT * 0.5;
    }

    compute_acceleration(
        s3,
        ax3,ay3,az3
    );

    std::vector<Body> s4 = bodies;

    for(int i=0;i<n;i++){

        s4[i].x += s3[i].vx * DT;
        s4[i].y += s3[i].vy * DT;
        s4[i].z += s3[i].vz * DT;

        s4[i].vx += ax3[i] * DT;
        s4[i].vy += ay3[i] * DT;
        s4[i].vz += az3[i] * DT;
    }

    compute_acceleration(
        s4,
        ax4,ay4,az4
    );

    for(int i=0;i<n;i++){

        bodies[i].x +=
            DT * (
                bodies[i].vx +
                2.0*s2[i].vx +
                2.0*s3[i].vx +
                s4[i].vx
            ) / 6.0;

        bodies[i].y +=
            DT * (
                bodies[i].vy +
                2.0*s2[i].vy +
                2.0*s3[i].vy +
                s4[i].vy
            ) / 6.0;

        bodies[i].z +=
            DT * (
                bodies[i].vz +
                2.0*s2[i].vz +
                2.0*s3[i].vz +
                s4[i].vz
            ) / 6.0;

        bodies[i].vx +=
            DT * (
                ax1[i] +
                2.0*ax2[i] +
                2.0*ax3[i] +
                ax4[i]
            ) / 6.0;

        bodies[i].vy +=
            DT * (
                ay1[i] +
                2.0*ay2[i] +
                2.0*ay3[i] +
                ay4[i]
            ) / 6.0;

        bodies[i].vz +=
            DT * (
                az1[i] +
                2.0*az2[i] +
                2.0*az3[i] +
                az4[i]
            ) / 6.0;
    }
}

// =====================================================

EMSCRIPTEN_KEEPALIVE
int get_count(){

    return bodies.size();
}

// =====================================================

EMSCRIPTEN_KEEPALIVE
double* get_positions(){

    static std::vector<double> pos;

    pos.resize(bodies.size()*3);

    for(size_t i=0;i<bodies.size();i++){

        pos[i*3]   = bodies[i].x;
        pos[i*3+1] = bodies[i].y;
        pos[i*3+2] = bodies[i].z;
    }

    return pos.data();
}

// =====================================================

EMSCRIPTEN_KEEPALIVE
double* get_masses(){

    static std::vector<double> masses;

    masses.resize(bodies.size());

    for(size_t i=0;i<bodies.size();i++){

        masses[i] = bodies[i].mass;
    }

    return masses.data();
}

}